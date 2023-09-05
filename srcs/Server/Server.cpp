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
#include "CgiResponse.hpp"

Server::Server() {}

Server::~Server() {
	// std::map<Socket *, HttpMessage *>::iterator	itr1;
	// for (itr1 = Recvs.begin(); itr1 != Recvs.end(); itr1++)
	// 	delete (itr1->second);
	// for (itr1 = Sends.begin(); itr1 != Sends.end(); itr1++)
	// 	delete (itr1->second);

	std::list<Socket *>::iterator	itr2;
	for (itr2 = server_sockets.begin(); itr2 != server_sockets.end(); itr2++)
		delete (*itr2);
	for (itr2 = recv_sockets.begin(); itr2 != recv_sockets.end(); itr2++)
		delete (*itr2);
	for (itr2 = send_sockets.begin(); itr2 != send_sockets.end(); itr2++)
		delete (*itr2);
}

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
		if (activity == 0)
			checkTimeout();
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

void	Server::recvError(Socket *sock)
{
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
	{
		ClSocket *clSocket = cgiSock->moveClSocket();
		try
		{
			addSend(clSocket, new Response("500"));
		}
		catch (const std::exception &e)
		{
			deleteSocket(TYPE_SEND, clSocket);
		}
	}
	deleteRecv(sock);
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
	for (itr = recv_sockets.begin(); itr != recv_sockets.end();)
	{
		tmp_socket = itr++;
		ssize_t	ret = 1;
		sock = *tmp_socket;
		if (FD_ISSET(sock->getFd(), read_fds))
		{
			ret = recv(sock, Recvs[sock]);
			--activity;
		}
		if (ret == -1)
			recvError(sock);
		else if (ret == 0 ||
			Recvs[sock]->isCompleted() || Recvs[sock]->isInvalid())
		{
			try {
				finishRecv(sock, Recvs[sock]);
				recv_sockets.erase(tmp_socket);
			}
			catch (const std::exception &e)
			{
				std::cout << e.what() << std::endl;
				recvError(sock);
			}
		}
	}
	// クライアントソケット送信
	for (itr = send_sockets.begin(); activity && itr != send_sockets.end();)
	{
		tmp_socket = itr++;
		sock = *tmp_socket;
		if (!FD_ISSET(sock->getFd(), write_fds))
			continue ;
		bool		isCgi = (dynamic_cast<CgiSocket *>(sock) != NULL);
		ssize_t		ret = send(sock, Sends[sock]);
		if (isCgi == false && ret == -1)
		{
			deleteSend(sock);
			delete (sock);
			send_sockets.erase(tmp_socket);
		}
		else if (Sends[sock]->doesSendEnd() || ret == -1)
		{
			send_sockets.erase(tmp_socket);
			finishSend(sock);
			deleteSend(sock);
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
	addRecv(newClSock, new Request(newClSock));
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
	if (ret >= 0) // 0 も含んでるのはcgiのために超大事
		message->addSendPos(ret);
	return (ret);
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
void Server::finishRecv(Socket *sock, HttpMessage *message)
{
	#ifdef TEST
		std::cout << "finishRecv [" << message->getRaw() << "]" << std::endl;
	#endif

	Router router(*this);
	// ルーティング
	HttpMessage *newMessage = router.routeHandler(*message, sock);
	if (newMessage)
	{
		// from cgi
		if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
		{
			Socket	*clSocket = cgiSock->moveClSocket();
			if (dynamic_cast<Response *>(newMessage))
			{
				addSend(clSocket, newMessage);
			}
			else if (dynamic_cast<Request *>(newMessage))
			{
				addRecv(clSocket, newMessage);
			}
			delete (sock);
		}
		// from client
		else
		{
			// レスポンスを送信用ソケットに追加　
			addSend(sock, newMessage);
			// アクセスログを書き込む
			Response *response = dynamic_cast<Response *>(newMessage);
			if (response)
			{
				Logger::instance()->writeAccessLog(*(Request *)message, *response);
				addKeepAliveHeader(response, (ClSocket *)sock);
			}
		}
	}
	deleteRecv(sock);
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

void	Server::ErrorfinishSendCgi(CgiSocket *cgiSock, Socket *clSock)
{
	if (setErrorResponse(clSock))
		delete (clSock);
	delete (cgiSock);
}


void	Server::finishSend(Socket *sock)
{
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
	{
		if (shutdown(sock->getFd(), SHUT_WR) == -1)
		// if (shutdown(sock->getFd(), SHUT_WR) == -1 &&
		// 	errno != ENOTCONN)
		{
			perror("=================================\nshutdown: ");
			std::cout << "shutdown error" << std::endl;
			std::cout << "=====================================" << std::endl;
			return (ErrorfinishSendCgi(cgiSock, cgiSock->moveClSocket()));
		}
		CgiResponse *response = new CgiResponse();
		if (addRecv(sock, response))
		{
			delete (response);
			return (ErrorfinishSendCgi(cgiSock, cgiSock->moveClSocket()));
		}
	}
	else if (ClSocket *clSock = dynamic_cast<ClSocket *>(sock))
	{
		if (clSock->getMaxRequest() == 0)
			delete (clSock);
		else
			addRecv(sock, new Request((ClSocket *)sock));
	}
}

/**
 * @brief 指定したソケットを監視対象に追加する
 *
 * @param type
 * @param sock
 * @param client_sock
 * @return int
 */
int	Server::setFd(int type, Socket *sock)
{
	try
	{
		if (type == TYPE_RECV)
			recv_sockets.push_back(sock);
		else if (type == TYPE_SEND)
			send_sockets.push_back(sock);
		else if (type == TYPE_SERVER)
			server_sockets.push_back(sock);
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
		deleteRecv(socket);
		delete (socket);
	}
	else if (type == TYPE_SEND)
	{
		deleteSend(socket);
		delete (socket);
	}
	else if (type == TYPE_SERVER)
	{
		server_sockets.remove(socket);
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
			deleteRecv(sock);
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
			deleteSend(sock);
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

void	Server::addKeepAliveHeader(Response *response, ClSocket *clientSock)
{
	if (clientSock->getMaxRequest() == 0)
		response->addHeader("connection", "close");
	else
	{
		response->addHeader("connection", "keep-alive");
		std::string	keepAliveValue("timeout=");
		keepAliveValue += std::to_string(Socket::timeLimit);
		keepAliveValue += ", max=";
		keepAliveValue += std::to_string(clientSock->getMaxRequest());
		response->addHeader("keep-alive", keepAliveValue);
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

int	Server::deleteSend(Socket *sock)
{
	delete (Sends[sock]);
	Sends.erase(sock);
	return (0);
}

int	Server::deleteRecv(Socket *sock)
{
	delete (Recvs[sock]);
	Recvs.erase(sock);
	return (0);
}