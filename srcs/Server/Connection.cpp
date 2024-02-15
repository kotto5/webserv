#include "Connection.hpp"

Connection::Connection(ClSocket *sock): client(sock), cgiFd(-1)
{
}

Connection::~Connection()
{
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