#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include "server.hpp"
#include <map>
#include "ErrorCode.hpp"
#include <vector>
#include <sys/stat.h>
#include "ConfigParser.hpp"
#include "HTTPContext.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include "ConfigException.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

void set_non_blocking(int socket){
	if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1)
	{
		throw ServerException("fcntl");
	}
}

size_t	find_start(const std::string &str, size_t start, const std::string &target){
	std::string substr = str.substr(start);
	return (substr.find(target));
}

// redefine socketpair function
// 1 : make socket it listen()
// 2 : make socket it connect()
// 3 : make socket from accept()
// if all system call is success, return 0


int _socketpair(int domain, int type, int protocol, int sv[2]) {
    int listener;
    struct sockaddr_in name, client_name;
    socklen_t client_len = sizeof(client_name);

    // 1. Create a socket.
    listener = socket(domain, SOCK_STREAM, protocol);
    if (listener == -1) {
        return -1;
    }

    // 2. Bind the socket to an address.
    memset(&name, 0, sizeof(name));
    name.sin_family = domain;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(listener, (struct sockaddr *)&name, sizeof(name)) == -1) {
        perror("bind");
        close(listener);
        return -1;
    }

    // 3. Listen on the socket.
    if (listen(listener, 1) == -1) {
        close(listener);
        return -1;
    }

    // 4. Get the port number.
    socklen_t name_len = sizeof(name);
    if (getsockname(listener, (struct sockaddr *)&name, &name_len) == -1) {
        close(listener);
        return -1;
    }

    // 5. Create and connect the second socket.
    sv[1] = socket(domain, type, protocol);
    if (sv[1] == -1) {
        close(listener);
        return -1;
    }
    if (connect(sv[1], (struct sockaddr *)&name, name_len) == -1) {
        close(listener);
        close(sv[1]);
        return -1;
    }

    // 6. Accept the connection on the listener socket.
    sv[0] = accept(listener, (struct sockaddr *)&client_name, &client_len);
    if (sv[0] == -1) {
        close(listener);
        close(sv[1]);
        return -1;
    }

    // Close the listener socket as we no longer need it.
    close(listener);
    return 0;
}

// https://tex2e.github.io/rfc-translater/html/rfc3875.html#4-1--Request-Meta-Variables

int runCgi(Request *request, int pipes[2][2])
{
    const std::string &uri = request->getUri();
    std::vector<std::string> envs;
    envs.push_back("AUTH_TYPE=" + request->getHeader("auth-scheme"));
    // requestBody の長さ
    envs.push_back("CONTENT_LENGTH=" + std::to_string(request->getBody().length()));
    // requestBody の content-type
    envs.push_back("CONTENT_TYPE=" + request->getHeader("content-type"));
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
    envs.push_back("QUERY_STRING=" + request->getQuery());
    envs.push_back("REMOTE_ADDR=" + request->getRemoteAddr()); // MUST
    envs.push_back("REQUEST_METHOD=" + request->getMethod());
    std::string::size_type  startScriptName = uri.find_last_of("/", lastCgiExtention);
    std::cout << "startScriptName: " << startScriptName << std::endl;
    std::cout << "startScriptName: " << uri.substr(startScriptName + 1) << std::endl;
    envs.push_back("SCRIPT_NAME=" + uri.substr(startScriptName + 1));
    envs.push_back("SERVER_NAME=" + request->getServerName());
    envs.push_back("SERVER_PORT=" + request->getServerPort());
    envs.push_back("SERVER_PROTOCOL=" + request->getProtocol());
    envs.push_back("SERVER_SOFTWARE=WakeWakame/1.0");

    envs.push_back("HTTP_ACCEPT=" + request->getHeader("accept"));
    envs.push_back("HTTP_ACCEPT=" + request->getHeader("Host")); // not must
    envs.push_back("HTTP_ACCEPT=" + request->getHeader("User-Agent")); // not must

    std::vector<char*> cenvs;
    std::vector<std::string>::iterator it = envs.begin();
    for (; it != envs.end(); it++)
        cenvs.push_back(const_cast<char*>(it->c_str()));
    cenvs.push_back(nullptr); // execveには最後にnullポインタが必要です

	int	pid = fork();
	if (pid == -1)
        return (ERROR);
	else if (pid == 0)
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
        std::string path = request->getActualUri();
        // request class has "uriAndQuery" and "uri" and "query"
        // uriAndQuery is full uri
        // uriAndQuery is splitted to uri and query by "?"
        // uri?variable=1&variable=2&variable=3
        // querys = split by "&" from query
        // querys = ["variable=1", "variable=2", "variable=3"]
        // please make "querys" Vector variable from "query" variable with split by "&"
        // so you have to set value by split by "&" from "query" variable

        std::vector<std::string> querys;
        std::string::size_type  start = 0;
        std::string::size_type  end = 0;
        while (true)
        {
            end = path.find("&", start);
            if (end == std::string::npos)
            {
                querys.push_back(path.substr(start));
                break;
            }
            querys.push_back(path.substr(start, end - start));
            start = end + 1;
        }

		char	*php_path = (char *)"/usr/bin/php";
        char    **argv = new char*[querys.size() + 3];
        argv[0] = php_path;
        argv[1] = const_cast<char *>(path.c_str());
        for (size_t i = 0; i < querys.size(); i++)
            argv[i + 2] = const_cast<char *>(querys[i].c_str());
        argv[querys.size() + 2] = nullptr;

		execve(php_path, argv, (char* const*)(cenvs.data()));
        perror(path.c_str());
        exit(1);
		throw ServerException("execve failed");
	}
    return (0);
}

