#!/bin/bash

# ディレクトリパス
DIR_PATH=$1

# ディレクトリとすべてのファイルを削除
rm -rf $DIR_PATH
# ディレクトリを作成
mkdir $DIR_PATH
chmod -R 777 $DIR_PATH

echo "Test directory is created: $DIR_PATH"