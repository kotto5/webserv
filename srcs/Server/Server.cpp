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
		delete ((*sockNode));
		server_sockets.erase(sockNode);
	}
}

int	Server::setup()
{
	// 設定されているポートをすべて取得
	const std::vector<std::string> ports = Config::instance()->getPorts();
	std::vector<std::string>::const_iterator itr = ports.begin();

	for (itr = ports.begin(); itr != ports.end(); itr++)
	{
		Socket *sock = new SvSocket(strtoi(*itr));
		if (sock == NULL)
			return (1);
		server_sockets.push_back(sock);
		_socketCount++;
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
		std::map<sSelectRequest, Connection *>::iterator	itr;
		Server::set_fd_set(read_fds, server_sockets, max_fd);
		for (itr = _connections.begin(); itr != _connections.end(); itr++)
		{
			const int fd = Connection::getFd(itr->first);
			const eSelectType type = Connection::getType(itr->first);
			if (type == READ)
				FD_SET(fd, &read_fds);
			else
				FD_SET(fd, &write_fds);
			if (fd > max_fd)
				max_fd = fd;
		}
		int	activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
		if (activity == -1 && errno != EINTR)
			throw ServerException("select");
		handleSockets(&read_fds, &write_fds, activity);
	}
}

bool	ClientClosedConnection(ssize_t ret, Socket *sock)
{
	return (ret == 0 && dynamic_cast<ClSocket *>(sock) && 
		shutdown(sock->getFd(), SHUT_RD) == -1 && errno == ENOTCONN);
}

// int		Server::setCgiErrorResponse(CgiSocket *cgiSock, bool timeout)
// {
// 	if (timeout && cgiSock->killCgi())
// 		perror("kill: ");
// 	if (ClSocket *clSock = cgiSock->moveClSocket())
// 	{
// 		clSock->updateLastAccess();
// 		if (addMapAndSockList(clSock, IHandler::handleError("500", clSock->getLocalPort()), E_SEND))
// 		{
// 			socketDeleter(clSock);
// 			return (1);
// 		}
// 	}
// 	return (0);
// }

// int	Server::handleConnectionErr(E_TYPE type, std::list<Socket *>::iterator sockNode, bool timeout)
// {
// 	Socket *sock = *sockNode;
// 	CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock);
// 	if (cgiSock)
// 		setCgiErrorResponse(cgiSock, timeout);
// 	deleteMapAndSockList(sockNode, type);
// 	socketDeleter(sock);
// 	return (0);
// }

int	Server::handleSockets(fd_set *read_fds, fd_set *write_fds, int activity)
{
	std::list<Socket *>::iterator	itr;
	std::list<Socket *>::iterator	sockNode;
	Socket							*sock;
	std::map<sSelectRequest, Connection *> newConnections;

	// サーバーソケット受信

	for (itr = server_sockets.begin(); activity && itr != server_sockets.end();)
	{
		sockNode = itr++;
		sock = *sockNode;
		if (FD_ISSET(((sock)->getFd()), read_fds))
		{
			Connection *newConnection = accept(sock);
			sSelectRequest req = Connection::createRequest(newConnection->getFd(), READ);
			newConnections[req] = newConnection;
			_socketCount++;
			--activity;
		}
	}
	std::map<sSelectRequest, Connection *>::iterator	itr2;
	for (itr2 = _connections.begin(); activity && itr2 != _connections.end();)
	{
		Connection *connection = itr2->second;
		const sSelectRequest r = itr2->first;
		const int fd = Connection::getFd(r);
		const bool fdIsSet = Connection::getType(r) == READ ? FD_ISSET(fd, read_fds) : FD_ISSET(fd, write_fds);

		const sSelectRequest req = connection->handleEvent(r, fdIsSet);
		if (req != -1)
			newConnections[req] = connection;
		itr2++;
	}
	_connections = newConnections;
	return (0);
}

Connection	*Server::accept(Socket *serverSock)
{
	if (_socketCount >= MAX_SOCKETS)
		return (NULL);
	SvSocket *svSock = dynamic_cast<SvSocket *>(serverSock);
	ClSocket *newClSock = svSock->dequeueSocket();
	if (newClSock == NULL)
		return (NULL);
	return (new Connection(newClSock));
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
