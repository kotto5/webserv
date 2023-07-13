#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include "server.hpp"
#include <map>

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
    listener = socket(domain, type, protocol);
    if (listener == -1) {
        return -1;
    }

    // 2. Bind the socket to an address.
    memset(&name, 0, sizeof(name));
    name.sin_family = domain;
    name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    name.sin_port = 0;  // Use a random port.
    if (bind(listener, (struct sockaddr *)&name, sizeof(name)) == -1) {
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

int	connectCgi()
{
	int sv[2];
	if (_socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
	{
		perror("socketpair");
		return (-1);
	}
	int	pairent_sock = sv[0];
	int	child_sock = sv[1];

	int	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		close(pairent_sock);
		dup2(child_sock, 0);
		dup2(child_sock, 1);
		close(child_sock);
		std::string path = "/Users/kakiba/AAproject/42_webserv/docs/index.php";
		// std::string query = request.getQuery();
		// std::string path_query = path + "?" + query;
		std::string path_query = path;
		char	*php_path = (char *)"/usr/bin/php";
		char *argv[] = {php_path, const_cast<char *>(path_query.c_str()), NULL};
		char *envp[] = {NULL};
		execve(php_path, argv, envp);
		perror("execve");
		exit(0);
	}
	else
	{
		close(child_sock);
		return (pairent_sock);
	}
}
