#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include "Server.hpp"
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
#include <cstdlib>

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

// int _socketpair(int domain, int type, int protocol, int sv[2]) {
//     int listener;
//     struct sockaddr_in name, client_name;
//     socklen_t client_len = sizeof(client_name);

//     // 1. Create a socket.
//     listener = socket(domain, SOCK_STREAM, protocol);
//     if (listener == -1) {
//         return -1;
//     }

//     // 2. Bind the socket to an address.
//     memset(&name, 0, sizeof(name));
//     name.sin_family = domain;
//     name.sin_addr.s_addr = INADDR_ANY;
//     name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
//     if (bind(listener, (struct sockaddr *)&name, sizeof(name)) == -1) {
//         perror("bind");
//         close(listener);
//         return -1;
//     }

//     // 3. Listen on the socket.
//     if (listen(listener, 1) == -1) {
//         close(listener);
//         return -1;
//     }

//     // 4. Get the port number.
//     socklen_t name_len = sizeof(name);
//     if (getsockname(listener, (struct sockaddr *)&name, &name_len) == -1) {
//         close(listener);
//         return -1;
//     }

//     // 5. Create and connect the second socket.
//     sv[1] = socket(domain, type, protocol);
//     if (sv[1] == -1) {
//         close(listener);
//         return -1;
//     }
//     if (connect(sv[1], (struct sockaddr *)&name, name_len) == -1) {
//         close(listener);
//         close(sv[1]);
//         return -1;
//     }

//     // 6. Accept the connection on the listener socket.
//     sv[0] = accept(listener, (struct sockaddr *)&client_name, &client_len);
//     if (sv[0] == -1) {
//         close(listener);
//         close(sv[1]);
//         return -1;
//     }

//     // Close the listener socket as we no longer need it.
//     close(listener);
//     return 0;
// }

int isValidFd(int fd) {
    return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}

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
