#!/bin/bash

# init
readonly SCRIPT_DIR_PATH=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR_PATH}/../../webserv"
readonly TEST_NAME="meta vars test"

g_total_test=0
g_test_index=0
g_test_passed=0
g_test_failed=0

function	init {
	if [ -e ${WEBSERV_PATH} ]; then
		printf "|------------------ ${TEST_NAME} start ------------------|\n"
	else
		printErr "${WEBSERV_PATH}: command not found"
		printErr "run \"make\" first to test"
		exit 1
	fi
	trap signalHandler HUP INT QUIT ABRT KILL TERM
}

# signal受信時に実行
function	signalHandler {
	printErr "\n${TEST_NAME} interrupted: Signal received."
	kill ${webserv_pid}
	exit 1
}

function	printErr {
	printf "${*}\n" >&2
}

function	printLog {
	printf "\n|------------------ ${TEST_NAME} results ------------------|\n"
	printf "[========]    ${g_test_cnt} tests ran\n"
	printf "[ ${GREEN}PASSED${WHITE} ]    ${g_test_passed} tests\n"
	printf "[ ${RED}FAILED${WHITE} ]    ${g_test_failed} tests\n"
}

function	runServer {
	# ${WEBSERV_PATH} $1 > /dev/null 2>&1 &
	# エラー出力する場合
	${WEBSERV_PATH} $1 > /dev/null &
	webserv_pid=$!
	sleep 1
}

function	assert {
	((++g_test_cnt))
	((++g_test_index))

	local	uri=$1
	local	request="localhost:4242/${uri}"
	printf "[  test${g_test_index}  ]\n${request}: "

	# responseのtimeoutを1秒に設定 --max-time
	# local	actual=$(curl -H "host: test" -H "content-type: text" ${request} --max-time 1 2>/dev/null)
	local	actual=$(curl ${request})
	# local	actual=$(curl -H "host: test" -H "content-type: text" ${request}  2>/dev/null)
	local	expect=$2
	if [ "${actual}" == "${expect}" ]; then
		printf "${GREEN}passed${WHITE}\n\n"
		((++g_test_passed))
	else
		printErr "${RED}failed\n${WHITE}"
		printErr "expected: \"${expect}\""
		printErr "actual  : \"${actual}\"\n"
		((++g_test_failed))
	fi
}

function	printLog {
	printf "\n|------------------ ${TEST_NAME} results ------------------|\n"
	printf "[========]    ${g_test_cnt} tests ran\n"
	printf "[ ${GREEN}PASSED${WHITE} ]    ${g_test_passed} tests\n"
	printf "[ ${RED}FAILED${WHITE} ]    ${g_test_failed} tests\n"
}

function	main {

	local root="test/cgi/cgi_files/executor"
	local uri="${root}/all_meta_vars.py"
	local meta_vars=(
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
	local expect=$(printf "%s" "${meta_vars[@]}")

	init
	runServer "${root}/all_meta_vars.conf"

	# assert "${root}/${uri}" "${expect}"
	assert "${uri}?key=value&test=vtest" "${expect}"

	# サーバープロセスを終了
	kill ${webserv_pid} > /dev/null 2>&1
	printLog

	if [ ${g_test_failed} -ne 0 ]; then
		return 1
	fi

	return 0
}

main "$@"
