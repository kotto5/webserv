#!/bin/bash

# サーバーの終了
PID=$(cat ./tests/pid.file)
kill $PID
