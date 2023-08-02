#include "Config.hpp"
#include "server.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include <errno.h>
#include "Router.hpp"
#include "IHandler.hpp"
#include "Logger.hpp"
#include "ErrorCode.hpp"
#include <vector>
#include <sys/wait.h>
#include <algorithm>
#include <ctime>
#include "ServerException.hpp"

int	Server::setup()
{
	// 設定されているポートをすべて取得
	const std::vector<std::string> ports = Config::instance()->getPorts();
	std::vector<std::string>::const_iterator itr = ports.begin();

	for (itr = ports.begin(); itr != ports.end(); itr++)
	{
		if (create_server_socket(Server::strtoi(*itr)))
			return (1);
	}
	memset(&timeout, 0, sizeof(timeout));
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
			ret = recv(socket, Recvs[socket]);
			does_connected_cgi = (cgi_client.count(socket) == 1);
			if (Recvs[socket]->isEnd() || (does_connected_cgi && ret == 0))
				finish_recv(tmp, Recvs[socket], does_connected_cgi);
			--activity;
		}
	}
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (FD_ISSET(socket->getFd(), write_fds))
		{
			ret = send(*tmp, Sends[socket]);
			does_connected_cgi = (cgi_client.count(socket) == 1);
			if (Sends[socket]->doesSendEnd())
				finish_send(tmp, Sends[socket], does_connected_cgi);
			--activity;
		}
	}
	if (expect_fds)
		throw ServerException("unexpected fds");
	return (0);
}

bool	Server::check_timeout()
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	Socket							*socket;
	bool							timeoutOccurred;
	std::time_t						current_time = std::time(NULL);

	timeoutOccurred = false;
	for (itr = recv_sockets.begin(); itr != recv_sockets.end();)
	{
		tmp = itr++;
		socket = *tmp;
		if (socket->isTimeout(current_time))
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
		if (socket->isTimeout(current_time))
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

		int	activity = select(max_fd + 1,&read_fds, &write_fds, NULL, &timeout);
		if (activity == -1)
			throw ServerException("select");
		if (activity == 0 && check_timeout())
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
	Recvs[new_socket] = new Request();
	std::cout << RED << "New connection, socket fd is " << new_socket->getFd() << ", port is " << ntohs(new_socket->getRemoteaddr().sin_port) << "time " << new_socket->getLastAccess() << DEF << std::endl;
	return (0);
}

int	Server::new_connect_cgi(Request *request, Socket *clientSocket)
{
	int	sockets[2];
	if (_socketpair(AF_INET, SOCK_STREAM, 0, sockets) == -1)
		throw ServerException("socketpair");
	if (runCgi(request, sockets[S_CHILD]))
		throw ServerException("runCgi");
	close(sockets[S_CHILD]);
	set_non_blocking(S_PARENT);

	Socket *socket = new Socket(sockets[S_PARENT]);
	setFd(TYPE_SEND, socket);
	setFd(TYPE_CGI, socket, clientSocket);
	Sends[socket] = request;
	return (0);
}

ssize_t	Server::recv(Socket *sock, HttpMessage *message) {
	ssize_t recv_ret;

	sock->updateLastAccess();
	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);
	recv_ret = ::recv(sock->getFd(), buffer, BUFFER_LEN, 0);
	message->parsing(buffer);
	return (recv_ret);
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
int	Server::finish_recv(std::list<Socket *>::iterator itr, HttpMessage *message, bool is_cgi_connection)
{
	std::cout << "finish_recv [" << message->getRow() << "]" << std::endl;
	int	wstatus;

	if (is_cgi_connection)
	{
		// fork pid も cgi socket とかに入れたろうかな
		Socket	*sock = *itr;
		waitpid(-1, &wstatus, 0);
		Sends[sock] = message;
		setFd(TYPE_SEND, cgi_client[sock]);
	}
	else
	{
		Request		*request = (Request *)message;
		ClSocket	*sock = (ClSocket *)*itr;
		request->setaddr(sock);
		request->setinfo();
		request->print_all();
		if (request_wants_cgi(request))
			new_connect_cgi(request, sock);
		else
		{
			Sends[sock] = makeResponse(request);
			setFd(TYPE_SEND, sock);
		}
	}
	delete (message);
	Recvs.erase((*itr));
	recv_sockets.erase(itr);
	return (0);
}

bool	Server::request_wants_cgi(Request *request)
{
	if (request->getUri().find(".php") != std::string::npos)
		return (true);
	return (false);
}

ssize_t		Server::send(Socket *sock, HttpMessage *message)
{
	ssize_t ret;
	const uint8_t *buffer;

	sock->updateLastAccess();
	buffer = message->getSendBuffer();
	ret = ::send(sock->getFd(), buffer, message->getContentLengthRemain(), 0);
	message->addSendPos(ret);
	return (ret);
}

int		Server::finish_send(std::list<Socket *>::iterator itr, HttpMessage *response, bool is_cgi_connection)
{
	if (is_cgi_connection)
	{
		setFd(TYPE_RECV, *itr);
		Recvs[*itr] = new Response();
	}
	else
	{
		delete (*itr);
		delete (response);
	}
	Sends.erase(*itr);
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

Response	*Server::makeResponse(Request *request){
	Router	router;
	IHandler	*handler = router.createHandler(*request);
	if (handler == NULL)
		return (new Response("404"));
	Response	*response = handler->handleRequest(*request);
	Logger::instance()->writeAccessLog(*request, *response);
	delete handler;
	return (response);
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
	if (type == TYPE_RECV)
		recv_sockets.push_back(sock);
	else if (type == TYPE_SEND)
		send_sockets.push_back(sock);
	else if (type == TYPE_SERVER)
		server_sockets.push_back(sock);
	else if (type == TYPE_CGI)
		cgi_client[sock] = client_sock;
	else
		return (1);
	return (0);
}

int	Server::eraseFd(Socket *sock, int type)
{
	if (type == TYPE_RECV)
		recv_sockets.remove(sock);
	else if (type == TYPE_SEND)
		send_sockets.remove(sock);
	else if (type == TYPE_SERVER)
		server_sockets.remove(sock);
	else if (type == TYPE_CGI)
		cgi_client.erase(sock);
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
