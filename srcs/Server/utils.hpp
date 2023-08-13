#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include "server.hpp"

enum {
	ERROR = -1,
	SUCCESS = 0,
};

enum {
	S_PARENT = 0,
	S_CHILD = 1,
};

void	set_non_blocking(int socket);
size_t	find_start(const std::string &str, size_t start, const std::string &target);
int		isValidFd(int fd);
bool    isFile(const char *path);
bool    pathExist(const char *path);
bool    isDirectory(const char *path);
std::string percentEncode(std::string str);
std::string percentDecode(std::string str);
int			strtoi(std::string str);

#endif
