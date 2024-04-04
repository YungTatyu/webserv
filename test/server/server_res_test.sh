#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"
readonly test_name="server response test"

if [ -e $webserv_path ]
then
	printf "|------------------ $test_name start ------------------|\n\n"
else
	echo "${webserv_path}: command not found"
	echo "run \"make\" first to test"
	exit 1
fi

g_test_index=0
g_test_passed=0
g_test_failed=0

function	runServer {
	$webserv_path $1 > /dev/null 2>&1 &
	# エラー出力する場合
	# $webserv_path $1 > /dev/null &
	readonly WEBSERV_PID=$!
}

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")

	local	uri=$1;
	local	request=localhost:4242/${uri}	
	printf "[  test$g_test_index  ]\n${request}: "

	local	actual=$(curl -s -o /dev/null -w "%{http_code}" $request --max-time 1)
	local	expect=$2;
	if [ "$actual" == "$expect" ]
	then
		printf "\033[32mpassed\033[0m\n\n"
		g_test_passed=$(bc <<< "$g_test_passed + 1")
	else
		printf "\033[31mfailed\n\033[0m"
		printf "expected:${expect}---\n"
		printf "actual  :${actual}---\n\n"
		g_test_failed=$(bc <<< "$g_test_failed + 1")
	fi
}

function	printLog {
	printf "|------------------ $test_name results ------------------|\n"
	printf "[========]    ${g_test_index} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

function	runTest {
	local	root="test/server/test_files/server_res_test"
	local	conf=$1
	runServer "${root}/${conf}"

	assert "${root}/static/index.html" "200"
	assert "${root}/nonexist" "404"

	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

runTest "server_res_test.conf"

printLog