#!/bin/bash

# init
readonly SCRIPT_DIR=$(dirname "$0")
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
readonly CLIENT_TIMEOUT5_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/client_timeout5"
readonly CLIENT_TIMEOUT10_PATH="${SCRIPT_DIR}/test_files/TimeoutTestFiles/client_timeout10"
readonly TEST_NAME="Timeout Test"
readonly OS=$(uname -s)

#if [ ! -e $WEBSERV_PATH ]; then
#	echo "${WEBSERV_PATH}: command not found"
#	echo "run \"make\" first to test"
#	rm "${CLIENT_PATH}"
#	exit 1
#fi
#
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
	#$WEBSERV_PATH "$conf" &
	sleep 1
	# エラー出力する場合
	# $WEBSERV_PATH $1 > /dev/null &
	WEBSERV_PID=$!
}

function	runClient {
	local client_executable=$1
	local server_ip=$2
	local server_port=$3
	local request=$4
	local sleep_time=$5
	$CLIENT_PATH "$server_ip" "$server_port" "$request" "$sleep_time" > /dev/null 2>&1 &
	#${client_executable} "$server_ip" "$server_port" "$request" "$sleep_time"
	# エラー出力する場合
	# $WEBSERV_PATH $1 > /dev/null &
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
	local	port="4242"
	local	url="$scheme://$host:$port$uri"
	local	request=$(cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
)

	((TOTAL_TESTS++))
	printf "[  test$TOTAL_TESTS  ]\n${url}: "
	echo "executable_name is ${executable_name}"

	#(printf "$request"; sleep $(bc <<< "$expect_sec + 5")) | telnet $host $port > /dev/null 2>&1 &
	runClient "${client_executable}" "${host}" "${port}" "${request}" "${expect_sec}" &

	# telnetがタイムアウトして1秒以内の誤差であればTRUE
	#sleep 1
	#ps | grep -q "${executable_name}"
	#local client_running=$?
	#if [ "$client_running" -eq 1 ]; then
	#	printf "${RED}failed.${RESET}\nmaybe you cannot send request correctly.\n"
	#	((FAILED_TESTS++))
	#	return 1
	#fi
	#ps | grep "${executable_name}" | grep -v grep
	sleep $(bc <<< "$expect_sec + 2")
	ps | grep "${executable_name}" | grep -v grep
	local client_running=$?
	echo "client_running is ${client_running}"

	if [ "$client_running" -eq 1 ]; then
		if [ "$expect_result" = "true" ]; then
			printf "${GREEN}passed.${RESET}\nServer closed the connection\n\n"
			((PASSED_TESTS++))
		else
			printf "${RED}failed.${RESET}\nServer did not timeout\n"
			((FAILED_TESTS++))
		fi
	else # clientが正常にタイムアウトする前にsleepが終了
		#kill $(ps | grep "${executable_name}" | grep -v grep | cut -d ' ' -f1) > /dev/null 2>&1
		kill $(ps | grep "${executable_name}" | grep -v grep | cut -d ' ' -f2)
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
	#assert "/" "0" "true"
	assert "/timeout5/" "5" "true" "${CLIENT_TIMEOUT5_PATH}" "client_timeout5"
	assert "/timeout10/" "10" "true" "${CLIENT_TIMEOUT10_PATH}" "client_timeout10"
	assert "/timeout5/" "3" "false" "${CLIENT_TIMEOUT5_PATH}" "client_timeout5"
	assert "/timeout10/" "8" "false" "${CLIENT_TIMEOUT10_PATH}" "client_timeout10"
	#assert "/timeout5/" "3" "false"
	#assert "/timeout10/" "8" "false"

	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

printf "|------------------ ${TEST_NAME} start ------------------|\n"

runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
#runTest "keepalive_timeout_select.conf" "select" # select
#runTest "keepalive_timeout_poll.conf" "poll" # poll

clean

printLog
