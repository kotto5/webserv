#include "server.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include <errno.h>
#include "Router.hpp"
#include "IHandler.hpp"

int	Server::setup()
{
	// config serverの数だけwhile で
	if (create_server_socket(80))
		return (1);
	if (create_server_socket(81))
		return (1);
	return (0);
}

int	Server::handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds, int &activity)
{
	std::list<int>::iterator	itr;
	std::list<int>::iterator	tmp;

	for (itr = server_sockets.begin(); read_fds && itr != server_sockets.end();)
	{
		tmp = itr++;
		if (FD_ISSET(*tmp, read_fds))
		{
			if (accept(*tmp) == -1)
				;
			--activity;
		}
	}
	for (itr = recv_sockets.begin(); read_fds && itr != recv_sockets.end();)
	{
		tmp = itr++;
		if (FD_ISSET(*tmp, read_fds))
		{
			if (cgi_client.count(*tmp))
				recv_cgi(tmp, Recvs[*tmp]);
			else
				if (recv(tmp, Recvs[*tmp]) == -1)
					;
			--activity;
		}
	}
	for (itr = send_sockets.begin(); write_fds && itr != send_sockets.end();)
	{
		tmp = itr++;
		if (FD_ISSET(*tmp, write_fds))
		{
			if (cgi_client.count(*tmp))
				send_cgi(tmp, Sends[*tmp]);
			else
				if (send(tmp, Sends[*tmp]) == -1)
					;
			--activity;
		}
	}
	if (expect_fds)
		std::cout << "EXPEXTION hHAHAHAHAH!!!!" << std::endl;
	return (0);
}

int	Server::run()
{
	while (1)
	{
		int max_fd = 0;
		fd_set read_fds;
		fd_set write_fds;
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		Server::set_fd_set(read_fds, server_sockets, max_fd);
		Server::set_fd_set(read_fds, recv_sockets, max_fd);
		Server::set_fd_set(write_fds, send_sockets, max_fd);

		int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);
		if (activity == -1)
		{
			perror("ERROR on select");
			exit(1);
		}
		handle_sockets(&read_fds, &write_fds, NULL, activity);
	}
}

Server::Server() {}

Server::~Server() {}

int	Server::accept(int listen_socket){
	struct sockaddr_in		client_address;
	socklen_t				client_length = sizeof(client_address);
	int 					new_socket = ::accept(listen_socket, (struct sockaddr*) &client_address, &client_length);

	if (new_socket < 0) {
		std::cout << errno << std::endl;
		perror("ERROR on accept");
		return (1);
	}
	set_non_blocking(new_socket);
	setFd(TYPE_RECV, new_socket);
	std::cout << RED << "New connection, socket fd is " << new_socket << ", port is " << ntohs(client_address.sin_port) << DEF << std::endl;
	std::cout << DEF;
	return (new_socket);
}

int isValidFd(int fd) {
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

int	Server::recv(std::list<int>::iterator itr, std::string &request_raw) {
	ssize_t recv_ret;
	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);

	std::cout << "recv!" << std::endl;
	recv_ret = ::recv(*itr, buffer, BUFFER_LEN, 0);
	if (recv_ret == -1)
	{
		std::cout << "recv err!" << std::endl;
		return (1);
	}
	request_raw += buffer;
	std::cout << "recv request[" << request_raw << "]" << std::endl;
	if (does_finish_recv_request(request_raw) == true)
	{
		std::cout << "send finished!"  << std::endl;
		Request	*request = parse_request(request_raw);
		std::string to_send;
		if (request->getUri().find(".php") != std::string::npos)
		{
			int	sockets[2];
			if (_socketpair(AF_INET, SOCK_STREAM, 0, sockets) == -1)
			{
				std::cout << "socketpair error" << std::endl;
				exit (-1);
			}

			if (runCgi(request, sockets[S_CHILD])){
				std::cout << "runCgi error" << std::endl;
				exit (-1);
			}
			close(sockets[S_CHILD]);
			std::cout << sockets[S_CHILD] << "  " <<  sockets[S_PARENT] << std::endl;
			set_non_blocking(S_PARENT);
			setFd(TYPE_SEND, sockets[S_PARENT]);
			setFd(TYPE_CGI, sockets[S_PARENT], *itr);
			Sends[sockets[S_PARENT]] = request->getBody();
		}
		else
		{
			Sends[*itr] = make_response(request);
			setFd(TYPE_SEND, *itr);
		}
		delete (request);
		Recvs.erase(*itr);
		recv_sockets.erase(itr);
	}
	return (0);
}

int	Server::recv_cgi(std::list<int>::iterator itr, std::string &request_raw) {
	ssize_t recv_ret;
	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);

	std::cout << "recv C   G    I!" << std::endl;
	recv_ret = ::recv(*itr, buffer, BUFFER_LEN, 0);
	if (recv_ret == -1)
	{
		std::cout << "recv err!" << std::endl;
		return (1);
	}
	request_raw += buffer;
	std::cout << "recv request[" << request_raw << "]" << std::endl;
	if (recv_ret == 0)
	{
		std::cout << "send finished!"  << std::endl;
		int	client_fd = cgi_client[*itr];
		Sends[client_fd] = request_raw;
		setFd(TYPE_SEND, client_fd);
		Recvs.erase(*itr);
		recv_sockets.erase(itr);
	}
	return (0);
}

