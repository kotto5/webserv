#include "Config.hpp"
#include "server.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include <errno.h>
#include "Router.hpp"
#include "IHandler.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include <vector>
#include <sys/wait.h>
#include <algorithm>
#include <ctime>

int	Server::setup()
{
	const std::vector<std::string> ports = Config::getInstance()->getPorts();
	std::vector<std::string>::const_iterator itr = ports.begin();

	for (itr = ports.begin(); itr != ports.end(); itr++)
	{
		if (create_server_socket(Server::strtoi(*itr)))
			return (1);
	}
	timeout.tv_sec = 5;
	return (0);
}

int	Server::handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds, int &activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	ssize_t							ret;
	bool							does_connected_cgi;
	Socket							*socket;

	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (FD_ISSET(((socket)->getFd()), read_fds))
		{
			if (accept(socket) == -1)
				;
			--activity;
		}
	}
	for (itr = recv_sockets.begin(); activity && itr != recv_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (FD_ISSET(socket->getFd(), read_fds))
		{
			ret = recv(socket, Recvs[socket->getFd()]);
			does_connected_cgi = (cgi_client.count(socket) == 1);
			if (does_finish_recv(Recvs[socket->getFd()], does_connected_cgi, ret))
				finish_recv(tmp, Recvs[socket->getFd()], does_connected_cgi);
			--activity;
		}
	}
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (FD_ISSET(socket->getFd(), write_fds))
		{
			ret = send(*tmp, Sends[socket->getFd()]);
			does_connected_cgi = (cgi_client.count(socket) == 1);
			if (does_finish_send(Sends[socket->getFd()], ret))
				finish_send(tmp, does_connected_cgi);
			else if (ret != -1)
				Sends[socket->getFd()] = Sends[socket->getFd()].substr(ret);
			--activity;
		}
	}
	if (expect_fds)
		std::cout << "EXPEXTION hHAHAHAHAH!!!!" << std::endl;
	return (0);
}

bool	Server::check_timeout()
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	Socket							*socket;
	bool							timeoutOccurred;

	timeoutOccurred = false;
	for (itr = recv_sockets.begin(); itr != recv_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (socket->isTimeout())
		{
			std::cout << "timeout" << std::endl;
			eraseFd(socket, TYPE_RECV);
			delete (socket);
			timeoutOccurred = true;
		}
	}
	for (itr = send_sockets.begin(); itr != send_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (socket->isTimeout())
		{
			std::cout << "timeout" << std::endl;
			eraseFd(socket, TYPE_SEND);
			delete (socket);
			timeoutOccurred = true;
		}
	}
	return (timeoutOccurred);
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

		int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
		if (activity == -1)
		{
			Error::print_error("select", Error::E_SYSCALL);
			exit(1);
		}
		if (check_timeout())
			continue ;
		handle_sockets(&read_fds, &write_fds, NULL, activity);
	}
}

Server::Server() {}

Server::~Server() {}

int	Server::accept(Socket *serverSocket)
{
	SvSocket *sv_socket = dynamic_cast<SvSocket *>(serverSocket);
	ClSocket *new_socket = sv_socket->dequeueSocket();
	if (new_socket == NULL)
		return (0);
	setFd(TYPE_RECV, new_socket);
	std::cout << RED << "New connection, socket fd is " << new_socket->getFd() << ", port is " << ntohs(new_socket->getRemoteaddr().sin_port) << "time " << new_socket->getLastAccess() << DEF << std::endl;
	return (0);
}

int	Server::new_connect_cgi(Request *request, Socket *clientSocket)
{
	int	sockets[2];
	if (_socketpair(AF_INET, SOCK_STREAM, 0, sockets) == -1)
	{
		Error::print_error("socketpair", Error::E_SYSCALL);
		exit (-1);
	}
	if (runCgi(request, sockets[S_CHILD])){

		Error::print_error("runcgi ERROR", Error::E_SYSCALL);
		exit (-1);
	}
	close(sockets[S_CHILD]);
	set_non_blocking(S_PARENT);

	Socket *socket = new Socket(sockets[S_PARENT]);
	setFd(TYPE_SEND, socket);
	setFd(TYPE_CGI, socket, clientSocket);
	Sends[sockets[S_PARENT]] = request->getBody();
	return (0);
}

ssize_t	Server::recv(Socket *sock, std::string &recieving) {
	ssize_t recv_ret;
	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);
	recv_ret = ::recv(sock->getFd(), buffer, BUFFER_LEN, 0);
	recieving += buffer;
	return (recv_ret);
}

int	Server::finish_recv(std::list<Socket *>::iterator itr, std::string &recieving, bool is_cgi_connection)
{
	std::cout << "finish_recv [" << recieving << "]" << std::endl;
	ClSocket	*sock = dynamic_cast<ClSocket *>(*itr);
	int	wstatus;

	recv_sockets.erase(itr);
	if (is_cgi_connection)
	{
		// fork pid も cgi socket とかに入れたろうかな
		waitpid(-1, &wstatus, 0);
		int	client_fd = cgi_client[sock]->getFd();
		Sends[client_fd] = recieving;
		setFd(TYPE_SEND, cgi_client[sock]);
	}
	else
	{
		Request	*request = parse_request(recieving);
		request->setaddr(sock);
		request->print_all();
		if (request_wants_cgi(request))
			new_connect_cgi(request, sock);
		else
		{
			Sends[sock->getFd()] = make_response(request);
			setFd(TYPE_SEND, sock);
		}
		delete (request);
	}
	Recvs.erase(sock->getFd());
	return (0);
}

