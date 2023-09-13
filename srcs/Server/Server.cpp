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
#include "CgiSocketFactory.hpp"

Server::Server() {
	_socketCount = 0;
}

Server::~Server() {
	// std::map<Socket *, HttpMessage *>::iterator	itr1;
	// for (itr1 = Recvs.begin(); itr1 != Recvs.end(); itr1++)
	// 	delete (itr1->second);
	// for (itr1 = Sends.begin(); itr1 != Sends.end(); itr1++)
	// 	delete (itr1->second);

	std::list<Socket *>::iterator	itr2;
	for (itr2 = server_sockets.begin(); itr2 != server_sockets.end();)
	{
		std::list<Socket *>::iterator sockNode = itr2++;
		socketDeleter(*sockNode);
		server_sockets.erase(sockNode);
	}
	for (itr2 = recv_sockets.begin(); itr2 != recv_sockets.end();)
	{
		std::list<Socket *>::iterator sockNode = itr2++;
		deleteMapAndSockList(*sockNode, E_RECV);
		socketDeleter(*sockNode);
	}
	for (itr2 = send_sockets.begin(); itr2 != send_sockets.end();)
	{
		std::list<Socket *>::iterator sockNode = itr2++;
		deleteMapAndSockList(*sockNode, E_SEND);
		socketDeleter(*sockNode);
	}
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
	timeout.tv_sec = 1;
	_limitClientMsgSize = Config::instance()->getHTTPBlock().getClientMaxBodySize();
	return (0);
}

int	Server::run()
{
	extern int sigStatus;
	while (1)
	{
		if (sigStatus != 0)
			return (0);
		int max_fd = 0;
		fd_set read_fds;
		fd_set write_fds;
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		Server::set_fd_set(read_fds, server_sockets, max_fd);
		Server::set_fd_set(read_fds, recv_sockets, max_fd);
		Server::set_fd_set(write_fds, send_sockets, max_fd);

		int	activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
		if (activity == -1 && errno != EINTR)
			throw ServerException("select");
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
	_socketCount++;
	return (0);
}

bool	ClientConnectionErr(ssize_t ret, Socket *sock)
{
	if (ret > 0)
		return (false);
	return (ret == -1 && dynamic_cast<ClSocket *>(sock));
}

bool	ClientClosedConnection(ssize_t ret, Socket *sock)
{
	return (ret == 0 && dynamic_cast<ClSocket *>(sock) && 
		shutdown(sock->getFd(), SHUT_RD) == -1 && errno == ENOTCONN);
}

bool	isClient(Socket *sock)
{
	return (dynamic_cast<ClSocket *>(sock) != NULL);
}

int	Server::deleteSocketData(E_TYPE type, std::list<Socket *>::iterator sockNode)
{
	Socket *sock = *sockNode;
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
	{
		if (cgiSock->killCgi())
			perror("kill: ");
		ClSocket *clSocket = cgiSock->moveClSocket();
		if (addMapAndSockList(clSocket, IHandler::handleError("500", clSocket->getLocalPort()), E_SEND))
			socketDeleter(clSocket);
	}
	deleteSocket(type, sockNode);
	return (0);
}

int	Server::handleSockets(fd_set *read_fds, fd_set *write_fds, int activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	sockNode;
	Socket							*sock;

	// サーバーソケット受信

	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		sockNode = itr++;
		sock = *sockNode;
		if (FD_ISSET(((sock)->getFd()), read_fds))
		{
			accept(sock);
			--activity;
		}
	}
	// クライアントソケット受信
	for (itr = recv_sockets.begin(); itr != recv_sockets.end();)
	{
		sockNode = itr++;
		sock = *sockNode;
		if (FD_ISSET(sock->getFd(), read_fds) == false)
		{
			if (sock->isTimeout(std::time(NULL)))
				deleteSocketData(E_RECV, sockNode);
			continue ;
		}
		--activity;
		ssize_t	ret = recv(sock, Recvs[sock]);
		if (ClientConnectionErr(ret, sock))
			deleteSocketData(E_RECV, sockNode);
		else if (ret <= 0 ||
			(isClient(sock) && (Recvs[sock]->isCompleted() || Recvs[sock]->isInvalid())))
		{
			setNewSendMessage(sock, Recvs[sock]);
			deleteMapAndSockList(sock, E_RECV);
		}
	}
	// クライアントソケット送信
	for (itr = send_sockets.begin(); itr != send_sockets.end();)
	{
		sockNode = itr++;
		sock = *sockNode;
		if (FD_ISSET(sock->getFd(), write_fds) == false)
		{
			if (sock->isTimeout(std::time(NULL)))
				deleteSocketData(E_SEND, sockNode);
			continue ;
		}
		--activity;
		bool		isCgi = (dynamic_cast<CgiSocket *>(sock) != NULL);
		ssize_t		ret = send(sock, Sends[sock]);
		if (isCgi == false && ret == -1)
			deleteSocket(E_SEND, sockNode);
		else if (Sends[sock]->doesSendEnd() || ret == -1)
		{
			finishSend(sock);
			deleteMapAndSockList(sock, E_SEND);
		}
	}
	return (0);
}

