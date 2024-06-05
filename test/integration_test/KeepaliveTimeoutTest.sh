#!/bin/bash

<<COMMENTOUT
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにtelnetを実行し、webservにリクエストを送る。
3. 想定sendtimeout時間分sleepし、telnet processがtimeoutされているかどうか確認する
COMMENTOUT

# init variable
readonly SCRIPT_DIR=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR}/../../webserv"
readonly TEST_NAME="KeepaliveTimeout Test"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
Scheme="http"
Host="127.0.0.1"
Port="4242"
# script color
GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"

# functions
function init {
  printf "${GREEN}make webserv ......${RESET}\n\n"
  make -j -C "${SCRIPT_DIR}/../../" >/dev/null
  if [ -e ${WEBSERV_PATH} ]; then
    printf "|------------------ ${TEST_NAME} start ------------------|\n"
  else
    printErr "${WEBSERV_PATH}: command not found"
    printErr "run \"make\" first to test"
    exit 1
  fi
  trap signalHandler HUP INT QUIT ABRT KILL TERM
}

function Kill {
  local target_pid=$1
  local color=$2
  kill ${target_pid} >/dev/null 2>&1
  printErr "${color}kill webserv.${RESET}"
}

function signalHandler {
  printErr "\n\n${RED}${TEST_NAME} interrupted: Signal received.${RESET}"
  Kill "${WEBSERV_PID}" "${RED}"
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
  $WEBSERV_PATH "$conf" >/dev/null 2>&1 &
  # エラー出力する場合
  # $WEBSERV_PATH $1 > /dev/null &
  WEBSERV_PID=$!
}

function assert {
  local uri=$1
  local expect_sec=$2
  local expect_result=$3
  local url="${Scheme}://${Host}:${Port}${uri}"
  local request=$(
    cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
  )

  ((TOTAL_TESTS++))
  printf "[  test$TOTAL_TESTS  ]\n${url}: "

  (
    printf "$request"
    sleep $(bc <<<"$expect_sec + 5")
  ) | telnet ${Host} ${Port} >/dev/null 2>&1 &

  # telnetがタイムアウトして1秒以内の誤差であればTRUE
  sleep $(bc <<<"$expect_sec + 1")
  ps | grep -q "telnet"
  local telnet_running=$?

  if [ "$telnet_running" -eq 1 ]; then
    kill $(ps | grep "sleep" | grep -v grep | cut -d ' ' -f2) >/dev/null 2>&1
    if [ "$expect_result" = "true" ]; then
      printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
      ((PASSED_TESTS++))
    else
      printErr "${RED}failed.${RESET}\nServer did not timeout"
      ((FAILED_TESTS++))
    fi
  else # telnetが正常にタイムアウトする前にsleepが終了
    kill $(ps | grep "telnet" | grep -v grep | cut -d ' ' -f2) >/dev/null 2>&1
    if [ "$expect_result" = "true" ]; then
      printErr "${RED}failed.${RESET}\nServer did not timeout"
      ((FAILED_TESTS++))
    else
      printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
      ((PASSED_TESTS++))
    fi
  fi
}

function runTest {
  local conf=$1
  local server_name=$2
  local root="test/integration_test/test_files/TimeoutTestFiles"
  runServer "${root}/${conf}"

  printf "\n${GREEN}<<< ${server_name} server test >>>${RESET}\n"
  assert "/" "0" "true"
  assert "/timeout5/" "5" "true"
  assert "/timeout10/" "10" "true"
  assert "/timeout5/" "3" "false"
  assert "/timeout10/" "8" "false"

  # サーバープロセスを終了
  kill "${WEBSERV_PID}"
}

function main {
  init

  runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
  runTest "keepalive_timeout_select.conf" "select"   # select
  runTest "keepalive_timeout_poll.conf" "poll"       # poll

  printLog

  if [ ${FAILED_TESTS} -ne 0 ]; then
    return 1
  fi
  return 0
}

main "$@"
