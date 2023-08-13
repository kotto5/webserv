#include "CgiHandler.hpp"
#include "ServerException.hpp"
#include "utils.hpp"
#include "server.hpp"
#include "Socket.hpp"
#include <filesystem>

// Constructors
CgiHandler::CgiHandler()
{
	_status = "200";
}

CgiHandler::CgiHandler(const CgiHandler &other)
{
	_status = other._status;
}

// Destructor
CgiHandler::~CgiHandler() {}

// Operators
CgiHandler &CgiHandler::operator=(const CgiHandler &rhs)
{
	if (this != &rhs)
	{
		_status = rhs._status;
	}
	return *this;
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
	int	socks[2][2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks[0]) == -1)
	{
		perror("socketpair");
		throw ServerException("socketpair");
	}
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks[1]) == -1)
	{
		perror("socketpair");
		throw ServerException("socketpair");
	}

	// CGIを実行
	if (runCgi(request, socks))
	{
		throw ServerException("runCgi");
	}
	close(socks[0][S_CHILD]);
	close(socks[1][S_CHILD]);
	set_non_blocking(socks[0][S_PARENT]);
	set_non_blocking(socks[1][S_PARENT]);

	if (request.getBody().size() != 0)
	{
		// FDからソケットを新規作成
		Socket *sockSend = new Socket(socks[0][S_PARENT]);
		setFd(TYPE_SEND, sockSend);
		Sends[sockSend] = new Request(request.getBody());
	}
	else
		close(socks[0][S_PARENT]);
	Socket *sockRecv = new Socket(socks[1][S_PARENT]);
	setFd(TYPE_RECV, sockRecv);
	setFd(TYPE_CGI, sockRecv, _clientSocket);
	return new Response();
	// Recvs[sockRecv] = new Response();
}

void CgiHandler::setClientSocket(Socket clientSocket)
{
	_clientSocket = clientSocket;
}

/**
 * @brief CGIを実行する
 *
 * @param request
 * @param pipes
 * @return int
 */
int CgiHandler::runCgi(const Request &request, int pipes[2][2])
{
	// スクリプトのURIを取得
    std::string script = request.getUri();
	// 環境変数を整形する
	std::vector<char *> envs = createEnvs(request);

    // // execveを呼び出す際にcenvs.data()を環境変数として渡します
    // char *argv[] = { "/path/to/your/command", nullptr };
    // if (execve(argv[0], argv, cenvs.data()) == -1) {
    //     perror("execve failed");
    // }

    // // メモリを開放します
    // for (auto p : cenvs) {
    //     free(p);
    // }

	if (fork() == 0)
	{
        std::cout << "sockRecv: " << pipes[0][0] << std::endl;
        std::cout << "sockSend: " << pipes[1][1] << std::endl;
        close(pipes[0][S_PARENT]);
        close(pipes[1][S_PARENT]);
        if (dup2(pipes[0][S_CHILD], STDIN_FILENO) == -1)
            perror("dup2 recv");
        if (dup2(pipes[1][S_CHILD], STDOUT_FILENO) == -1)
            perror("dup2 send");
        close(pipes[0][S_CHILD]);
        close(pipes[1][S_CHILD]);
        std::string path = request.getActualUri();
		// std::string query = request.getQuery();
		// std::string path_query = path + "?" + query;
		std::string path_query = path;
		char	*php_path = (char *)"/usr/bin/php";
		char *argv[] = {php_path, const_cast<char *>(path_query.c_str()), NULL};
		// プログラム呼び出し
		execve(php_path, argv, (char* const*)(envs.data()));
        perror(path_query.c_str());
        exit(1);
		throw ServerException("execve failed");
	}
	else
	{
		return -1;
	}
    return (0);
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