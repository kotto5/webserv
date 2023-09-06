#include "CgiSocketFactory.hpp"

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

    std::string cgiExtention = ".php";
    std::string::size_type  lastCgiExtention = uri.find_last_of(cgiExtention);
    std::string pathInfo = uri.substr(lastCgiExtention + 1);
    if (pathInfo.empty() == false)
    {
        envs.push_back("PATH_INFO=" + pathInfo);
        // std::filesystem::path   absolutePath = std::filesystem::absolute(pathInfo);
        // envs.push_back("PATH_TRANSLATED=" + absolutePath.string());
        envs.push_back("PATH_TRANSLATED=" + pathInfo);
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
    std::string script = request.getUri();
	// 環境変数を整形

	int pid = fork();
	if (pid == -1)
        return (1);
	else if (pid == 0)
	{
		try
		{
		std::vector<char *> *envs = createEnvs(request);
        if (close(pipes[S_PARENT]) ||
			dup2(pipes[S_CHILD], STDOUT_FILENO) == -1 ||
        	dup2(pipes[S_CHILD], STDIN_FILENO) == -1 ||
	        close(pipes[S_CHILD]))
			exit(1);
        std::string path = request.getActualUri();
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
        perror(path_query.c_str());
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
