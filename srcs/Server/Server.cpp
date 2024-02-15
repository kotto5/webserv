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
		Socket *sock = *sockNode;
		deleteMapAndSockList(sockNode, E_RECV);
		socketDeleter(sock);
	}
	for (itr2 = send_sockets.begin(); itr2 != send_sockets.end();)
	{
		std::list<Socket *>::iterator sockNode = itr2++;
		Socket *sock = *sockNode;
		deleteMapAndSockList(sockNode, E_SEND);
		socketDeleter(sock);
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
int	Server::createServerSocket(int port) throw()
{
	Socket *sock = new SvSocket(port);
	if (sock == NULL)
		return (1);
	if (setSocket(E_SERVER, sock))
	{
		delete (sock);
		return (1);
	}
	_socketCount++;
	return (0);
}

bool	ClientClosedConnection(ssize_t ret, Socket *sock)
{
	return (ret == 0 && dynamic_cast<ClSocket *>(sock) && 
		shutdown(sock->getFd(), SHUT_RD) == -1 && errno == ENOTCONN);
}

int		Server::setCgiErrorResponse(CgiSocket *cgiSock, bool timeout)
{
	if (timeout && cgiSock->killCgi())
		perror("kill: ");
	if (ClSocket *clSock = cgiSock->moveClSocket())
	{
		clSock->updateLastAccess();
		if (addMapAndSockList(clSock, IHandler::handleError("500", clSock->getLocalPort()), E_SEND))
		{
			socketDeleter(clSock);
			return (1);
		}
	}
	return (0);
}

int	Server::handleConnectionErr(E_TYPE type, std::list<Socket *>::iterator sockNode, bool timeout)
{
	Socket *sock = *sockNode;
	CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock);
	if (cgiSock)
		setCgiErrorResponse(cgiSock, timeout);
	deleteMapAndSockList(sockNode, type);
	socketDeleter(sock);
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
	std::map<sSelectRequest, Connection *>::iterator	itr2;
	for (itr2 = _connections.begin(); activity && itr2 != _connections.end();)
	{
		Connection *connection = itr2->second;
		const int fd = Connection::getFd(itr2->first);
		const sSelectRequest req = connection->handleEvent(itr2->first, FD_ISSET(fd, read_fds));
		FD_CLR(fd, read_fds);
		_connections.erase(itr2++);
		if (req != -1)
			_connections[req] = connection;
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

	Connection *newConnection = new Connection(newClSock);
	sSelectRequest req = Connection::createRequest(newClSock->getFd(), READ);
	_connections[req] = newConnection;

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

/**
 * @brief 指定したソケットを監視対象に追加する
 *
 * @param type
 * @param sock
 * @param client_sock
 * @return int
 */
int	Server::setSocket(E_TYPE type, Socket *sock) throw ()
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

int	Server::set_fd_set(fd_set &set, std::list<Socket *> sockets, int &maxFd) throw ()
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

// 例外: 強い保証 + message を削除する
int	Server::addMapAndSockList(Socket *sock, HttpMessage *message, S_TYPE type) throw ()
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

// 失敗しない保障
int	Server::deleteMapAndSockList(std::list<Socket *>::iterator sockNode, S_TYPE type) throw()
{
	// delete Map
	std::map<Socket *, HttpMessage *>	*map = NULL;
	if (type == E_RECV)
		map = &Recvs;
	else if (type == E_SEND)
		map = &Sends;
	delete ((*map)[*sockNode]);
	map->erase(*sockNode);

	// delete SockList
	if (type == E_RECV)
		recv_sockets.erase(sockNode);
	else if (type == E_SEND)
		send_sockets.erase(sockNode);
	return (0);
}

// 例外: 強い保証
int	Server::addMessageToMap(std::map<Socket *, HttpMessage *> &map, Socket *sock, HttpMessage *message) throw()
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

// 失敗しない保障
void	Server::socketDeleter(Socket *sock) throw ()
{
	delete (sock);
	_socketCount--;
}
