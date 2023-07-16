#!/bin/bash
HOST='localhost'
PORT=80
TIMEOUT=1

ARG=$1

# リクエスト
STATUS_CODE=$(echo -e "$ARG" | nc -i $TIMEOUT $HOST $PORT | grep HTTP/1.1 | awk '{print $2}')

# テスト結果を保存
echo $STATUS_CODE > ./tests/status.log