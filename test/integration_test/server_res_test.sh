#!/bin/bash

# init
readonly SCRIPT_DIR_PATH=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR_PATH}/../../webserv"
readonly SV_RES_DYNAMIC_PATH="${SCRIPT_DIR_PATH}/test_files/server_res_test/dynamic"
readonly TEST_NAME="server response test"

readonly WHITE="\033[0m"
readonly GREEN="\033[32m"
readonly RED="\033[31m"

g_test_cnt=0
g_test_index=0
g_test_passed=0
g_test_failed=0

function init {
  if [ -e ${WEBSERV_PATH} ]; then
    printf "|------------------ ${TEST_NAME} start ------------------|\n"
  else
    printErr "${WEBSERV_PATH}: command not found"
    printErr "run \"make\" first to test"
    exit 1
  fi
  trap signalHandler HUP INT QUIT ABRT KILL TERM
  make -C ${SV_RES_DYNAMIC_PATH} >/dev/null
}

# signal受信時に実行
function signalHandler {
  printErr "\n${TEST_NAME} interrupted: Signal received."
  kill ${webserv_pid}
  make fclean -C ${SV_RES_DYNAMIC_PATH} >/dev/null
  exit 1
}

function printErr {
  printf "${*}\n" >&2
}

function printLog {
  printf "\n|------------------ ${TEST_NAME} results ------------------|\n"
  printf "[========]    ${g_test_cnt} tests ran\n"
  printf "[ ${GREEN}PASSED${WHITE} ]    ${g_test_passed} tests\n"
  printf "[ ${RED}FAILED${WHITE} ]    ${g_test_failed} tests\n"
}

function runServer {
  # ${WEBSERV_PATH} $1 >/dev/null 2>&1 &
  # エラー出力する場合
  $WEBSERV_PATH $1 >/dev/null &
  webserv_pid=$!
  sleep 1
}

# responseのstatusをテスト
function assert {
  ((++g_test_cnt))
  ((++g_test_index))

  local uri=$1
  local request="localhost:4242/${uri}"
  local method=$3
  local option=$4
  printf "[  test${g_test_index}  ]\n${request} ${method}: "

  # responseのtimeoutを1秒に設定 --max-time
  local actual=$(curl -X ${method} ${option} -s -o /dev/null -w "%{http_code}" ${request} --max-time 1.5)
  local expect=$2
  if [ "${actual}" == "${expect}" ]; then
    printf "${GREEN}passed${WHITE}\n\n"
    ((++g_test_passed))
  else
    printErr "${RED}failed\n${WHITE}"
    printErr "expected: \"${expect}\""
    printErr "actual  : \"${actual}\"\n"
    ((++g_test_failed))
  fi
}

function runTest {
  local root="test/integration_test/test_files/server_res_test"
  local conf=$1
  local server_name=$2
  g_test_index=0

  runServer "${root}/${conf}"
  printf "\n${GREEN}<<< ${server_name} server test >>>${WHITE}\n"

  # 以下にテストを追加
  # GET
  assert "${root}/static/index.html" "200" "GET" ""
  assert "${root}/static/index.html" "400" "GET" "-H Host:"
  assert "${root}/nonexist" "404" "GET" ""
  assert "${root}/static/index.html" "501" "GET" "-H Transfer-Encoding:gzip"
  assert "${root}/dynamic/exit_non_zero.cgi" "500" "GET" ""
  assert "${root}/dynamic/parse_error.py" "502" "GET" ""
  assert "${root}/dynamic/document_response.py" "200" "GET" ""
  assert "${root}/dynamic/local_redirect_res.py" "302" "GET" ""
  assert "${root}/dynamic/client_redirect_res.cgi" "302" "GET" ""
  assert "${root}/dynamic/client_redirect_res_doc.cgi" "302" "GET" ""
  assert "${root}/dynamic/body_res.py" "200" "GET" ""
  assert "${root}/dynamic/post_cgi.py?key=value" "200" "GET" ""
  # HEAD
  assert "${root}/static/index.html" "200" "HEAD" ""
  assert "${root}/static/nonexist" "404" "HEAD" ""
  assert "${root}/dynamic/post_cgi.py?key=value" "200" "HEAD" ""
  assert "${root}/dynamic/exit_non_zero.cgi" "500" "GET" ""
  assert "${root}/dynamic/parse_error.py" "502" "GET" ""
  assert "${root}/dynamic/document_response.py" "200" "GET" ""
  assert "${root}/dynamic/local_redirect_res.py" "302" "GET" ""
  assert "${root}/dynamic/client_redirect_res.cgi" "302" "GET" ""
  assert "${root}/dynamic/client_redirect_res_doc.cgi" "302" "GET" ""
  assert "${root}/dynamic/body_res.py" "200" "GET" ""
  assert "${root}/dynamic/post_cgi.py?key=value" "200" "GET" ""

  # POST
  assert "${root}/dynamic/post_cgi.py" "200" "POST" "-d key=value"
  assert "${root}/dynamic/post_cgi.py" "400" "POST" "-d invalid=value"
  assert "${root}/static/index.html" "405" "POST" ""
  assert "${root}/dynamic/post_cgi.py" "400" "POST" "-d ''"
  assert "${root}/dynamic/exit_non_zero.cgi" "500" "POST" ""
  assert "${root}/dynamic/parse_error.py" "502" "POST" ""
  assert "${root}/dynamic/document_response.py" "200" "POST" ""
  assert "${root}/dynamic/local_redirect_res.py" "302" "POST" ""
  assert "${root}/dynamic/client_redirect_res.cgi" "302" "POST" ""
  assert "${root}/dynamic/client_redirect_res_doc.cgi" "302" "POST" ""
  assert "${root}/dynamic/body_res.py" "200" "POST" ""

  # DELETE
  assert "${root}/dynamic/post_cgi.py" "405" "DELETE" ""
  assert "${root}/static/index.html" "405" "DELETE" ""
  assert "${root}/dynamic/exit_non_zero.cgi" "500" "DELETE" ""
  assert "${root}/dynamic/parse_error.py" "502" "DELETE" ""
  assert "${root}/dynamic/document_response.py" "200" "DELETE" ""
  assert "${root}/dynamic/local_redirect_res.py" "302" "DELETE" ""
  assert "${root}/dynamic/client_redirect_res.cgi" "302" "DELETE" ""
  assert "${root}/dynamic/client_redirect_res_doc.cgi" "302" "DELETE" ""
  assert "${root}/dynamic/body_res.py" "200" "DELETE" ""

  # サーバープロセスを終了
  kill ${webserv_pid} >/dev/null 2>&1
}

function main {
  init
  runTest "server_res_test.conf" "kqueue or epoll" # kqueue or epoll
  runTest "server_res_test_select.conf" "select"   # select
  runTest "server_res_test_poll.conf" "poll"       # poll

  printLog

  make fclean -C ${SV_RES_DYNAMIC_PATH} >/dev/null

  if [ ${g_test_failed} -ne 0 ]; then
    return 1
  fi

  return 0
}

main "$@"
