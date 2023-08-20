#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include "Server.hpp"

void	set_non_blocking(int socket);
size_t	find_start(const std::string &str, size_t start, const std::string &target);
int     _socketpair(int domain, int type, int protocol, int sv[2]);
int	runCgi(Request *request, int pipes[2][2]);
int	runCgi(Request *request, int sockRecv, int sockSend);
int		isValidFd(int fd);
bool    isFile(const char *path);
bool    pathExist(const char *path);
bool    isDirectory(const char *path);
std::string percentEncode(std::string str);
std::string percentDecode(std::string str);
int			strtoi(std::string str);

#endif
