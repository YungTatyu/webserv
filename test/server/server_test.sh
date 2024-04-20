#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# サーバープログラムをバックグラウンドで起動
./webserv $1  > /dev/null &
WEBSERV_PID=$!

# 起動プロセス中ちょっと待つ
sleep 1

# リクエストを送信し、レスポンスをチェックする
send_request() {
  local url=$1
  local response=$(curl -o /dev/null -s -w "%{http_code}\n" "$url")

  if [ "$response" == "200" ]; then
    echo -e "${GREEN}URL $url: 正常なレスポンスが返ってきました。${NC}"
  else
    echo -e "${RED}URL $url: エラー、ステータスコード $response${NC}"
  fi
}

# リクエストを送信
# CGIスクリプト
send_request "http://localhost:3001/cgi/script.php"
send_request "http://localhost:3001/cgi/rich.php"

# CGIスクリプトのクエリ付き
send_request "http://localhost:3001/cgi/script.php?aa"

# ディレクトリ指定のときにindex.html表示
send_request "http://localhost:3001/html"

# autoindex
send_request "http://localhost:3001/"

# 存在しないファイルはエラー
send_request "http://localhost:3001/aa"

# サーバープロセスを終了
kill $WEBSERV_PID
