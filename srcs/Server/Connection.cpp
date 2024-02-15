#include "Connection.hpp"
#include "Request.hpp"
#include "Server.hpp"

Connection::Connection(ClSocket *sock)
{
    client = sock;
    recvMessage = new Request(sock);
}

Connection::~Connection()
{
}

#define MSG_LIMIT 10000000000

ssize_t clientRead(ClSocket *client, HttpMessage *m)
{
	ssize_t recv_ret;

	static char buffer[BUFFER_LEN];
	client->updateLastAccess();
	recv_ret = ::recv(client->getFd(), buffer, BUFFER_LEN, 0);
	if (recv_ret == -1)
	{
		perror("recv::: ");
		return (-1);
	}
	try {
		m->parsing(std::string(buffer, (std::size_t)recv_ret), MSG_LIMIT);
		return (recv_ret);
	}
	catch (const std::exception &e) {
		return (-1);
	}
}

bool	ClientConnectionErr(ssize_t ret, Socket *sock)
{
	if (ret > 0)
		return (false);
	return (ret == -1 && dynamic_cast<ClSocket *>(sock));
}

bool	isClient(Socket *sock)
{
	return (dynamic_cast<ClSocket *>(sock) != NULL);
}

sSelectRequest  Connection::handleEvent(sSelectRequest req, bool isSet)
{
    if (getFd(req) != client->getFd())
        return -1;
    else if (!isSet)
    {
        if (client->isTimeout())
        {
            // タイムアウト処理
            return -1;
        }
        else
            return req;
    }
    else if (getType(req) == READ)
    {
        if (getFd(req) == client->getFd())
        {
            ssize_t ret = clientRead(client, recvMessage);
            if (ClientConnectionErr(ret, client))
            {
                delete recvMessage;
                delete client;
                return -1;
            }
            else if (ret <= 0 ||
                (isClient(client) && (recvMessage->isCompleted() || recvMessage->isInvalid())))
            {
                // createResponse
                // setNewSendMessage(sock, Recvs[sock]);
                // deleteMapAndSockList(sockNode, E_RECV);
            }
        }
    }
    else if (getType(req) == WRITE)
    {
    }
    return 1;
}

sSelectRequest Connection::createRequest(int fd, eSelectType type)
{
    return type * MAX_SOCKETS + fd;
}

int Connection::getFd(sSelectRequest r)
{
    return r % MAX_SOCKETS;
}

eSelectType Connection::getType(sSelectRequest r)
{
    return (eSelectType)(r / MAX_SOCKETS);
}