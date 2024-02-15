#include "Connection.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Router.hpp"
#include "CgiSocketFactory.hpp"

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

Socket	*getHandleSock(Socket *sock, HttpMessage *recvdMessage, HttpMessage *toSendMessage)
{
	ClSocket *clSock = NULL;
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock)) // from cgi
	{
		clSock = cgiSock->moveClSocket();
        delete (sock);
	}
	else
		clSock = dynamic_cast<ClSocket *>(sock); // from client
	if (dynamic_cast<Request *>(toSendMessage))
		return CgiSocketFactory::create(*recvdMessage, clSock); // to cgi
	else
		return (clSock); // to client
}

sConnection	setNewSendMessage(Socket *sock, HttpMessage *message)
{
	#ifdef TEST
		std::cout << "setNewSendMessage [" << message->getRaw() << "]" << std::endl;
	#endif

	Router router;
	// ルーティング
	HttpMessage *newMessage = router.routeHandler(*message, sock);
	if (newMessage == NULL)
        return (sConnection(NULL, NULL, ERROR));
	Socket *handleSock = getHandleSock(sock, message, newMessage);
	if (handleSock == NULL)
	{
		delete (newMessage);
        return (sConnection(NULL, NULL, ERROR));
	}
    return (sConnection(handleSock, newMessage, WRITE));
}

ssize_t		clientWrite(Socket *sock, HttpMessage *message)
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

sConnection	finishSend(Socket *sock)
{
	if (CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock))
	{
        (void)cgiSock;
		// if ((shutdown(sock->getFd(), SHUT_WR) == -1 && errno != ENOTCONN) 
		// 	|| addMapAndSockList(sock, new(std::nothrow) CgiResponse(), E_RECV))
		// {
		// 	perror("shutdown: ");
		// 	setCgiErrorResponse(cgiSock, false);
		// 	socketDeleter(cgiSock);
		// 	return (1);
		// }
        return (sConnection(NULL, NULL, ERROR));
	}
	else
	{
        ClSocket *clSock = dynamic_cast<ClSocket *>(sock);
		if (clSock->getMaxRequest() == 0)
            return (sConnection(NULL, NULL, ERROR));
        else
            return (sConnection(clSock, new Request(clSock), READ));
	}
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
                sConnection newMessage = setNewSendMessage(client, recvMessage);
                delete recvMessage;
                if (newMessage.type == ERROR)
                {
                    delete client;
                    return -1;
                }
                else
                {
                    client = dynamic_cast<ClSocket *>(newMessage.sock);
                    sendMessage = newMessage.message;
                    return createRequest(client->getFd(), newMessage.type);
                }
            }
        }
    }
    else if (getType(req) == WRITE)
    {
		// bool		isCgi = getFd(req) != cgiFd;
        // cgi はまだ実装していない
        bool        isCgi = false;

		ssize_t		ret = clientWrite(client, sendMessage);
		if (isCgi == false && ret == -1)
		{
            delete sendMessage;
            delete client;
            return -1;
		}
		else if (sendMessage->doesSendEnd() || ret == -1)
		{
			finishSend(client);
            delete sendMessage;
		}
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