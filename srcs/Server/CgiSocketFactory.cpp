#include "CgiSocketFactory.hpp"
#include "utils.hpp"
#include <arpa/inet.h>

std::string getAbsolutePath(const std::string& relativePath) {
    char realPath[PATH_MAX];
    if (realpath(relativePath.c_str(), realPath)) {
        return std::string(realPath);
    } else {
        // エラーハンドリング（realpathが失敗した場合）
        return std::string("");
    }
}

std::string makePathInfo(const std::string& str) {
    std::string::size_type pos = str.find(".php");
    if (pos == std::string::npos) {
        return ""; // ".php" が見つからない場合、空の文字列を返す
    }
    pos += 4; // ".php" の長さ分だけ位置を移動

    // pos 以降の文字列を返す
    if (pos < str.size()) {
        return str.substr(pos);
    } else {
        return "";
    }
}

std::string makeScriptName(const std::string& str) {
    std::string::size_type pos = str.rfind(".php");
    if (pos == std::string::npos) {
        return "";
    }
    std::string::size_type startPos = str.rfind('/', pos);
    if (startPos == std::string::npos) {
        return "";
    }
	startPos++;
    return str.substr(startPos, pos + 4 - startPos);
}

std::string makeDirectory(const std::string& str) {
    std::string::size_type pos = str.rfind(".php");
    if (pos == std::string::npos) {
        return "";
    }
    std::string::size_type endPos = str.rfind('/', pos);
    if (endPos == std::string::npos) {
        return "";
    }
    return str.substr(0, endPos);
}


std::vector<char *> *CgiSocketFactory::createEnvs(const Request &request)
{
    const std::string &uri = request.getUri();
    std::vector<std::string> envs;
    envs.push_back("AUTH_TYPE=" + request.getHeader("auth-scheme"));
    // requestBody の長さ
    envs.push_back("CONTENT_LENGTH=" + to_string(request.getBody().length()));
    // requestBody の content-type
    envs.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
    envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envs.push_back("REDIRECT_STATUS=");

    std::string cgiExtention = ".php";
    std::string::size_type  lastCgiExtention = uri.find_last_of(cgiExtention);
    std::string pathInfo = makePathInfo(uri);
    if (pathInfo.empty() == false)
    {
        envs.push_back("PATH_INFO=" + pathInfo);
        envs.push_back("PATH_TRANSLATED=" + getAbsolutePath(request.getActualUri()));
    }
    envs.push_back("QUERY_STRING=" + request.getQuery());
    envs.push_back("REMOTE_ADDR=" + request.getRemoteAddr()); // MUST
    envs.push_back("REQUEST_METHOD=" + request.getMethod());
    std::string::size_type  startScriptName = uri.find_last_of("/", lastCgiExtention);
	#ifdef TEST
		std::cout << "startScriptName: " << startScriptName << std::endl;
		std::cout << "startScriptName: " << uri.substr(startScriptName + 1) << std::endl;
	#endif
    envs.push_back("SCRIPT_NAME=" + uri.substr(startScriptName + 1));
    envs.push_back("SERVER_NAME=" + request.getServerName());
    envs.push_back("SERVER_PORT=" + request.getServerPort());
    envs.push_back("SERVER_PROTOCOL=" + request.getProtocol());
    envs.push_back("SERVER_SOFTWARE=WakeWakame/1.0");

    envs.push_back("HTTP_ACCEPT=" + request.getHeader("accept"));
    envs.push_back("HTTP_ACCEPT=" + request.getHeader("Host")); // not must
    envs.push_back("HTTP_ACCEPT=" + request.getHeader("User-Agent")); // not must

    std::vector<char*> *environVector = new std::vector<char*>();
    std::vector<std::string>::iterator it = envs.begin();
    for (; it != envs.end(); it++)
	{
		char *node = new char[it->length() + 1];
		strcpy(node, it->c_str());
		environVector->push_back(node);
	}
    environVector->push_back(NULL); // execveには最後にnullポインタが必要です

	return environVector;
}

int CgiSocketFactory::runCgi(const Request &request, int pipes[2])
{
	const LocationContext &locationContext = request.getLocationContext();
	// 環境変数を整形

	int pid = fork();
	if (pid == -1)
        return (1);
	else if (pid == 0)
	{
		try
		{
		    const std::string &uri = request.getActualUri();
			std::vector<char *> *envs = createEnvs(request);
			if (close(pipes[S_PARENT]) ||
				dup2(pipes[S_CHILD], STDOUT_FILENO) == -1 ||
				dup2(pipes[S_CHILD], STDIN_FILENO) == -1 ||
				close(pipes[S_CHILD]))
				exit(1);
			std::string path = makeScriptName(uri);
			chdir(makeDirectory(uri).c_str());
			std::string path_query = path;

			char cgi_pass_buf[1000]; // あるいは適切な最大サイズ
			strncpy(cgi_pass_buf, locationContext.getDirective("cgi_pass").c_str(), sizeof(cgi_pass_buf));
			cgi_pass_buf[sizeof(cgi_pass_buf) - 1] = '\0';

			char path_query_buf[1000];
			strncpy(path_query_buf, path_query.c_str(), sizeof(path_query_buf));
			path_query_buf[sizeof(path_query_buf) - 1] = '\0';

			char *argv[] = {cgi_pass_buf, path_query_buf, NULL};

			char * const *env = envs->data();
			execve(argv[0], argv, env);
			perror("execve: ");
			exit(1);
		}
		catch(const std::exception& e)
		{
			perror(e.what());
			exit(1);
		}
	}
    return 0;
}

CgiSocket *CgiSocketFactory::create(const Request &request, ClSocket *clientSocket)
{
	int	socks[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1)
		return (NULL);
	set_non_blocking(socks[S_PARENT]);
	CgiSocket *cgiSock = new(std::nothrow) CgiSocket(socks[S_PARENT], clientSocket);
	if (cgiSock == NULL)
	{
		close(socks[S_CHILD]);
		close(socks[S_PARENT]);
		return (NULL);
	}
	// CGIを実行
	if (runCgi(request, socks) == -1)
	{
		delete (cgiSock);
		close(socks[S_PARENT]);
		return (NULL);
	}
	close(socks[S_CHILD]);
	return (cgiSock);
}

CgiSocket *CgiSocketFactory::create(const CgiResponse &cgiResponse, ClSocket *clientSocket)
{
	Request *req = new(std::nothrow) Request();
	if (req == NULL)
		return (NULL);
	struct sockaddr_in addr = clientSocket->getLocalAddr();
	std::stringstream ss;
	ss << "GET " << cgiResponse.getHeader("Location") << " HTTP/1.1\r\n" <<
	"Host: " << std::string(inet_ntoa(addr.sin_addr)) << ":" << ntohs(addr.sin_port) << "\r\n"
	"\r\n";
	req->parsing(ss.str(), 0);
	req->setAddr(clientSocket);
	req->setInfo();
	CgiSocket *cgiSock = CgiSocketFactory::create(*req, clientSocket);
	delete (req);
	return (cgiSock);
}

CgiSocket *CgiSocketFactory::create(const HttpMessage &message, ClSocket *clientSocket)
{
	if (const Request *request = dynamic_cast<const Request *>(&message))
		return CgiSocketFactory::create(*request, clientSocket);
	else if (const CgiResponse *cgiResponse = dynamic_cast<const CgiResponse *>(&message))
		return CgiSocketFactory::create(*cgiResponse, clientSocket);
	else
		return (NULL);
}
