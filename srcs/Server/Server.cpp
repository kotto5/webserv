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
		if (FD_ISSET(sock->getFd(), read_fds))
		{
			bool is_cgi = cgi_client.count(sock);
			if (recv(sock, Recvs[sock]) == 1 || Recvs[sock]->isEnd())
			{
				finishRecv(sock, Recvs[sock], is_cgi);
				recv_sockets.erase(tmp_socket);
			}
			--activity;
		}
	}
	// クライアントソケット送信
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp_socket = itr++;
		sock = *tmp_socket;
		if (FD_ISSET(sock->getFd(), write_fds))
		{
			send(sock, Sends[sock]);
			if (Sends[sock]->doesSendEnd())
			{
				finishSend(sock, Sends[sock]);
				send_sockets.erase(tmp_socket);
			}
			--activity;
		}
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
	std::cout << RED << "New connection, socket fd is " << newClSock->getFd() << ", port is " << ntohs(newClSock->getRemoteAddr().sin_port) << "time " << newClSock->getLastAccess() << DEF << std::endl;
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

// bool じゃなくて dynamic_cast で判定したほうがいいかも
void Server::finishRecv(Socket *sock, HttpMessage *message, bool is_cgi)
{
	std::cout << "finishRecv [" << message->getRaw() << "]" << std::endl;

	Router router(*this);
	// ルーティング
	HttpMessage *newMessage = router.routeHandler(*message, sock);
	if (newMessage)
	{
		if (is_cgi)
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
	Recvs.erase(sock);
	delete (message);
}

void	Server::finishSend(Socket *sock, HttpMessage *message)
{
	Sends.erase(sock);
	if (message->getHeader("connection") == "close")
		delete (sock);
	else
	{
		Recvs[sock] = new Request((ClSocket *)sock);
		setFd(TYPE_RECV, sock);
	}
	delete (message);
}

/**
 * @brief 指定したソケットを監視リストに追加する
 *
 * @param type
 * @param sock
 * @param client_sock
 * @return int
 */
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
void Server::createSocketForCgi(int fd, const std::string &body, Socket *clientSocket)
{
	// CGI用のソケットを作成する
	Socket *sock = new Socket(fd);

	if (!clientSocket)
	{
		// 送信
		setFd(TYPE_SEND, sock);
		Sends[sock] = new Request(body);
	}
	else
	{
		// 受信
		setFd(TYPE_RECV, sock);
		setFd(TYPE_CGI, sock, clientSocket);
		Recvs[sock] = new Response();
	}
}

void	Server::addKeepAliveHeader(Response *response, ClSocket *clientSock, Request *request)
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
