#include "Config.hpp"
#include "Server.hpp"
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

Server::Server() {}

Server::~Server() {}

int	Server::setup()
{
	// 設定されているポートをすべて取得
	const std::vector<std::string> ports = Config::instance()->getPorts();
	std::vector<std::string>::const_iterator itr = ports.begin();

	for (itr = ports.begin(); itr != ports.end(); itr++)
	{
		if (createServerSocket(strtoi(*itr)))
			return (1);
	}
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 5;
	_limitClientMsgSize = Config::instance()->getHTTPBlock().getClientMaxBodySize();
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

		int	activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
		if (activity == -1)
			throw ServerException("select");
		if (activity == 0 && checkTimeout())
			continue ;
		handleSockets(&read_fds, &write_fds, activity);
	}
}

/**
 * @brief サーバーソケット作成
 *
 * @param port
 * @return int
 */
int	Server::createServerSocket(int port)
{
	Socket *sock = new SvSocket(port);
	if (sock == NULL)
		return (1);
	server_sockets.push_back(sock);
	return (0);
}

bool	cgiConnectionClosed(int ret, int isCgi) { return (ret == 0 && isCgi); }
bool	clientConnectionClosed(int ret, int isCgi) { return (ret == 0 && !isCgi); }

void	Server::recvError(Socket *sock, bool is_cgi)
{
	if (is_cgi)
	{
		Socket	*clSocket = cgi_client[sock];
		try
		{
			Sends[clSocket] = new Response("500");
			setFd(TYPE_SEND, clSocket);
		}
		catch (const std::exception &e)
		{
			deleteSocket(TYPE_SEND, clSocket);
		}
		cgi_client.erase(sock);
	}
	recv_sockets.remove(sock);
	delete (Recvs[sock]);
	delete (sock);
}

int	Server::handleSockets(fd_set *read_fds, fd_set *write_fds, int activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	tmp_socket;
	Socket							*sock;

	// サーバーソケット受信
	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		tmp_socket = itr++;
		sock = *tmp_socket;
		if (FD_ISSET(((sock)->getFd()), read_fds))
		{
			accept(sock);
			--activity;
		}
	}
	// クライアントソケット受信
	for (itr = recv_sockets.begin(); activity && itr != recv_sockets.end();)
	{
		tmp_socket = itr++;
		sock = *tmp_socket;
		if (!FD_ISSET(sock->getFd(), read_fds))
			continue ;
		bool is_cgi = cgi_client.count(sock) != 0;
		try
		{
			ssize_t ret = recv(sock, Recvs[sock]);
			if (ret == -1)
				recvError(sock, is_cgi);
			if (ret == 0 || 
				Recvs[sock]->isCompleted() || Recvs[sock]->isInvalid())
			{
				finishRecv(sock, Recvs[sock], is_cgi);
				recv_sockets.erase(tmp_socket);
			}
		}
		catch (const std::exception &e)
		{
			std::cout << e.what() << std::endl;
			recvError(sock, is_cgi);
		}
		--activity;
	}
	// クライアントソケット送信
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp_socket = itr++;
		sock = *tmp_socket;
		if (!FD_ISSET(sock->getFd(), write_fds))
			continue ;
		bool is_cgi = cgi_client.count(sock);
		if (send(sock, Sends[sock]) == -1)
		{
			if (is_cgi)
				ErrorfinishSendCgi(sock, cgi_client[sock]);
			else
			{
				delete (Sends[sock]);
				delete (sock);
			}
			send_sockets.erase(tmp_socket);
		}
		if (Sends[sock]->doesSendEnd())
		{
			send_sockets.erase(tmp_socket);
			finishSend(sock, Sends[sock], is_cgi);
		}
		--activity;
	}
	return (0);
}

int	Server::accept(Socket *serverSock)
{
	SvSocket *svSock = dynamic_cast<SvSocket *>(serverSock);
	ClSocket *newClSock = svSock->dequeueSocket();
	if (newClSock == NULL)
		return (0);
	setFd(TYPE_RECV, newClSock);
	Recvs[newClSock] = new Request(newClSock);
	return (0);
}

int	Server::recv(Socket *sock, HttpMessage *message) {
	ssize_t recv_ret;

	static char buffer[BUFFER_LEN];
	sock->updateLastAccess();
	recv_ret = ::recv(sock->getFd(), buffer, BUFFER_LEN, 0);
	if (recv_ret == -1)
	{
		perror("recv::: ");
		return (-1);
	}
	try {
		message->parsing(std::string(buffer, (std::size_t)recv_ret), _limitClientMsgSize);
		return (recv_ret);
	}
	catch (const std::exception &e) {
		return (-1);
	}
}