int isValidFd(int fd) {
    return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}

#include <cstdlib>

bool    isFile(const char *path)
{
	struct stat st;

	return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

bool    isDirectory(const char *path)
{
	struct stat st;

	return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

bool    pathExist(const char *path)
{
	return (access(path, F_OK) == 0);
}

bool	isReadable(const char *path)
{
	return (access(path, R_OK) == 0);
}

bool	isWritable(const char *path)
{
	return (access(path, W_OK) == 0);
}

// % decoding function
std::string percentDecode(std::string str)
{
    std::string ret;
    std::string::iterator itr = str.begin();
    std::string::iterator end = str.end();
    while (itr != end)
    {
        if (*itr == '%')
        {
            itr++;
            if (itr == end)
                break;
            char c = *itr;
            itr++;
            if (itr == end)
                break;
            char d = *itr;
            itr++;
            if ('0' <= c && c <= '9')
                c -= '0';
            else if ('a' <= c && c <= 'f')
                c -= 'a' - 10;
            else if ('A' <= c && c <= 'F')
                c -= 'A' - 10;
            else
                break;
            if ('0' <= d && d <= '9')
                d -= '0';
            else if ('a' <= d && d <= 'f')
                d -= 'a' - 10;
            else if ('A' <= d && d <= 'F')
                d -= 'A' - 10;
            else
                break;
            ret += (c << 4) + d;
        }
        else
        {
            ret += *itr;
            itr++;
        }
    }
    return ret;
}

// encoding function
std::string percentEncode(std::string str)
{
    std::string ret;
    std::string::iterator itr = str.begin();
    std::string::iterator end = str.end();
    while (itr != end)
    {
        if ('0' <= *itr && *itr <= '9')
            ret += *itr;
        else if ('a' <= *itr && *itr <= 'z')
            ret += *itr;
        else if ('A' <= *itr && *itr <= 'Z')
            ret += *itr;
        else if (*itr == '-' || *itr == '_' || *itr == '.' || *itr == '~')
            ret += *itr;
        else
        {
            ret += '%';
            ret += "0123456789ABCDEF"[(*itr >> 4) & 0x0f];
            ret += "0123456789ABCDEF"[(*itr) & 0x0f];
        }
        itr++;
    }
    return ret;
}

int strtoi(std::string str)
{
	int ret = 0;
	for (std::string::iterator itr = str.begin(); itr != str.end(); itr++)
	{
		ret *= 10;
		ret += *itr - '0';
	}
	return (ret);
}
