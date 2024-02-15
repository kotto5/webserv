#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Socket.hpp"

typedef int sSelectRequest;

enum eSelectType
{
    READ,
    WRITE,
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
    ClSocket *client;
    int cgiFd;

public:
    Connection(ClSocket *sock);
    ~Connection();
    sSelectRequest handleEvent();

    static int             getFd(sSelectRequest req);
    static eSelectType     getType(sSelectRequest req);
    static sSelectRequest  createRequest(int fd, eSelectType type);
};

#endif