ssize_t		Server::send(Socket *sock, HttpMessage *message)
{
	ssize_t ret;
	const uint8_t *buffer;

	sock->updateLastAccess();
	buffer = message->getSendBuffer();
	if (buffer == NULL)
		return (-1);
	ret = ::send(sock->getFd(), buffer, message->getContentLengthRemain(), 0);
	if (ret > 0)
		message->addSendPos(ret);
	return (ret);
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
void Server::finishRecv(Socket *sock, HttpMessage *message, bool isCgi)
{
	Recvs.erase(sock);
	std::cout << "finishRecv [" << message->getRaw() << "]" << std::endl;

	Router router(*this);
	// ルーティング
	HttpMessage *newMessage = router.routeHandler(*message, sock);
	if (newMessage)
	{
		if (isCgi)
		{
			Socket	*clSocket = cgi_client[sock];
			Sends[clSocket] = newMessage;
			setFd(TYPE_SEND, clSocket);
			cgi_client.erase(sock);
			delete (sock);
		}
		else
		{
			// レスポンスを送信用ソケットに追加　
			Sends[sock] = newMessage;
			setFd(TYPE_SEND, sock);
			// アクセスログを書き込む
			Response *response = dynamic_cast<Response *>(newMessage);
			if (response)
			{
				Logger::instance()->writeAccessLog(*(Request *)message, *response);
				addKeepAliveHeader(response, (ClSocket *)sock, message);
			}
		}
	}
	delete (message);
}

int	Server::setErrorResponse(Socket *clSock)
{
	Response *response;
	try {
		response = new Response("500");
	}
	catch (const std::exception &e)
	{
		return (-1);
	}
	if (addSend(clSock, response))
	{
		delete (response);
		return (-1);
	}
	return (0);
}

void	Server::ErrorfinishSendCgi(Socket *cgiSock, Socket *clSock)
{
	if (setErrorResponse(clSock))
		delete (clSock);
	cgi_client.erase(cgiSock);
	delete (cgiSock);
}


void	Server::finishSend(Socket *sock, HttpMessage *message, bool isCgi)
{
	Sends.erase(sock);
	if (isCgi)
	{
		Socket	*clSocket = cgi_client[sock];
		if (shutdown(sock->getFd(), SHUT_WR) == -1)
			return (ErrorfinishSendCgi(sock, clSocket));
		Response *response = new Response();
		if (addRecv(sock, response))
		{
			delete (response);
			return (ErrorfinishSendCgi(sock, clSocket));
		}
	}
	else if (message->getHeader("connection") == "close")
		delete (sock);
	else
	{
		// Recvs[sock] = new Request((ClSocket *)sock);
		// setFd(TYPE_RECV, sock);
		addRecv(sock, new Request((ClSocket *)sock));
	}
	delete (message);
}

/**
 * @brief 指定したソケットを監視対象に追加する
 *
 * @param type
 * @param sock
 * @param client_sock
 * @return int
 */
int	Server::setFd(int type, Socket *sock, Socket *client_sock)
{
	try
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
	catch(const std::exception& e)
	{
		return (1);
	}
}

int	Server::deleteSocket(int type, Socket *socket)
{
	if (type == TYPE_RECV)
	{
		recv_sockets.remove(socket);
		delete (Recvs[socket]);
		delete (socket);
	}
	else if (type == TYPE_SEND)
	{
		send_sockets.remove(socket);
		delete (Sends[socket]);
		delete (socket);
	}
	else if (type == TYPE_SERVER)
	{
		server_sockets.remove(socket);
		delete (socket);
	}
	else if (type == TYPE_CGI)
	{
		cgi_client.erase(socket);
		delete (socket);
	}
	return (0);
}

bool	Server::checkTimeout()
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
			Logger::instance()->writeErrorLog(ErrorCode::RECV_TIMEOUT);
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
			Logger::instance()->writeErrorLog(ErrorCode::SEND_TIMEOUT);
			send_sockets.erase(tmp);
			delete (Sends[sock]);
			delete (sock);
			timeoutOccurred = true;
		}
	}
	return (timeoutOccurred);
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

/**
 * @brief CGI用のソケットを作成する
 *
 * @detail delegateパターンを用いてCgiHandlerクラスに移譲する
 *
 */

void	Server::addKeepAliveHeader(Response *response, ClSocket *clientSock, HttpMessage *request)
{
	if (request->getHeader("connection") == "close" || clientSock->getMaxRequest() == 0)
		response->addHeader("connection", "close");
	else
	{
		response->addHeader("connection", "keep-alive");
		std::string	keepAliveValue("timeout=");
		keepAliveValue += std::to_string(Socket::timeLimit);
		keepAliveValue += ", max=";
		keepAliveValue += std::to_string(clientSock->getMaxRequest());
		response->addHeader("keep-alive", keepAliveValue);
		clientSock->decrementMaxRequest();
	}
	response->makeRowString();
}

int	Server::addSend(Socket *sock, HttpMessage *message)
{
	Sends[sock] = message;
	setFd(TYPE_SEND, sock);
	return (0);
}

int	Server::addRecv(Socket *sock, HttpMessage *message)
{
	Recvs[sock] = message;
	setFd(TYPE_RECV, sock);
	return (0);
}

int	Server::addCgi(Socket *sock, Socket *clientSocket)
{
	setFd(TYPE_CGI, sock, clientSocket);
	return (0);
}