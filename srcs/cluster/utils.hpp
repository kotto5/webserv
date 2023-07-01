#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include "server.hpp"

void	set_non_blocking(int socket);
size_t	find_start(const std::string &str, size_t start, const std::string &target);
void	partitionAndAddToMap(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);

#endif