bool	Server::request_wants_cgi(Request *request)
{
	if (request->getUri().find(".php") != std::string::npos)
		return (true);
	return (false);
}

ssize_t		Server::send(Socket *sock, std::string &response)
{
	ssize_t ret;
	const char *buffer;

	buffer = response.c_str();
	std::cout << "[" << buffer << "] is response" << std::endl;
	ret = ::send(sock->getFd(), (void *)buffer, response.length(), 0);
	return (ret);
}

bool	Server::does_finish_send(const std::string &request, ssize_t recv_ret)
{
	return (recv_ret != -1 && request.length() == static_cast<size_t>(recv_ret));
}

int	Server::finish_send(std::list<Socket *>::iterator itr, bool is_cgi_connection)
{
	Sends.erase((*itr)->getFd());
	if (is_cgi_connection)
	{
		setFd(TYPE_RECV, *itr);
	}
	else
	{
		delete (*itr);
	}
	send_sockets.erase(itr);
	return (0);
}

int	Server::create_server_socket(int port)
{
	Socket *socket = new SvSocket(port);
	if (socket == NULL)
		return (1);
	server_sockets.push_back(socket);
	return (0);
}

bool	is_valid_line(const std::string &line, const bool is_requestline)
{
	if (is_requestline)
	{
		long sp_count = std::count(line.begin(), line.end(), ' ');
		if (sp_count != 2)
			return (false);
		std::string::size_type	sp1 = line.find(" ");
		std::string::size_type	sp2 = line.find(" ", sp1 + 1);
		if (sp1 == sp2 + 1 || sp2 == static_cast<std::string::size_type>(line.end() - line.begin() - 1))
			return (false);
	}
	else
	{
		std::string::size_type	colon = line.find(": ");
		if (colon == std::string::npos || colon == 0 || colon == line.length() - 2)
			return (false);
	}
	return (true);
}

Request	*Server::parse_request(const std::string &row_request)
{
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
		if (is_valid_line(line, startPos == 0) == false)
			return (NULL);
		if (startPos == 0){
			std::string::size_type	method_end = line.find(" ");
			std::string::size_type	uri_end = line.find(" ", method_end + 1);
			method = line.substr(0, method_end);
			uri = line.substr(method_end + 1, uri_end - method_end - 1);
			protocol = line.substr(uri_end + 1);
		}
		else
			partitionAndAddToMap(headers, line, ": ");
		startPos = endPos + 2; // Skip CRLF
	}
	std::string content_length_name = "content-length";
	if (headers.find(content_length_name) != headers.end())
	{
		std::string::size_type	content_length = std::stoi(headers[content_length_name]);
		std::cout << "content_length: " << content_length << std::endl;
		body = row_request.substr(startPos + 2, content_length);
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
	Logger::getInstance()->writeAccessLog(*request, response);
	delete handler;
	return (response.toString());
}

bool	Server::does_finish_recv(const std::string &request, bool is_cgi_connection, ssize_t recv_ret)
{
	if (is_cgi_connection)
		return (recv_ret == 0);
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

int	Server::set_fd_set(fd_set &set, std::list<Socket *> sockets, int &maxFd)
{
	int	fd;
	std::list<Socket *>::iterator	itr;

	for (itr = sockets.begin(); itr != sockets.end(); itr++)
	{
		fd = (*itr)->getFd();
		FD_SET(fd , &set);
		if (fd > maxFd)
			maxFd = fd;
	}
	return (0);
}

int	Server::setFd(int type, Socket *sock, Socket *client_sock)
{
	ClSocket	*cl_socket = dynamic_cast<ClSocket *>(sock);

	if (type == TYPE_RECV && cl_socket)
		recv_sockets.push_back(cl_socket);
	else if (type == TYPE_SEND && cl_socket)
		send_sockets.push_back(cl_socket);
	else if (type == TYPE_SERVER)
		server_sockets.push_back(sock);
	else if (type == TYPE_CGI)
		cgi_client[sock] = client_sock;
	else
		return (1);
	return (0);
}

int	Server::eraseFd(Socket *socket, int type)
{
	ClSocket	*cl_socket = dynamic_cast<ClSocket *>(socket);

	if (type == TYPE_RECV && cl_socket)
		recv_sockets.remove(cl_socket);
	else if (type == TYPE_SEND && cl_socket)
		send_sockets.remove(cl_socket);
	else if (type == TYPE_SERVER)
		server_sockets.remove(socket);
	else if (type == TYPE_CGI)
		cgi_client.erase(socket);
	else
		return (1);
	return (0);
}

int Server::strtoi(std::string str)
{
	int ret = 0;
	for (std::string::iterator itr = str.begin(); itr != str.end(); itr++)
	{
		ret *= 10;
		ret += *itr - '0';
	}
	return (ret);
}
