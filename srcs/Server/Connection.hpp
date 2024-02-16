#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Socket.hpp"
#include "HttpMessage.hpp"

typedef int sSelectRequest;

enum eSelectType
{
    READ,
    WRITE,
    ERROR,
};

struct sConnection {
    sConnection(Socket *sock, HttpMessage *message, eSelectType type): sock(sock), message(message), type(type) {}

    Socket      *sock;
    HttpMessage *message;
    eSelectType type;
};

#define MAX_SOCKETS 1024

/*
input: イベント(isSet)
calculate: イベント処理
output: 監視対象のファイルディスクリプタとイベントタイプ

イベント処理
if read
    if cgiFd
        cgiFdを読み込む
    else
        clientFdを読み込む
else if write
    if cgiFd
        cgiFdを書き込む
    else
        clientFdを書き込む
else
    エラー
*/


class Connection
{
private:
    ClSocket    *client;
    HttpMessage *recvMessage;
    HttpMessage *sendMessage;

public:
    Connection(ClSocket *sock);
    ~Connection();
    sSelectRequest handleEvent(sSelectRequest req, bool isSet);
    int getFd() const;

    static int             getFd(sSelectRequest req);
    static eSelectType     getType(sSelectRequest req);
    static sSelectRequest  createRequest(int fd, eSelectType type);
};

#endif
