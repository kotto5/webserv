#include "CgiHandler.hpp"
#include "ServerException.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <filesystem>

// Constructors
CgiHandler::CgiHandler()
{
	_status = "200";
}

CgiHandler::CgiHandler(const CgiHandler &other): _server(other._server)
{
	*this = other;
}

// Destructor
CgiHandler::~CgiHandler() {}

// Operators
CgiHandler &CgiHandler::operator=(const CgiHandler &rhs)
{
	if (this != &rhs)
	{
		_status = rhs._status;
		_server = rhs._server;
	}
	return *this;
}

void	CgiHandler::init(Server &server, const LocationContext &lc)
{
	_server = &server;
	_locationContext = &lc;
}

/**
 * @brief CGIの呼び出し処理を行う
 *
 * @param request
 * @return Response*
 */
Response *CgiHandler::handleRequest(const Request &request)
{
	// ソケットペアを作成
	int	socks[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1)
	{
		throw ServerException("socketpair");
	}

	// CGIを実行
	if (runCgi(request, socks))
	{
		throw ServerException("runCgi");
	}
	close(socks[S_CHILD]);

	set_non_blocking(socks[S_PARENT]);
	Socket *cgiSock = new Socket(socks[S_PARENT]);
	_server->addCgi(cgiSock, _clientSocket);
	if (request.getBody().size() != 0)
	{
		// リクエストボディがある場合はCGIに送信する
		Request *req = new Request(request.getBody());
		_server->addSend(cgiSock, req);
	}
	else
	{
		// リクエストボディがない場合はEOFを送信する
		shutdown(socks[S_PARENT], SHUT_WR);
		HttpMessage *res = new Response();
		_server->addRecv(cgiSock, res);
	}
	// レスポンスを受信する
	return NULL;
}

/**
 * @brief CGIを実行する
 *
 * @param request
 * @param pipes
 * @return int
 */
int CgiHandler::runCgi(const Request &request, int pipes[2])
{
	// スクリプトのURIを取得
    std::string script = request.getUri();
	// 環境変数を整形
	std::vector<char *> envs = createEnvs(request);

	int pid = fork();
	if (pid == -1)
	{
		throw ServerException("execve failed");
	}
	else if (pid == 0)
	{
        std::cout << "sockRecv: " << pipes[S_CHILD] << std::endl;
        std::cout << "sockSend: " << pipes[S_PARENT] << std::endl;
        if (close(pipes[S_PARENT]) ||
			dup2(pipes[S_CHILD], STDOUT_FILENO) == -1 ||
        	dup2(pipes[S_CHILD], STDIN_FILENO) == -1 ||
	        close(pipes[S_CHILD]))
			exit(1);
        std::string path = request.getActualUri();
		std::string path_query = path;

		char *cgi_pass = const_cast<char *>(_locationContext->getDirective("cgi_pass").c_str());
		char *argv[] = {cgi_pass, const_cast<char *>(path_query.c_str()), NULL};

		// プログラム呼び出し
		execve(cgi_pass, argv, (char* const*)(envs.data()));
        perror(path_query.c_str());
        exit(1);
	}
    return 0;
}

/**
 * @brief CGIに渡す環境変数を整形する
 *
 * @param request
 * @return std::vector<char *>
 */
std::vector<char *> CgiHandler::createEnvs(const Request &request)
{
    std::vector<std::string> envs;

    envs.push_back("AUTH_TYPE=" + request.getHeader("auth-scheme"));
    envs.push_back("CONTENT_LENGTH=" + std::to_string(request.getBody().length()));
    // envs.push_back("CONTENT_TYPE=" + request->getContentType());
    envs.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // std::filesystem::path p{"../../docs"};
    // if (std::filesystem::exists(p))
    //     envs.push_back("PATH_TRANSLATED=" + std::filesystem::absolute(p).string());
    envs.push_back("PATH_TRANSLATED=");

    std::string uri = request.getUri();
    // std::string filename = uri.substr(uri.find_last_of('/') + 1);
    std::string filename = ".php";
    std::string target = ".php"; // file name
    size_t pos = uri.find(".php");
    if (pos != std::string::npos)
        envs.push_back("PATH_INFO=" + uri.substr(pos + target.size()));

    envs.push_back("QUERY_STRING=" + (request.getUri().find('?') != std::string::npos
		? request.getUri().substr(request.getUri().find('?') + 1)
		: ""));
    // envs.push_back("REMOTE_ADDR=" + request->getRemoteAddr()); // MUST
    // envs.push_back("REMOTE_IDENT=" + request->getRemoteIdent()); // SHOULD
    // envs.push_back("REMOTE_USER=" + request->getRemoteUser()); // SHOULD
    envs.push_back("REQUEST_METHOD=" + request.getMethod());
    envs.push_back("REQUEST_URI=" + request.getUri());
    envs.push_back("SCRIPT_NAME=" + filename);
    // envs.push_back("SERVER_NAME=" + request->getServerName());
    // envs.push_back("SERVER_PORT=" + std::to_string(request->getServerPort()));
    envs.push_back("SERVER_PROTOCOL=" + request.getProtocol());
    // envs.push_back("SERVER_SOFTWARE=" + request->getServerSoftware());
    envs.push_back("HTTP_ACCEPT=" + request.getHeader("Accept")); //////////

	// char型に一括置換
    std::vector<char *> cenvs;
    std::vector<std::string>::iterator it = envs.begin();
    for (; it != envs.end(); it++)
        cenvs.push_back(const_cast<char*>(it->c_str()));
    cenvs.push_back(nullptr); // execveには最後にnullポインタが必要

	return cenvs;
}

void CgiHandler::setClientSocket(Socket *clientSocket)
{
	_clientSocket = clientSocket;
}
