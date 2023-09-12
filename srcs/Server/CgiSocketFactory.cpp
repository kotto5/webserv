#include "CgiSocketFactory.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <signal.h>

std::string getAbsolutePath(const std::string& relativePath) {
    char realPath[PATH_MAX];
    if (realpath(relativePath.c_str(), realPath)) {
        return std::string(realPath);
    } else {
        // エラーハンドリング（realpathが失敗した場合）
        return std::string("");
    }
}

std::string makePathInfo(const std::string& str, const std::string &ext) {
    std::string::size_type pos = str.find(ext);
    if (pos == std::string::npos) {
        return "";
	}
    pos += ext.size(); // 長さ分だけ位置を移動

    // pos 以降の文字列を返す
    if (pos < str.size()) {
        return str.substr(pos);
    } else {
        return "";
    }
}

std::string makeScriptName(const std::string& str, const std::string &ext) {
	std::string::size_type pos = str.rfind(ext);
	if (pos == std::string::npos) {
		return "";
    }
    std::string::size_type startPos = str.rfind('/', pos);
    if (startPos == std::string::npos) {
        return "";
    }
	startPos++;
    return str.substr(startPos, pos + ext.size() - startPos);
}

std::string makeDirectory(const std::string& str, const std::string &ext) {
    std::string::size_type pos = str.rfind(ext);
    if (pos == std::string::npos) {
        return "";
    }
    std::string::size_type endPos = str.rfind('/', pos);
    if (endPos == std::string::npos) {
        return "";
    }
    return str.substr(0, endPos);
}

std::string CgiSocketFactory::extractExtension(const std::string& filename)
{
	std::size_t lastDot = filename.rfind('.');

	if (lastDot != std::string::npos)
	{
		return filename.substr(lastDot);
	}
	return "";
}

std::string CgiSocketFactory::extractFilename(const std::string& uri, const std::string& keyword)
{
	std::size_t pos = uri.find(keyword);

	if (pos != std::string::npos)
	{
		// キーワードの後に続く部分を取得
		std::string remaining = uri.substr(pos + keyword.length());

		// 次の'/'までがファイル名と仮定（存在しなければそのまま全てがファイル名）
		std::size_t nextSlash = remaining.find('/');
		if (nextSlash != std::string::npos)
		{
			return remaining.substr(0, nextSlash);
		}
		else
		{
			return remaining;
		}
	}
	return "";  // 見つからなかった場合
}

std::vector<char *> *CgiSocketFactory::createEnvs(const Request &request, const std::string &ext, const std::string &filename)
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

	std::string pathInfo = makePathInfo(uri, ext);
	if (pathInfo.empty() == false)
	{
		envs.push_back("PATH_INFO=" + pathInfo);
		envs.push_back("PATH_TRANSLATED=" + getAbsolutePath(makeDirectory(request.getActualUri(), ext)) + pathInfo);
	}
	envs.push_back("QUERY_STRING=" + request.getQuery());
	envs.push_back("REMOTE_ADDR=" + request.getRemoteAddr()); // MUST
	envs.push_back("REQUEST_METHOD=" + request.getMethod());
	envs.push_back("SCRIPT_NAME=" + filename);
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
	if (pid != 0)
		return (pid);
	else
	{
		try
		{
			const std::string &uri = request.getActualUri();
			std::string filename;
			if (locationContext.getDirective("alias") != "")
				filename = extractFilename(uri, locationContext.getDirective("alias"));
			else
				filename = extractFilename(uri, locationContext.getDirective("path"));
			const std::string cgiExtension = extractExtension(filename);
			std::vector<char *> *envs = createEnvs(request, cgiExtension, filename);
			if (close(pipes[S_PARENT]) ||
				dup2(pipes[S_CHILD], STDOUT_FILENO) == -1 ||
				dup2(pipes[S_CHILD], STDIN_FILENO) == -1 ||
				close(pipes[S_CHILD]))
				exit(1);
			std::string path = makeScriptName(uri, cgiExtension);
			chdir(makeDirectory(uri, cgiExtension).c_str());
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
}

CgiSocket *CgiSocketFactory::create(const Request &request, ClSocket *clientSocket)
{
	int	socks[2];
	int	pid;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1)
		return (NULL);
	set_non_blocking(socks[S_PARENT]);
	// CGIを実行
	pid = runCgi(request, socks);
	close(socks[S_CHILD]);
	if (pid == -1)
	{
		close(socks[S_PARENT]);
		return (NULL);
	}
	CgiSocket *cgiSock = new(std::nothrow) CgiSocket(socks[S_PARENT], pid, clientSocket);
	if (cgiSock == NULL)
	{
		close(socks[S_PARENT]);
		kill(pid, SIGKILL);
		return (NULL);
	}
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
