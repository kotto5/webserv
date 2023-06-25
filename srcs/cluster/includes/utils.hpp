#ifndef UTILS_HPP
#define UTILS_HPP

#include <unistd.h>
#include <string>
#include "server.hpp"

void	set_non_blocking(int socket);
size_t	find_start(const std::string &str, size_t start, const std::string &target);
int		array_insert(int (&array)[MAX_CLIENTS], int value);
int	array_delete(int (&array)[MAX_CLIENTS], int value);

#endif
