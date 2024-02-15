#include "Connection.hpp"

Connection::Connection(ClSocket *sock): client(sock), cgiFd(-1)
{
}

Connection::~Connection()
{
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