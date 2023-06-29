#include "server.hpp"
#include "utils.hpp"
#include "Request.hpp"

Server::Server() {
	server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8000);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}
	if (bind(server_socket_, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
		perror("ERROR on binding");
		exit(1);
	}
	if (listen(server_socket_, 100) < 0){
		perror("ERROR on listening");
		exit(1);
	}
	set_non_blocking(server_socket_);
}

Server::~Server() {}

bool	does_finish(const std::string &request){
	size_t	end_of_header;
	end_of_header = request.find("\r\n\r\n");
	if (end_of_header == std::string::npos)
		return (false);
	if (request.find("content-length: ") != std::string::npos && 
			find_start(request, end_of_header, "\r\n\r\n") == std::string::npos)
		return (false);
	return (true);
}

T_STATUS Server::recv(int socket_fd, std::string &request) {
	ssize_t recv_ret;
	static char buffer[BUFFER_LEN];

	recv_ret = ::recv(socket_fd, buffer, BUFFER_LEN, 0);
	if (recv_ret == -1)
		return (RECV_ERROR);
	request += buffer;
	if (does_finish(request) == false)
		return (RECV_CONTINUE);
	return (RECV_FINISHED);
}

T_STATUS Server::send(int socket_fd, std::string &request) {
	ssize_t ret;
	const char *buffer;

	buffer = request.c_str();
	std::cout << "[" << buffer << "] is response" << std::endl;
	ret = ::send(socket_fd, (void *)buffer, request.length(), 0);
	if (ret == -1)
		return (RECV_ERROR);
	if (static_cast<size_t>(ret) != request.length())
	{
		request = request.substr(ret);
		return (RECV_CONTINUE);
	}
	return (RECV_FINISHED);
}

int	Server::handle_new_connection(){
	struct sockaddr_in		client_address;
	socklen_t				client_length = sizeof(client_address);
	int 					new_socket = accept(server_socket_, (struct sockaddr*) &client_address, &client_length);

	if (new_socket < 0) {
		perror("ERROR on accept");
		exit(1);
	}
	set_non_blocking(new_socket);
	// client_sockets.emplace(new_socket, );
	std::pair<std::string, std::string> tmp;
	client_sockets[new_socket] = tmp;

	std::cout << "New connection, socket fd is " << new_socket << ", port is " << ntohs(client_address.sin_port) << std::endl;
	return (new_socket);
}

std::string	make_response(Request *request){
	(void)request;
	std::string ret = "HTTP/1.1 200 OK\r\n"
        "\r\n\r\n";
	return (ret);
}
// 	std::string	str;

// 	str += _getMethod();
// 	str += _getUri();
// 	std::map<std::string, std::string>::iterator itr;
// 	for (itr = _headers.begin(); itr != _headers.end(); itr++) {
// 		str += itr->first += ": " += itr->second;
// 	}
// 	str += getUri();
// 	return (str);
// }

int	Server::get_server_socket(){
	return (this->server_socket_);
}

Request	*Server::make_request(const std::string &row_request){
	std::string method;
	std::string uri;
	std::map<std::string, std::string> headers;
	std::string body;

	std::string::size_type startPos = 0;
	std::string::size_type endPos;
	std::string	line;

	while ((endPos = row_request.find("\r\n", startPos)) != std::string::npos)
	{
		line = row_request.substr(startPos, endPos);
		if (startPos == 0){
			std::string::size_type	tmp = line.find(": ");
			method = line.substr(0, tmp);
			uri = line.substr(tmp + 2);
		}
		else
			partitionAndAddToMap(headers, line, ": ");
		startPos = endPos + 2; // Skip CRLF
	}
	return (new Request(method, uri, headers, body));
}

int	Server::recieve(int &activity, fd_set &read_fds,
	int (&socket_recv)[MAX_CLIENTS], int (&socket_send)[MAX_CLIENTS]){
	std::map<int, massages>::iterator itr;
	int			fd;
	T_STATUS	ret;

	for (itr = client_sockets.begin(); itr != client_sockets.end() && activity; itr++) {
		fd = itr->first;
		if (FD_ISSET(fd, &read_fds)) {
			ret = Server::recv(fd, itr->second.first);
			if (ret == RECV_ERROR)
				exit(1);
			else if (ret == RECV_FINISHED)
			{
				std::cout << "recv finish!\n" << std::endl;
				array_delete(socket_recv, fd);
				Request	*request = make_request(itr->second.first);
				itr->second.second = make_response(request);
				delete request;
				// itr->second.second = itr->second.first;
				array_insert(socket_send, fd);
			}
			activity--;
		}
	}
	return (0);
}

int	Server::sender(int &activity, fd_set &write_fds, int (&socket_send)[MAX_CLIENTS])
{
	std::map<int, massages>::iterator itr;
	int			fd;
	T_STATUS	ret;

	std::cout << "sending!" << std::endl;
	for (itr = client_sockets.begin(); itr != client_sockets.end() && activity; itr++) {
		fd = itr->first;
		if (FD_ISSET(fd, &write_fds)) {
			ret = Server::send(fd, itr->second.second);
			if (ret == RECV_ERROR)
				exit(1);
			else if (ret == RECV_FINISHED)
			{
				close(fd);
				array_delete(socket_send, fd);
			}
			activity--;
		}
	}
	return (0);
}
