#ifndef CONNECTION_HPP
#define CONNECTION_HPP

enum eSelectType
{
    READ,
    WRITE
};

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

struct sSelectRequest
{
    int         fd;
    eSelectType type;
};

class Connection
{
private:
    int clientFd;
    int cgiFd;

public:
    Connection();
    ~Connection();
    sSelectRequest handleEvent(bool isSet);
};

Connection::Connection(/* args */)
{
}

Connection::~Connection()
{
}


#endif
