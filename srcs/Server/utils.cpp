#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include "server.hpp"
#include <map>
#include "Error.hpp"

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
