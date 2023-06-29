#include "server.hpp"
#include "utils.hpp"
#include <string.h>

int main()
{
	Server server;

	int server_socket = server.get_server_socket();
	int sockets_recv[MAX_CLIENTS];
	int sockets_send[MAX_CLIENTS];
	int client_count = 0;

	bzero(sockets_recv, sizeof(int) * MAX_CLIENTS);
	bzero(sockets_send, sizeof(int) * MAX_CLIENTS);
	int fd;
	while (1)
	{
		fd_set read_fds;
		FD_ZERO(&read_fds);
		// while (servers)
		FD_SET(server_socket, &read_fds);
		int max_fd = server_socket;

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			int sd = sockets_recv[i];
			if (sd > 0)
			{
				FD_SET(sd, &read_fds);
			}
			if (sd > max_fd)
			{
				max_fd = sd;
			}
		}

		std::cout << "before" << std::endl;
		int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (activity < 0)
		{
			perror("ERROR on select");
			exit(1);
		}
		std::cout << "after" << std::endl;

		if (FD_ISSET(server_socket, &read_fds) && client_count < MAX_CLIENTS)
		{
			fd = server.handle_new_connection();
			if (fd < 0)
				exit(1);
			array_insert(sockets_recv, fd);
			activity--;
			client_count++;
		}
		std::cout << "foo" << std::endl;

		if (activity)
			server.recieve(activity, read_fds, sockets_recv, sockets_send);
	}
}