int	Server::send(std::list<int>::iterator itr, std::string &response){
	ssize_t ret;
	const char *buffer;

	buffer = response.c_str();
	std::cout << "[" << buffer << "] is response" << std::endl;
	ret = ::send(*itr, (void *)buffer, response.length(), 0);
	if (ret == -1)
		return (1);
	if (static_cast<size_t>(ret) == response.length())
	{
		close(*itr);
		Sends.erase(*itr);
		send_sockets.erase(itr);
		// eraseFd(*itr, TYPE_SEND);
	}
	else
	{
		response = response.substr(ret);
	}
	return (RECV_FINISHED);
}

int	Server::send_cgi(std::list<int>::iterator itr, std::string &response){
	ssize_t ret;
	const char *buffer;

	buffer = response.c_str();
	std::cout << "[" << buffer << "] is response" << std::endl;
	ret = ::send(*itr, (void *)buffer, response.length(), 0);
	if (ret == -1)
		return (1);
	if (static_cast<size_t>(ret) == response.length())
	{
		Sends.erase(*itr);
		setFd(TYPE_RECV, *itr);
		eraseFd(*itr, TYPE_SEND);
	}
	else
	{
		response = response.substr(ret);
	}
	return (RECV_FINISHED);
}

int	Server::create_server_socket(int port)
{
	int	new_sock;
	new_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(new_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		return (1);
	}
	if (bind(new_sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
		perror("ERROR on binding");
		return (1);
	}
	if (listen(new_sock, 200) < 0){
		perror("ERROR on listening");
		return (1);
	}
	set_non_blocking(new_sock);
	server_sockets.push_back(new_sock);
	return (0);
}

Request	*Server::parse_request(const std::string &row_request){
	std::string method;
	std::string uri;
	std::string protocol;
	std::map<std::string, std::string> headers;
	std::string body;

	std::string::size_type startPos = 0;
	std::string::size_type endPos;
	std::string	line;
	while ((endPos = row_request.find("\r\n", startPos)) != std::string::npos)
	{
		if (endPos == startPos) // empty line
			break;
		line = row_request.substr(startPos, endPos - startPos);
		if (startPos == 0){
			std::string::size_type	tmp = line.find(" ");
			method = line.substr(0, tmp);
			std::string::size_type	tmp2 = line.find(" ", tmp + 1);
			uri = line.substr(tmp + 1, tmp2 - tmp - 1);
			tmp = line.find(" ");
			protocol = line.find(tmp);
		}
		else
			partitionAndAddToMap(headers, line, ": ");
		startPos = endPos + 2; // Skip CRLF
	}
	if (headers.find("Content-Length") != headers.end())
	{
		std::string::size_type	content_length = std::stoi(headers["Content-Length"]);
		body = row_request.substr(startPos, content_length);
	}
	if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] == "chunked")
	{
		std::string::size_type	end_of_body = row_request.find("\r\n0\r\n\r\n");
		body = row_request.substr(startPos, end_of_body - startPos);
		// TODO: body = decode_chunked(body);
	}
	return (new Request(method, uri, protocol, headers, body));
}

std::string	Server::make_response(Request *request){
	Router	router;
	IHandler	*handler = router.createHandler(*request);
	if (handler == NULL)
		return ("HTTP/1.1 404 Not Found\r\n\r\n");
	Response response = handler->handleRequest(*request);
	delete handler;
	return (response.toString());
}

bool	Server::does_finish_recv_request(const std::string &request){
	size_t	end_of_header;
	if (request.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		if (request.find("\r\n0\r\n\r\n") != std::string::npos)
			return (true);
		else
			return (false);
	}
	end_of_header = request.find("\r\n\r\n");
	if (end_of_header == std::string::npos)
		return (false);
	if (request.find("content-length: ") != std::string::npos &&
			find_start(request, end_of_header, "\r\n\r\n") == std::string::npos)
		return (false);
	return (true);
}

int	Server::set_fd_set(fd_set &set, std::list<int> sockets, int &maxFd)
{
	std::list<int>::iterator	itr;

	for (itr = sockets.begin(); itr != sockets.end(); itr++)
	{
		FD_SET(*itr , &set);
		if (*itr > maxFd)
			maxFd = *itr;
	}
	return (0);
}

int	Server::setFd(int type, int fd, int client_fd)
{
	if (type == TYPE_RECV)
		recv_sockets.push_back(fd);
	else if (type == TYPE_SEND)
		send_sockets.push_back(fd);
	else if (type == TYPE_SERVER)
		server_sockets.push_back(fd);
	else if (type == TYPE_CGI)
		cgi_client[fd] = client_fd;
	else
		return (1);
	return (0);
}

int	Server::eraseFd(int fd, int type)
{
	if (type == TYPE_RECV)
		recv_sockets.remove(fd);
	else if (type == TYPE_SEND)
		send_sockets.remove(fd);
	else if (type == TYPE_SERVER)
		server_sockets.remove(fd);
	else if (type == TYPE_CGI)
		cgi_client.erase(fd);
	else
		return (1);
	return (0);
}

int	Server::setcgiFd(int fd, int client_fd)
{
	cgi_client[fd] = client_fd;
	return (0);
}
