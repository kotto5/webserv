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
void	partitionAndAddToMap(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
int     _socketpair(int domain, int type, int protocol, int sv[2]);
int		runCgi(Request *request, int socket);

#endif