int	Server::accept(Socket *serverSock)
{
	if (_socketCount >= MAX_SOCKETS)
		return (0);
	SvSocket *svSock = dynamic_cast<SvSocket *>(serverSock);
	ClSocket *newClSock = svSock->dequeueSocket();
	if (newClSock == NULL)
		return (0);
	if (addMapAndSockList(newClSock, new Request(newClSock), E_RECV))
	{
		socketDeleter(newClSock);
		return (1);
	}
	_socketCount++;
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

Socket	*Server::getHandleSock(Socket *sock, HttpMessage *recvdMessage, HttpMessage *toSendMessage)
{
	ClSocket *clSock = NULL;
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock)) // from cgi
	{
		clSock = cgiSock->moveClSocket();
		socketDeleter(cgiSock);
	}
	else
		clSock = dynamic_cast<ClSocket *>(sock); // from client
	if (dynamic_cast<Request *>(toSendMessage))
	{
		CgiSocket *newCgiSock;
		if ((newCgiSock = CgiSocketFactory::create(*recvdMessage, clSock))) // to cgi
			_socketCount++;
		return (newCgiSock);
	}
	else
		return (clSock); // to client
}

// bool じゃなくて dynamic_cast で判定したほうがいいかも
int	Server::setNewSendMessage(Socket *sock, HttpMessage *message)
{
	#ifdef TEST
		std::cout << "setNewSendMessage [" << message->getRaw() << "]" << std::endl;
	#endif

	Router router;
	// ルーティング
	HttpMessage *newMessage = router.routeHandler(*message, sock);
	if (newMessage == NULL)
		return (1);
	Socket *handleSock = getHandleSock(sock, message, newMessage);
	if (Response *response = dynamic_cast<Response *>(newMessage))
	{
		if (addMapAndSockList(handleSock, response, E_SEND))
		{
			socketDeleter(handleSock);
			return (1);
		}
		ClSocket *clSock = dynamic_cast<ClSocket *>(handleSock);
		Logger::instance()->writeAccessLog(*response, *clSock);
		addKeepAliveHeader(response, clSock);
	}
	else if (dynamic_cast<Request *>(newMessage)) // callCgi
		addMapAndSockList(handleSock, newMessage, E_SEND);
	return (0);
}

int	Server::setErrorResponse(Socket *clSock)
{
	HttpMessage *response = IHandler::handleError("500", clSock->getLocalPort());
	if (response == NULL)
		return (-1);
	else if (addMapAndSockList(clSock, response, E_SEND))
	{
		delete (response);
		return (-1);
	}
	else
		return (0);
}

void	Server::ErrorfinishSendCgi(CgiSocket *cgiSock, Socket *clSock)
{
	if (setErrorResponse(clSock))
		socketDeleter(clSock);
	socketDeleter(cgiSock);
}

