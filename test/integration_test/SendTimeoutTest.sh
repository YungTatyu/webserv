#!/bin/bash

# init
readonly SCRIPT_DIR=$(dirname "$0")
readonly TEST_NAME="Timeout Test"
make -j -C "${SCRIPT_DIR}/../../"
if [ $? -eq 1 ]; then
	echo "Build webserv failed"
	exit 1
fi
readonly WEBSERV_PATH="${SCRIPT_DIR}/../../webserv"
make -j -C "${SCRIPT_DIR}/test_files/TimeoutTestFiles/"
if [ $? -eq 1 ]; then
	echo "Build client failed"
	rm "${WEBSERV_PATH}"
	exit 1
fi
readonly CLIENT_TIMEOUT5_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/timeout5"
readonly CLIENT_TIMEOUT10_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/timeout10"

if [ ! -e $CLIENT_TIMEOUT5_PATH ]; then
	echo "${CLIENT_TIMEOUT5_PATH}: command not found"
	echo "run \"make\" first to test"
	rm "${WEBSERV_PATH}"
	exit 1
fi
if [ ! -e $CLIENT_TIMEOUT10_PATH ]; then
	echo "${CLIENT_TIMEOUT10_PATH}: command not found"
	echo "run \"make\" first to test"
	rm "${WEBSERV_PATH}"
	rm $CLIENT_TIMEOUT5_PATH
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
function	clean {
	if [ -f "${CLIENT_TIMEOUT5_PATH}" ]; then
		rm "${CLIENT_TIMEOUT5_PATH}"
		echo "Clean: client executable removed"
	fi
	if [ -f "${CLIENT_TIMEOUT10_PATH}" ]; then
		rm "${CLIENT_TIMEOUT10_PATH}"
		echo "Clean: client executable removed"
	fi
	if [ -f "${WEBSERV_PATH}" ]; then
		rm "${WEBSERV_PATH}"
		echo "Clean: webserv executable removed"
	fi
}

function	runServer {
	local conf=$1
	$WEBSERV_PATH "$conf" > /dev/null 2>&1 &
	# debug 出力する場合
	#$WEBSERV_PATH "$conf" &
	sleep 1
	WEBSERV_PID=$!
}

function	runClient {
	local client_executable=$1
	local server_ip=$2
	local server_port=$3
	local request=$4
	local sleep_time=$5
	${client_executable} "$server_ip" "$server_port" "$request" "$sleep_time" > /dev/null 2>&1 &
	# debug 出力する場合
	#${client_executable} "$server_ip" "$server_port" "$request" "$sleep_time"
	CLIENT_PID=$!
}

function	assert {
	local	uri=$1
	local	expect_sec=$2
	local	expect_result=$3
	local	client_executable=$4
	local	executable_name=$5
	local	scheme="http"
	local	host="127.0.0.1"
	local	port="4200"
	local	url="$scheme://$host:$port$uri"
	local	request=$(cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
)

	((TOTAL_TESTS++))
	printf "[  test$TOTAL_TESTS  ]\n${url}: "

	# program 実行
	runClient "${client_executable}" "${host}" "${port}" "${request}" "${expect_sec}" &
	sleep $(bc <<< "$expect_sec + 1.5")

	# 判定
	#ps | grep "${executable_name}" | grep -v grep
	ps | grep "${executable_name}" | grep -v grep > /dev/null 2>&1
	local client_running=$?
	if [ "$client_running" -eq 1 ]; then
		if [ "$expect_result" = "true" ]; then
			printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
			((PASSED_TESTS++))
		else
			printf "${RED}failed.${RESET}\nServer closed the connection\n"
			((FAILED_TESTS++))
		fi
	else # clientが正常にタイムアウトする前にsleepが終了
		kill $(ps | grep "${executable_name}" | grep -v grep | cut -d ' ' -f1) > /dev/null 2>&1
		#kill $(ps | grep "${executable_name}" | grep -v grep | cut -d ' ' -f1)
		if [ "$expect_result" = "true" ]; then
			printf "${RED}failed.${RESET}\nServer did not timeout\n"
			((FAILED_TESTS++))
		else
			printf "${GREEN}passed.${RESET}\nServer did not timeout\n\n"
			((PASSED_TESTS++))
		fi
	fi
	sleep 2
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

	printf "\n${GREEN}<<< ${server_name} server test >>>${RESET}\n"
	runServer "${root}/${conf}"

	# テスト実行
	#assert "/" "0" "true"
	assert "/timeout5/" "5" "true" "${CLIENT_TIMEOUT5_PATH}" "timeout5"
	assert "/timeout10/" "10" "true" "${CLIENT_TIMEOUT10_PATH}" "timeout10"
	assert "/timeout5/" "3" "false" "${CLIENT_TIMEOUT5_PATH}" "timeout5"
	assert "/timeout10/" "8" "false" "${CLIENT_TIMEOUT10_PATH}" "timeout10"

	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

printf "|------------------ ${TEST_NAME} start ------------------|\n"

runTest "send_timeout.conf" "kqueue or epoll" # kqueue or epoll
runTest "send_timeout_select.conf" "select" # select
runTest "send_timeout_poll.conf" "poll" # poll

clean

printLog
