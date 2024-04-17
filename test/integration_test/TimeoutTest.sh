#!/bin/bash

# init
readonly SCRIPT_DIR=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR}/../../webserv"
readonly TEST_NAME="Timeout Test"
readonly OS=$(uname -s)

if [ ! -e $WEBSERV_PATH ]; then
	echo "${WEBSERV_PATH}: command not found"
	echo "run \"make\" first to test"
	exit 1
fi

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# script color
GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"

# functions
function	runServer {
	local conf=$1
	$WEBSERV_PATH "$conf" > /dev/null 2>&1 &
	# エラー出力する場合
	# $WEBSERV_PATH $1 > /dev/null &
	WEBSERV_PID=$!
}

function	assert {
	local	uri=$1
	local	expect_sec=$2
	local	expect_result=$3
	local	scheme="http"
	local	host="127.0.0.1"
	local	port="4242"
	local	url="$scheme://$host:$port$uri"
	local	request=$(cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
)

	((TOTAL_TESTS++))
	printf "[  test$TOTAL_TESTS  ]\n${url}: "

	(printf "$request"; sleep $(bc <<< "$expect_sec + 5")) | telnet $host $port > /dev/null 2>&1 &

	# telnetがタイムアウトして1秒以内の誤差であればTRUE
	sleep $(bc <<< "$expect_sec + 1")
	ps | grep -q "telnet"
	local telnet_running=$?

	if [ "$telnet_running" -eq 1 ]; then
		kill $(ps | grep "sleep" | grep -v grep | cut -d ' ' -f1) >/dev/null 2>&1
		if [ "$expect_result" = "true" ]; then
			printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
			((PASSED_TESTS++))
		else
			printf "${RED}failed.${RESET}\nServer did not timeout\n"
			((FAILED_TESTS++))
		fi
	else # telnetが正常にタイムアウトする前にsleepが終了
		kill $(ps | grep "telnet" | grep -v grep | cut -d ' ' -f1) > /dev/null 2>&1
		if [ "$expect_result" = "true" ]; then
			printf "${RED}failed.${RESET}\nServer did not timeout\n"
			((FAILED_TESTS++))
		else
			printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
			((PASSED_TESTS++))
		fi
	fi
}

function	printLog {
	printf "\n|------------------ ${TEST_NAME} results ------------------|\n"
	printf "[========]    ${TOTAL_TESTS} tests ran\n"
	printf "[ ${GREEN}PASSED${RESET} ]    ${PASSED_TESTS} tests\n"
	printf "[ ${RED}FAILED${RESET} ]    ${FAILED_TESTS} tests\n"
}

function	runTest {
	local	conf=$1
	local	server_name=$2
	local	root="test/integration_test/test_files/TimeoutTestFiles"
	runServer "${root}/${conf}"

	printf "\n${GREEN}<<< ${server_name} server test >>>${RESET}\n"
	assert "/" "0" "true"
	assert "/timeout5/" "5" "true"
	assert "/timeout10/" "10" "true"
	assert "/timeout5/" "3" "false"
	assert "/timeout10/" "8" "false"

	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

printf "|------------------ ${TEST_NAME} start ------------------|\n"

runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
runTest "keepalive_timeout_select.conf" "select" # select
runTest "keepalive_timeout_poll.conf" "poll" # poll

printLog
