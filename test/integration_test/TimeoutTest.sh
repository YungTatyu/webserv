#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"
readonly test_name="Timeout Test"
readonly OS=$(uname -s)

if [ -e $webserv_path ]
then
	printf "|------------------ $test_name start ------------------|\n"
else
	echo "${webserv_path}: command not found"
	echo "run \"make\" first to test"
	exit 1
fi

g_total_test=0
g_test_index=0
g_test_passed=0
g_test_failed=0

function	runServer {
	# sleep 1
	$webserv_path $1 > /dev/null 2>&1 &
	# エラー出力する場合
	# $webserv_path $1 > /dev/null &
	WEBSERV_PID=$!
}

function	assert {
	g_total_test=$(bc <<< "$g_total_test + 1")
	g_test_index=$(bc <<< "$g_test_index + 1")

	local	uri=$1;
	local	expect_sec=$2
	local	expect_result=$3
	local	scheme="http"
	local	host="127.0.0.1"
	local	port="4242"
	local	url=$scheme://$host:$port${uri}
	printf "[  test$g_test_index  ]\n${url}: "

	# http request作成
	local	request=$(cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
)

	(printf "$request"; sleep $(bc <<< "$expect_sec + 5")) | telnet $host $port > /dev/null 2>&1 &

	# telnetがタイムアウトして1秒以内の誤差であればTRUE
	sleep $(bc <<< "$expect_sec + 1") | ps | grep sleep | grep -v grep > /dev/null
	ps | grep telnet | grep -v grep > /dev/null
	local exitstatus=$?

	if [ "$exitstatus" == "1" ]; then # telnetが正常にタイムアウト
		kill $(ps | grep sleep | grep -v grep | awk '{ print $1}') > /dev/null
		if [ "$expect_result" = "true" ]; then
			printf "\033[32mpassed.\n\033[0mServer closed the connection\n\n"
			g_test_passed=$(bc <<< "$g_test_passed + 1")
		else
			printf "\033[31mfailed.\n\033[0mServer did not timeout\n"
			g_test_failed=$(bc <<< "$g_test_failed + 1")
		fi
	else # telnetが正常にタイムアウトする前にsleepが終了
		if [ "$expect_result" = "true" ]; then
			printf "\033[31mfailed.\n\033[0mServer did not timeout\n"
			g_test_failed=$(bc <<< "$g_test_failed + 1")
		else
			printf "\033[32mpassed.\n\033[0mServer closed the connection\n\n"
			g_test_passed=$(bc <<< "$g_test_passed + 1")
		fi
		kill $(ps | grep telnet | grep -v grep | awk '{ print $1}') > /dev/null 2>&1
	fi
}

function	printLog {
	printf "\n|------------------ $test_name results ------------------|\n"
	printf "[========]    ${g_total_test} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

function	runTest {
	local	root="test/integration_test/test_files/TimeoutTestFiles"
	local	conf=$1
	local	server_name=$2
	runServer "${root}/${conf}"

	printf "\n\033[32m<<< ${server_name} server test >>>\033[0m\n"
	assert "/" "0" "true"
	assert "/timeout5/" "5" "true"
	assert "/timeout10/" "10" "true"
	assert "/timeout5/" "3" "false"
	assert "/timeout10/" "8" "false"

	g_test_index=0
	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
runTest "keepalive_timeout_select.conf" "select" # select
runTest "keepalive_timeout_poll.conf" "poll" # poll

printLog
