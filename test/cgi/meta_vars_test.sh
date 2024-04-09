#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"
readonly test_name="meta vars test"

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
	$webserv_path $1 > /dev/null 2>&1 &
	webserv_pid=$!
}

# responseのstatusをテスト
function	assert {
	g_total_test=$(bc <<< "$g_total_test + 1")
	g_test_index=$(bc <<< "$g_test_index + 1")

	local	uri=$1
	local	request="localhost:4242/${uri}"
	printf "[  test$g_test_index  ]\n${request}: "

	# responseのtimeoutを1秒に設定 --max-time
	local	actual=$(curl -H "host: test" -H "content-type: text" -d "body message" $request --max-time 1 2>/dev/null)
	local	expect=$2
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
	printf "\n|------------------ $test_name results ------------------|\n"
	printf "[========]    ${g_total_test} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"

	if [ $g_test_failed -ne 0 ]
	then
		exit 1
	fi
}

runServer "${root}/all_meta_vars.conf"

root="test/cgi/cgi_files/executor"
uri="${root}/all_meta_vars.py"

meta_vars=(
	"AUTH_TYPE=\n"
	"CONTENT_LENGTH=12\n"
	"CONTENT_TYPE=text\n"
	"GATEWAY_INTERFACE=CGI/1.1\n"
	"PATH_INFO=\n"
	"PATH_TRANSLATED=\n"
	"QUERY_STRING=key=value&test=vtest\n"
	"REMOTE_ADDR=127.0.0.1\n"
	"REMOTE_HOST=127.0.0.1\n"
	"REQUEST_METHOD=GET\n"
	"SCRIPT_NAME=${uri}\n"
	"SERVER_NAME=test\n"
	"SERVER_PORT=4242\n"
	"SERVER_PROTOCOL=HTTP/1.1\n"
	"SERVER_SOFTWARE=webserv/1.0\n"
)

expect=$(printf "%s" "${meta_vars[@]}")

assert "${root}/${uri}?key=value&test=vtest" "${expect}"

# サーバープロセスを終了
kill $webserv_pid > /dev/null 2>&1

printLog
