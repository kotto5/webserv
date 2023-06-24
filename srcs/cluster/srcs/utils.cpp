#include "utils.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>

void set_non_blocking(int socket) {
    if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
        perror("ERROR on fcntl");
        exit(1);
    }
}
