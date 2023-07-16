#!/bin/bash
HOST='localhost'
PORT=80
TIMEOUT=1

METHOD=$1
URI=$2
HEADER=$3
BODY=$4

# リクエスト
STATUS_CODE=$(echo -e "$METHOD $URI HTTP/1.1\r\n$HEADER\r\n\r\n$BODY" | nc -i $TIMEOUT $HOST $PORT | grep HTTP/1.1 | awk '{print $2}')

# テスト結果を保存
echo $STATUS_CODE > ./tests/status_code.txt