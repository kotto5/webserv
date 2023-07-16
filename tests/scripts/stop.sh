#!/bin/bash

# サーバーの終了
PID=$(cat ./tests/pid.log)
kill $PID
