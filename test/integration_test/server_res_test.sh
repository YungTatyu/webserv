#!/bin/bash

# init
readonly SCRIPT_DIR_PATH=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR_PATH}/../../webserv"
readonly SV_RES_DYNAMIC_PATH="${SCRIPT_DIR_PATH}/test_files/server_res_test/dynamic"
readonly TEST_NAME="server response test"

g_total_test=0
g_test_index=0
g_test_passed=0
g_test_failed=0

function	init {
	if [ -e $WEBSERV_PATH ]; then
		printf "|------------------ $TEST_NAME start ------------------|\n"
	else
		printErr "${WEBSERV_PATH}: command not found"
		printErr "run \"make\" first to test"
		exit 1
	fi
	make -C ${SV_RES_DYNAMIC_PATH} > /dev/null
}

function	printErr {
	printf "${*}\n" >&2
}

function	printLog {
	printf "\n|------------------ $TEST_NAME results ------------------|\n"
	printf "[========]    ${g_total_test} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

function	runServer {
	$WEBSERV_PATH $1 > /dev/null 2>&1 &
	# エラー出力する場合
	# $WEBSERV_PATH $1 > /dev/null &
	webserv_pid=$!
}

# responseのstatusをテスト
function	assert {
	((++g_total_test))
	((++g_test_index))

	local	uri=$1
	local	request="localhost:4242/${uri}"
	printf "[  test$g_test_index  ]\n${request}: "

	# responseのtimeoutを1秒に設定 --max-time
	local	actual=$(curl -s -o /dev/null -w "%{http_code}" ${request} --max-time 1)
	local	expect=$2
	if [ "${actual}" == "${expect}" ]; then
		printf "\033[32mpassed\033[0m\n\n"
		((++g_test_passed))
	else
		printErr "\033[31mfailed\n\033[0m"
		printErr "expected:${expect}---"
		printErr "actual  :${actual}---\n"
		((++g_test_failed))
	fi
}

function	runTest {
	local	root="test/integration_test/test_files/server_res_test"
	local	conf=$1
	local	server_name=$2
	g_test_index=0

	runServer "${root}/${conf}"
	printf "\n\033[32m<<< ${server_name} server test >>>\033[0m\n"

	sleep 1
	# 以下にテストを追加
	assert "${root}/static/index.html" "200"
	assert "${root}/nonexist" "404"
	assert "${root}/dynamic/document_response.py" "200"
	assert "${root}/dynamic/local_redirect_res.py" "302"
	assert "${root}/dynamic/client_redirect_res.cgi" "302"
	assert "${root}/dynamic/client_redirect_res_doc.cgi" "302"
	assert "${root}/dynamic/body_res.py" "200"

	# サーバープロセスを終了
	kill $webserv_pid > /dev/null 2>&1
}

function	main {
	init
	runTest "server_res_test.conf" "kqueue or epoll" # kqueue or epoll
	runTest "server_res_test_select.conf" "select" # select
	runTest "server_res_test_poll.conf" "poll" # poll

	printLog

	make fclean -C ${SV_RES_DYNAMIC_PATH} > /dev/null

	if [ $g_test_failed -ne 0 ]; then
		return 1
	fi

	return 0
}

main "$@"