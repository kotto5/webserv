#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include "server.hpp"
#include <map>
#include <vector>

void set_non_blocking(int socket) {
	if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
		perror("ERROR on fcntl");
		exit(1);
	}
}

size_t	find_start(const std::string &str, size_t start, const std::string &target){
	std::string substr = str.substr(start);
	return (substr.find(target));
}

void partitionAndAddToMap(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword) {
	size_t pos = inputStr.find(keyword);

	if (pos != std::string::npos) {
		std::string part1 = inputStr.substr(0, pos);
		std::string part2 = inputStr.substr(pos + keyword.length());
		m.insert(std::map<std::string, std::string>::value_type(part1, part2));
	}
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

#include <filesystem>

int	runCgi(Request *request, int socket)
{
    std::string script = request->getUri();

    std::vector<std::string> envs;
    envs.push_back("AUTH_TYPE=Basic");
    envs.push_back("CONTENT_LENGTH=" + std::to_string(request->getBody().length()));
    // envs.push_back("CONTENT_TYPE=" + request->getContentType());
    envs.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // std::filesystem::path p{"../../docs"};

    // if (std::filesystem::exists(p))
    //     envs.push_back("PATH_TRANSLATED=" + std::filesystem::absolute(p).string());
    envs.push_back("PATH_TRANSLATED=");

    std::string uri = request->getUri();
    // std::string filename = uri.substr(uri.find_last_of('/') + 1);
    std::string filename = ".php";
    std::string target = ".php"; // file name
    size_t pos = uri.find(".php");
    if (pos != std::string::npos)
        envs.push_back("PATH_INFO=" + uri.substr(pos + target.size()));

    envs.push_back("QUERY_STRING=" + (request->getUri().find('?') != std::string::npos ? request->getUri().substr(request->getUri().find('?') + 1) : ""));
    // envs.push_back("REMOTE_ADDR=" + request->getRemoteAddr()); // MUST
    // envs.push_back("REMOTE_IDENT=" + request->getRemoteIdent()); // SHOULD
    // envs.push_back("REMOTE_USER=" + request->getRemoteUser()); // SHOULD
    envs.push_back("REQUEST_METHOD=" + request->getMethod());
    envs.push_back("REQUEST_URI=" + request->getUri());
    envs.push_back("SCRIPT_NAME=" + filename);
    // envs.push_back("SERVER_NAME=" + request->getServerName());
    // envs.push_back("SERVER_PORT=" + std::to_string(request->getServerPort()));
    envs.push_back("SERVER_PROTOCOL=" + request->getProtocol());
    // envs.push_back("SERVER_SOFTWARE=" + request->getServerSoftware());
    // envs.push_back("HTTP_ACCEPT=" + request->getAccept());


    std::vector<char*> cenvs;
    std::vector<std::string>::iterator it = envs.begin();
    for (; it != envs.end(); it++)
        cenvs.push_back(const_cast<char*>(it->c_str()));
    cenvs.push_back(nullptr); // execveには最後にnullポインタが必要です

    // // execveを呼び出す際にcenvs.data()を環境変数として渡します
    // char *argv[] = { "/path/to/your/command", nullptr };
    // if (execve(argv[0], argv, cenvs.data()) == -1) {
    //     perror("execve failed");
    // }

    // // メモリを開放します
    // for (auto p : cenvs) {
    //     free(p);
    // }


	int	pid = fork();
	if (pid == -1)
        return (ERROR);
	else if (pid == 0)
	{
        (void)request;
		dup2(socket, 0);
		dup2(socket, 1);
		close(socket);
		std::string path = "/Users/kakiba/AAproject/42_webserv/docs/index.php";
		// std::string query = request.getQuery();
		// std::string path_query = path + "?" + query;
		std::string path_query = path;
		char	*php_path = (char *)"/usr/bin/php";
		// char *argv[] = {php_path, const_cast<char *>(path_query.c_str()), const_cast<char* const*>(cenvs.data())};
		char *argv[] = {php_path, const_cast<char *>(path_query.c_str())};
		execve(php_path, argv, (char* const*)(cenvs.data()));
		perror("execve");
		exit(1);
	}
    return (0);
}

int isValidFd(int fd) {
    return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}
