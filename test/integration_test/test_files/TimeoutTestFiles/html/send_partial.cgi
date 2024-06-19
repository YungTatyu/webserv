#!/bin/bash

# HTTPレスポンスの送信
echo -ne "Status: 200\r\nContent-Type: text/html\r\nContent-Length: 500\r\n\r\n"
echo -ne "<html><body>"
echo -ne "sending partial response ..."

# 出力をフラッシュする
#exec > /dev/null 2>&1

# ずっとスリープ
while true; do
    sleep 60
done
