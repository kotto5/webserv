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

int		array_insert(int (&array)[MAX_CLIENTS], int value){
	int	i = 0;
	while (i < MAX_CLIENTS){
		if (array[i] == 0)
		{
			array[i] = value;
			return 0;
		}
		i++;
	}
	return 1;
}

int	array_delete(int (&array)[MAX_CLIENTS], int value){
	int	i = 0;
	while (i < MAX_CLIENTS){
		if (array[i] == value)
		{
			array[i] = 0;
			return 0;
		}
		i++;
	}
	return (1);
}

void partitionAndAddToMap(std::map<std::string, massages>& m, const std::string& inputStr, const std::string& keyword) {
    size_t pos = inputStr.find(keyword);

    if (pos != std::string::npos) {
        std::string part1 = inputStr.substr(0, pos);
        std::string part2 = inputStr.substr(pos + keyword.length());

        m[keyword] = std::make_pair(part1, part2);
    }
}
