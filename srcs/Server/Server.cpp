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
		if (create_server_socket(strtoi(*itr)))
			return (1);
	}
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 5;
	_limitClientMsgSize = Config::instance()->getHTTPBlock().getClientMaxBodySize();
	return (0);
}

int	Server::handle_sockets(fd_set *read_fds, fd_set *write_fds, int activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp;
	bool							does_connected_cgi;
	Socket							*sock;

	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		tmp = itr++;
		sock = *tmp;
		if (FD_ISSET(((sock)->getFd()), read_fds))
		{
			accept(sock);
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
			if (recv(sock, Recvs[sock]) == 1 || Recvs[sock]->isEnd())
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
			send(sock, Sends[sock]);
			if (Sends[sock]->doesSendEnd())
			{
				finish_send(sock, Sends[sock]);
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

		int	activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
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
	Recvs[newSock] = new Request(newSock);
	std::cout << RED << "New connection, socket fd is " << newSock->getFd() << ", port is " << ntohs(newSock->getRemoteAddr().sin_port) << "time " << newSock->getLastAccess() << DEF << std::endl;
	return (0);
}

int	Server::new_connect_cgi(Request *request, Socket *clientSock)
{
	int	socks[2][2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks[0]) == -1)
	{
		perror("socketpair");
		throw ServerException("socketpair");
	}
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks[1]) == -1)
	{
		perror("socketpair");
		throw ServerException("socketpair");
	}

	if (runCgi(request, socks))
		throw ServerException("runCgi");
	close(socks[0][S_CHILD]);
	close(socks[1][S_CHILD]);
	set_non_blocking(socks[0][S_PARENT]);
	set_non_blocking(socks[1][S_PARENT]);

	if (request->getBody().size() != 0)
	{
		Socket *sockSend = new Socket(socks[0][S_PARENT]);
		setFd(TYPE_SEND, sockSend);
		Sends[sockSend] = new Request(request->getBody());
	}
	else
		close(socks[0][S_PARENT]);
	Socket *sockRecv = new Socket(socks[1][S_PARENT]);
	setFd(TYPE_RECV, sockRecv);
	setFd(TYPE_CGI, sockRecv, clientSock);
	Recvs[sockRecv] = new Response();
	return (0);
}

int	Server::recv(Socket *sock, HttpMessage *message) {
	ssize_t recv_ret;

	static char buffer[BUFFER_LEN];
	memset(buffer, 0, BUFFER_LEN);
	recv_ret = ::recv(sock->getFd(), buffer, BUFFER_LEN, 0);
	if (recv_ret >= 0)
	{
		std::cout << "recv_ret >= 0 recv is " << recv_ret << std::endl;
		sock->updateLastAccess();
	}
	else
		std::cout << "recv_ret < 0" << std::endl;	
	if (recv_ret == -1)
		message->parsing((""), _limitClientMsgSize);
	else
		message->parsing(std::string(buffer, (std::size_t)recv_ret), _limitClientMsgSize);
	return ((std::size_t)recv_ret == 0);
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
int	Server::finish_recv(Socket *sock, HttpMessage *message, bool is_cgi_connection)
{
	std::cout << "finish_recv [" << message->getRow() << "]" << std::endl;

	if (is_cgi_connection)
	{
		// fork pid も cgi socket とかに入れたろうかな
		int	wstatus;
		waitpid(-1, &wstatus, 0);
		Socket	*clSocket = cgi_client[sock];
		cgi_client.erase(sock);
		// if (WEXITSTATUS(wstatus) == 1 || !message->isEnd())
		if (WEXITSTATUS(wstatus) == 1)
			Sends[clSocket] = new Response("500");
		else
			Sends[clSocket] = new Response(*(Response *)message);
		setFd(TYPE_SEND, clSocket);
		delete (sock);
	}
	else
	{
		Request		*request = (Request *)message;
		request->setInfo();
		request->printAll();
		if (request_wants_cgi(request))
			new_connect_cgi(request, sock);
		else
		{
			Sends[sock] = makeResponse(request);
			setFd(TYPE_SEND, sock);
		}
	}
	Recvs.erase(sock);
	delete (message);
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

int		Server::finish_send(Socket *sock, HttpMessage *message)
{
	Sends.erase(sock);
	delete (message);
	delete (sock);
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

Response	*Server::makeResponse(Request *request)
{
	Router	router;

	if (request->isTooBigError())
		return (new Response("401"));
	else if (request->isBadRequest())
		return (new Response("400"));
	else if (request->getUri().find("..") != std::string::npos)
		return (new Response("403"));

	Response *response = router.routeHandler(*request);
	Logger::instance()->writeAccessLog(*request, *response);
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
