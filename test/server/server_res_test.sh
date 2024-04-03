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
	# エラー出力する場合
	# ./webserv $1 > /dev/null 2>&1 &
	./webserv $1 > /dev/null &
	readonly WEBSERV_PID=$!
}

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")
	
	printf "[  test$g_test_index  ]: "

	local	uri=$1;
	local	actual=$(curl -s -o /dev/null -w "%{http_code}" localhost:4242/${uri} --max-time 1)
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

readonly root="test/server/test_files/server_res_test"
runServer "${root}/server_res_test.conf"

assert "${root}/static/" "200"

# サーバープロセスを終了
kill $WEBSERV_PID

printLog

