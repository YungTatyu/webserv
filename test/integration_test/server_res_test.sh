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
  printf "[  test${g_test_index}  ]\n${request}: "

  # responseのtimeoutを1秒に設定 --max-time
  local actual=$(curl -X ${method} -s -o /dev/null -w "%{http_code}" ${request} --max-time 1)
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
  assert "${root}/static/index.html" "200" "GET"
  assert "${root}/nonexist" "404" "GET"
  assert "${root}/dynamic/document_response.py" "200" "GET"
  assert "${root}/dynamic/local_redirect_res.py" "302" "GET"
  assert "${root}/dynamic/client_redirect_res.cgi" "302" "GET"
  assert "${root}/dynamic/client_redirect_res_doc.cgi" "302" "GET"
  assert "${root}/dynamic/body_res.py" "200" "GET"
  assert "${root}/dynamic/exit_non_zero.cgi" "500" "GET"

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
