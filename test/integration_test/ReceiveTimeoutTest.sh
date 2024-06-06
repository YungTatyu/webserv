#!/bin/bash

<<COMMENTOUT
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientを実行し、webservにリクエストをそもそも送らないか、一部だけ送って途中で止める。
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
COMMENTOUT

# init variable
readonly SCRIPT_DIR=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR}/../../webserv"
readonly CLIENT_RECV_TIMEOUT_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/recv_timeout"
readonly CLIENT_NO_SEND_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/no_send"
readonly TEST_NAME="ReceiveTimeout Test"
readonly MAKEFILE_NAME="MAKE_RECEIVETIMEOUT"
# cnt
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
# url
Scheme="http"
Host="127.0.0.1"
Port="4300"
# script color
GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"

# functions
function init {
  printf "${GREEN}make executable ......${RESET}\n\n"
  make -j -C "${SCRIPT_DIR}/../../" >/dev/null
  if [ ! -e ${WEBSERV_PATH} ]; then
    echo "Build webserv failed"
    exit 1
  fi
  make -j -C "${SCRIPT_DIR}/test_files/TimeoutTestFiles/" -f "${MAKEFILE_NAME}" >/dev/null
  if [ ! -f ${CLIENT_RECV_TIMEOUT_PATH} ]; then
    echo "Build recv_timeout failed"
    clean "${RED}"
    exit 1
  fi
  if [ ! -f ${CLIENT_NO_SEND_PATH} ]; then
    echo "Build no_send failed"
    clean "${RED}"
    exit 1
  fi
  printf "|------------------ ${TEST_NAME} start ------------------|\n"
  trap signalHandler HUP INT QUIT ABRT KILL TERM
}

function clean {
  local color=$1
  printf "${color}make fclean client.${RESET}\n"
  make fclean -C "${SCRIPT_DIR}/test_files/TimeoutTestFiles/" -f "${MAKEFILE_NAME}" >/dev/null
}

function Kill {
  local target_name=$1
  local target_pid=$2
  local color=$3
  kill ${target_pid} >/dev/null 2>&1
  clean "${RED}"
  printErr "${color}kill ${target_name}.${RESET}"
}

function signalHandler {
  printErr "\n\n${RED}${TEST_NAME} interrupted: Signal received.${RESET}"
  Kill "webserv" "${WEBSERV_PID}" "${RED}"
  Kill "client" "${CLIENT_PID}" "${RED}"
  exit 1
}

function printLog {
  printf "\n|------------------ ${TEST_NAME} results ------------------|\n"
  printf "[========]    ${TOTAL_TESTS} tests ran\n"
  printf "[ ${GREEN}PASSED${RESET} ]    ${PASSED_TESTS} tests\n"
  printf "[ ${RED}FAILED${RESET} ]    ${FAILED_TESTS} tests\n"
}

function printErr {
  printf "${*}\n" >&2
}

function runServer {
  local conf=$1
  ${WEBSERV_PATH} "${conf}" >/dev/null 2>&1 &
  # debug 出力する場合
  #$WEBSERV_PATH "$conf" &
  sleep 1
  WEBSERV_PID=$!
}

function runClient {
  local client_executable=$1
  local server_ip=$2
  local server_port=$3
  local sleep_time=$4
  local request1=$5
  local request2=$6
  ${client_executable} "$server_ip" "$server_port" "$sleep_time" "$request1" "$request2" >/dev/null 2>&1 &
  # debug 出力する場合
  #${client_executable} "$server_ip" "$server_port" "$request" "$sleep_time"
  CLIENT_PID=$!
}

function assert {
  local uri=$1
  local expect_sec=$2
  local expect_result=$3
  local client_executable=$4
  local executable_name=$5
  local url="${Scheme}://${Host}:${Port}${uri}"
  local request1="GET ${uri} HTTP/1.1"
  local request2="Host: _"

  ((TOTAL_TESTS++))
  printf "[  test$TOTAL_TESTS  ]\n${url}: "

  # program 実行
  runClient "${client_executable}" "${Host}" "${Port}" "${expect_sec}" "${request1}" "${request2}" &
  sleep $(bc <<<"${expect_sec} + 1.5")

  # 判定
  #ps | grep "${executable_name}" | grep -v grep
  ps | grep "${executable_name}" | grep -v grep >/dev/null 2>&1
  local client_running=$?
  if [ "$client_running" -eq 1 ]; then
    if [ "$expect_result" = "true" ]; then
      printf "${GREEN}passed.${RESET}\nServer closed the connection\n"
      ((PASSED_TESTS++))
    else
      printErr "${RED}failed.${RESET}\nServer closed the connection"
      ((FAILED_TESTS++))
    fi
  else # clientが正常にタイムアウトする前にsleepが終了
    #kill $(ps | grep "${executable_name}" | grep -v grep | awk '{print $1}') >/dev/null 2>&1
    kill $(ps | grep "${executable_name}" | grep -v grep | awk '{print $1}')
    if [ "$expect_result" = "true" ]; then
      printErr "${RED}failed.${RESET}\nServer did not timeout"
      ((FAILED_TESTS++))
    else
      printf "${GREEN}passed.${RESET}\nServer did not timeout\n"
      ((PASSED_TESTS++))
    fi
  fi
  printf "\n"
}

function runTest {
  local conf=$1
  local server_name=$2
  local root="test/integration_test/test_files/TimeoutTestFiles"

  printf "\n${GREEN}<<< ${server_name} server test >>>${RESET}\n"
  runServer "${root}/${conf}"

  # テスト実行
  assert "/timeout0/" "3" "false" "${CLIENT_RECV_TIMEOUT_PATH}" "timeout0"
  assert "/timeout5/" "5" "true" "${CLIENT_RECV_TIMEOUT_PATH}" "timeout5"
  assert "/timeout10/" "10" "true" "${CLIENT_RECV_TIMEOUT_PATH}" "timeout10"
  assert "/timeout5/" "3" "false" "${CLIENT_RECV_TIMEOUT_PATH}" "timeout5"
  assert "/timeout10/" "8" "false" "${CLIENT_RECV_TIMEOUT_PATH}" "timeout10"
  # このテストは本来keepalive_timeoutのテストですが、テストの形式の関係でとりあえずこちらで行っています。
  assert "/no-send/" "3" "true" "${CLIENT_NO_SEND_PATH}" "no_send"
  assert "/no-send/" "1" "false" "${CLIENT_NO_SEND_PATH}" "no_send"

  # サーバープロセスを終了
  kill "${WEBSERV_PID}"
  #kill "${WEBSERV_PID}" > /dev/null 2>&1
}

function main {
  init

  runTest "receive_timeout.conf" "kqueue or epoll" # kqueue or epoll
  runTest "receive_timeout_poll.conf" "poll"       # poll
  runTest "receive_timeout_select.conf" "select"   # select

  printLog

  clean "${RED}"

  if [ ${FAILED_TESTS} -ne 0 ]; then
    return 1
  fi
  return 0
}

main "$@"
