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
#include "OnlyBody.hpp"

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

int	Server::handle_sockets(fd_set *read_fds, fd_set *write_fds, int activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	ssize_t							ret;
	bool							does_connected_cgi;
	Socket							*sock;

	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (FD_ISSET(((sock)->getFd()), read_fds))
		{
			if (accept(sock) == -1)
				;
			--activity;
		}
	}
	for (itr = recv_sockets.begin(); activity && itr != recv_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (FD_ISSET(sock->getFd(), read_fds))
		{
			does_connected_cgi = (cgi_client.count(sock) == 1);
			ret = recv(sock, Recvs[sock]);
			if (Recvs[sock]->isEnd())
			{
				finish_recv(sock, Recvs[sock], does_connected_cgi);
				recv_sockets.erase(tmp);
			}
			--activity;
		}
	}
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (FD_ISSET(sock->getFd(), write_fds))
		{
			does_connected_cgi = (cgi_client.count(sock) == 1);
			ret = send(sock, Sends[sock]);
			if (Sends[sock]->doesSendEnd())
			{
				finish_send(sock, Sends[sock], does_connected_cgi);
				send_sockets.erase(tmp);
			}
			--activity;
		}
	}
	return (0);
}

bool	Server::check_timeout()
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	Socket							*sock;
	bool							timeoutOccurred;
	std::time_t						current_time = std::time(NULL);

	timeoutOccurred = false;
	for (itr = recv_sockets.begin(); itr != recv_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (sock->isTimeout(current_time))
		{
			std::cout << "timeout" << std::endl;
			recv_sockets.erase(tmp);
			delete (Recvs[sock]);
			delete (sock);
			timeoutOccurred = true;
		}
	}
	for (itr = send_sockets.begin(); itr != send_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (sock->isTimeout(current_time))
		{
			std::cout << "timeout" << std::endl;
			send_sockets.erase(tmp);
			delete (Sends[sock]);
			delete (sock);
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
		handle_sockets(&read_fds, &write_fds, activity);
	}
}

Server::Server() {}
Server::~Server() {}

int	Server::accept(Socket *serverSock)
{
	SvSocket *svSock = dynamic_cast<SvSocket *>(serverSock);
	ClSocket *newSock = svSock->dequeueSocket();
	if (newSock == NULL)
		return (0);
	setFd(TYPE_RECV, newSock);
	Recvs[newSock] = new Request();
	std::cout << RED << "New connection, socket fd is " << newSock->getFd() << ", port is " << ntohs(newSock->getRemoteaddr().sin_port) << "time " << newSock->getLastAccess() << DEF << std::endl;
	return (0);
}

int	Server::new_connect_cgi(Request *request, Socket *clientSock)
{
	int	socks[2];
	if (_socketpair(AF_INET, SOCK_STREAM, 0, socks) == -1)
		throw ServerException("socketpair");
	if (runCgi(request, socks[S_CHILD]))
		throw ServerException("runCgi");
	close(socks[S_CHILD]);
	set_non_blocking(S_PARENT);

	Socket *sock = new Socket(socks[S_PARENT]);
	setFd(TYPE_SEND, sock);
	setFd(TYPE_CGI, sock, clientSock);
	Sends[sock] = new OnlyBody();
	Sends[sock]->parsing(request->getBody(), true);
	delete (request);
	return (0);
}

ssize_t	Server::recv(Socket *sock, HttpMessage *message) {
	ssize_t recv_ret;

	sock->updateLastAccess();
	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);
	recv_ret = ::recv(sock->getFd(), buffer, BUFFER_LEN, 0);
	message->parsing(buffer, recv_ret == 0);
	return (recv_ret);
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
int	Server::finish_recv(Socket *sock, HttpMessage *message, bool is_cgi_connection)
{
	std::cout << "finish_recv [" << message->getRow() << "]" << std::endl;
	int	wstatus;

	if (is_cgi_connection)
	{
		// fork pid も cgi socket とかに入れたろうかな
		waitpid(-1, &wstatus, 0);
		Socket	*clSocket = cgi_client[sock];
		Sends[clSocket] = message;
		setFd(TYPE_SEND, clSocket);
	}
	else
	{
		Request		*request = (Request *)message;
		ClSocket	*clsock = (ClSocket *)sock;
		request->setaddr(clsock);
		request->setinfo();
		request->print_all();
		if (request_wants_cgi(request))
			new_connect_cgi(request, clsock);
		else
		{
			Sends[clsock] = makeResponse(request);
			setFd(TYPE_SEND, clsock);
			delete (request);
		}
	}
	Recvs.erase((sock));
	return (0);
}

bool	Server::request_wants_cgi(Request *request)
{
	if (request->getActualUri().find(".php") != std::string::npos)
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

int		Server::finish_send(Socket *sock, HttpMessage *message, bool is_cgi_connection)
{
	if (is_cgi_connection)
	{
		Recvs[sock] = new Response();
		setFd(TYPE_RECV, sock);
	}
	else
	{
		delete (sock);
	}
	delete (message);
	Sends.erase(sock);
	return (0);
}

int	Server::create_server_socket(int port)
{
	Socket *sock = new SvSocket(port);
	if (sock == NULL)
		return (1);
	server_sockets.push_back(sock);
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
