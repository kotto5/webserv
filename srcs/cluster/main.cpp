#include "server.hpp"
#include "utils.hpp"
#include <string.h>

int main()
{
	std::cout << SOMAXCONN << std::endl;
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
		fd_set write_fds;
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_SET(server_socket, &read_fds);
		int max_fd = server_socket;

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			int sd = sockets_recv[i];
			if (sd > 0)
				FD_SET(sd, &read_fds);
			if (sd > max_fd)
				max_fd = sd;
		}
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			int sd = sockets_send[i];
			if (sd > 0)
				FD_SET(sd, &write_fds);
			if (sd > max_fd)
				max_fd = sd;
		}
		int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);
		if (activity == -1)
		{
			perror("ERROR on select");
			exit(1);
		}
		if (FD_ISSET(server_socket, &read_fds) && client_count < MAX_CLIENTS)
		{
			// while (client_count < MAX_CLIENTS)
			// {
			// 	fd = server.handle_new_connection();
			// 	if (fd < 0)
			// 		break ;
			// 	array_insert(sockets_recv, fd);
			// 	client_count++;
			// }

			fd = server.handle_new_connection();
			if (fd < 0)
				exit(1);
			array_insert(sockets_recv, fd);
			activity--;
			client_count++;
		}
		// std::cout << "before recv" << activity << std::endl;
		server.recieve(activity, read_fds, sockets_recv, sockets_send);
		// std::cout << "after recv" << activity << std::endl;
		// }
		// if (activity)
		server.sender(activity, write_fds, sockets_send, client_count);
		std::cout << "clients: " << client_count << std::endl;
	}
}