int	Server::finishSend(Socket *sock)
{
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
	{
		// if (shutdown(sock->getFd(), SHUT_WR) == -1)
		if (shutdown(sock->getFd(), SHUT_WR) == -1 &&
			errno != ENOTCONN)
		{
			perror("=================================\nshutdown: ");
			std::cout << "shutdown error" << std::endl;
			std::cout << "=====================================" << std::endl;
			ErrorfinishSendCgi(cgiSock, cgiSock->moveClSocket());
			return (1);
		}
		if (addMapAndSockList(sock, new(std::nothrow) CgiResponse(), E_RECV))
		{
			ErrorfinishSendCgi(cgiSock, cgiSock->moveClSocket());
			return (1);
		}
	}
	else if (ClSocket *clSock = dynamic_cast<ClSocket *>(sock))
	{
		if (clSock->getMaxRequest() == 0)
			socketDeleter(clSock);
		else
			addMapAndSockList(sock, new Request((ClSocket *)sock), E_RECV);
	}
	return (0);
}

/**
 * @brief 指定したソケットを監視対象に追加する
 *
 * @param type
 * @param sock
 * @param client_sock
 * @return int
 */
int	Server::setSocket(int type, Socket *sock)
{
	try
	{
		if (type == E_RECV)
			recv_sockets.push_back(sock);
		else if (type == E_SEND)
			send_sockets.push_back(sock);
		else if (type == E_SERVER)
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

// delete MapNode and delete Socket at all(from list)
int	Server::deleteSocket(E_TYPE type, std::list<Socket *>::iterator sockNode)
{
	Socket *sock = *sockNode;
	if (type == E_SERVER)
		server_sockets.erase(sockNode);
	else
		deleteMapAndSockList(sock, type);
	socketDeleter(sock);
	return (0);
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

void	Server::addKeepAliveHeader(Response *response, ClSocket *clientSock)
{
	if (clientSock->getMaxRequest() == 0)
		response->addHeader("connection", "close");
	else
	{
		response->addHeader("connection", "keep-alive");
		std::string	keepAliveValue("timeout=");
		keepAliveValue += to_string(Socket::timeLimit);
		keepAliveValue += ", max=";
		keepAliveValue += to_string(clientSock->getMaxRequest());
		response->addHeader("keep-alive", keepAliveValue);
	}
	response->makeRowString();
}

int	Server::addMapAndSockList(Socket *sock, HttpMessage *message, S_TYPE type)
{
	if (message == NULL)
		return (1);
	std::map<Socket *, HttpMessage *>	*map = NULL;
	if (type == E_RECV)
		map = &Recvs;
	else if (type == E_SEND)
		map = &Sends;
	if (addMessageToMap(*map, sock, message))
	{
		delete (message);
		return (1);
	}
	if (setSocket(type, sock))
	{
		map->erase(sock);
		delete (message);
		return (1);
	}
	return (0);
}

int	Server::deleteMapAndSockList(Socket *sock, S_TYPE type)
{
	deleteMap(sock, type);
	if (type == E_RECV)
		recv_sockets.remove(sock);
	else if (type == E_SEND)
		send_sockets.remove(sock);
	return (0);
}

int	Server::addMessageToMap(std::map<Socket *, HttpMessage *> &map, Socket *sock, HttpMessage *message)
{
	try {
		map[sock] = message;
	}
	catch (const std::exception &e)
	{
		return (1);
	}
	return (0);
}

int	Server::deleteMap(Socket *sock, S_TYPE type)
{
	std::map<Socket *, HttpMessage *>	*map = NULL;
	if (type == E_RECV)
		map = &Recvs;
	else if (type == E_SEND)
		map = &Sends;
	delete ((*map)[sock]);
	map->erase(sock);
	return (0);
}

void	Server::socketDeleter(Socket *sock)
{
	delete (sock);
	_socketCount--;
}
