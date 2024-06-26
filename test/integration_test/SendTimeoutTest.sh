#!/bin/bash

<<COMMENTOUT
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientを実行し、webservにリクエストを送る。
3. 想定sendtimeout時間分sleepし、client processがtimeoutされているかどうか確認する
COMMENTOUT

# init variable
readonly SCRIPT_DIR=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR}/../../webserv"
readonly CLIENT_SEND_TIMEOUT_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/send_timeout"
readonly CLIENT_NO_RECEIVE_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/no_recv"
readonly TEST_NAME="SendTimeout Test"
readonly MAKEFILE_NAME="MAKE_SENDTIMEOUT"
G_SEND_TIMEOUT_IN_MACOS=0
# cnt
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
# url
Scheme="http"
Host="127.0.0.1"
Port="4200"
# script color
GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"
# expect_result
DISCONNECT="true"
STAY_CONNECT="false"

# functions
function init {
  printf "${GREEN}make executable ......${RESET}\n\n"
  make -j -C "${SCRIPT_DIR}/../../" >/dev/null
  if [ ! -f ${WEBSERV_PATH} ]; then
    echo "Build webserv failed"
    exit 1
  fi
  make -j -C "${SCRIPT_DIR}/test_files/TimeoutTestFiles/" -f "${MAKEFILE_NAME}" >/dev/null
  if [ ! -f ${CLIENT_SENDTIMEOUT_PATH} ]; then
    echo "Build send_timeout failed"
    clean "${RED}"
    exit 1
  fi
  if [ ! -f ${CLIENT_NO_RECEIVE_PATH} ]; then
    echo "Build no_recv failed"
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
  ${client_executable} "${server_ip}" "${server_port}" "${sleep_time}" "${request1}" "${request2}" >/dev/null 2>&1 &
  # debug 出力する場合
  #${client_executable} "${server_ip}" "${server_port}" "${sleep_time}" "${request1}" "${request2}" &
  CLIENT_PID=$!
}

function assert {
  local uri=$1
  local expect_sec=$2
  expect_sec=$(bc <<<"$expect_sec + $G_SEND_TIMEOUT_IN_MACOS")
  local expect_result=$3
  local client_executable=$4
  local executable_name=$5
  local sleep_between_case=$6
  local url="${Scheme}://${Host}:${Port}${uri}"
  local request1="GET ${uri} HTTP/1.1"
  local request2="Host: _"

  ((TOTAL_TESTS++))
  printf "[  test$TOTAL_TESTS  ]\n${url}: "

  # program 実行
  runClient "${client_executable}" "${Host}" "${Port}" "${expect_sec}" "${request1}" "${request2}"

  # bufferが詰まるまでに時間がかかることがあるので他のテストよりも長めにsleep
  sleep $(bc <<<"$expect_sec + 1.5")

  # 判定
  #ps | grep "${executable_name}" | grep -v grep
  pgrep "${executable_name}"
  local client_running=$?
  if [ "$client_running" -eq 1 ]; then
    if [ "$expect_result" = ${DISCONNECT} ]; then
      printf "${GREEN}passed.${RESET}\nServer closed the connection\n"
      ((PASSED_TESTS++))
    else
      printErr "${RED}failed.${RESET}\nServer closed the connection"
      ((FAILED_TESTS++))
    fi
  else # clientが正常にタイムアウトする前にsleepが終了
    kill $(pgrep "${executable_name}")
    if [ "$expect_result" = ${DISCONNECT} ]; then
      printErr "${RED}failed.${RESET}\nServer did not timeout"
      ((FAILED_TESTS++))
    else
      printf "${GREEN}passed.${RESET}\nServer did not timeout\n"
      ((PASSED_TESTS++))
    fi
  fi
  printf "\n"
  sleep $(bc <<<"$sleep_between_case") # selectのみsleep
}

function runTest {
  local conf=$1
  local server_name=$2
  local sleep_between_case=$3
  local root="test/integration_test/test_files/TimeoutTestFiles"

  printf "\n${GREEN}<<< ${server_name} server test >>>${RESET}\n"
  runServer "${root}/${conf}"

  # テスト実行
  assert "/timeout5/" "5" ${DISCONNECT} "${CLIENT_SEND_TIMEOUT_PATH}" "send_timeout" ${sleep_between_case}
  assert "/timeout10/" "10" ${DISCONNECT} "${CLIENT_SEND_TIMEOUT_PATH}" "send_timeout" ${sleep_between_case}
  assert "/timeout5/" "3" ${STAY_CONNECT} "${CLIENT_SEND_TIMEOUT_PATH}" "send_timeout" ${sleep_between_case}
  assert "/timeout10/" "8" ${STAY_CONNECT} "${CLIENT_SEND_TIMEOUT_PATH}" "send_timeout" ${sleep_between_case}
  # /no-recv/へのテストは本来keepalive_timeoutのテストですが、テストの形式の関係でこちらで行っている。
  assert "/no-recv/" "3" ${DISCONNECT} "${CLIENT_NO_RECEIVE_PATH}" "no_recv" ${sleep_between_case}
  assert "/no-recv/" "1" ${STAY_CONNECT} "${CLIENT_NO_RECEIVE_PATH}" "no_recv" ${sleep_between_case}
  assert "/timeout0/" "3" ${STAY_CONNECT} "${CLIENT_SEND_TIMEOUT_PATH}" "send_timeout" ${sleep_between_case}

  # サーバープロセスを終了
  #kill "${WEBSERV_PID}"
  kill "${WEBSERV_PID}" >/dev/null 2>&1
}

function main {
  # 環境によってはsocketが詰まるまでに時間がかかるので、その遅延分を引数で受け取る。
  # mac os では大体22秒くらい、場合によっては40秒かかる
  # linuxでは渡す必要はない
  # 何も渡されなければ0
  G_SEND_TIMEOUT_IN_MACOS=${1:-0}
  init

  # 第三引数の数値はテストケース間のスリープ秒数
  # selectはeofを感知できないので、timeoutが終わるまで待たないといけない。
  runTest "send_timeout.conf" "kqueue or epoll" "0" # kqueue or epoll
  runTest "send_timeout_poll.conf" "poll" "0"       # poll
  runTest "send_timeout_select.conf" "select" "3"   # select

  printLog

  clean "${RESET}"

  if [ $(uname) == "Darwin" ]; then
    ((FAILED_TESTS != 0)) && printErr "The test failed on Darwin, but this is expected.\nIf you want to know the details, please run this test individually with send_timeout time."
    return 0
  fi
  if [ ${FAILED_TESTS} -ne 0 ]; then
    return 1
  fi
  return 0
}

main "$@"
