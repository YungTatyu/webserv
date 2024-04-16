#!/bin/bash

# init
readonly SCRIPT_DIR_PATH=$(dirname "$0")
readonly WEBSERV_PATH="${SCRIPT_DIR_PATH}/../../webserv"
readonly TEST_NAME="meta vars test"

readonly WHITE="\033[0m"
readonly GREEN="\033[32m"
readonly RED="\033[31m"

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

	local	request="$1"
	local	method="$2"
	local	host="$3"
	local	content_type="$4"
	local	body="$5"
	local	expect="$6"

	printf "[  test${g_test_index}  ]\n${request}: "
	# responseのtimeoutを1秒に設定 --max-time
	local	actual=$(curl -X ${method}  -H "host: ${host}" -H "content-type: ${content_type}" -d "${body}" ${request} --max-time 1 )
	
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

function	runTest {
	local root="test/cgi/cgi_files/executor"
	local conf=$1
	local body=$2
	local content_type=$3
	local query_string=$4
	local method=$5
	local host=$6
	local port=$7
	local uri=$8
	
	local meta_vars=(
		"AUTH_TYPE=\n"
		"CONTENT_LENGTH=$(printf ${body} | wc -c)\n"
		"CONTENT_TYPE=${content_type}\n"
		"GATEWAY_INTERFACE=CGI/1.1\n"
		"PATH_INFO=\n"
		"PATH_TRANSLATED=\n"
		"QUERY_STRING=${query_string}\n"
		"REMOTE_ADDR=127.0.0.1\n"
		"REMOTE_HOST=127.0.0.1\n"
		"REQUEST_METHOD=${method}\n"
		"SCRIPT_NAME=${root}/${uri}\n"
		"SERVER_NAME=${host}\n"
		"SERVER_PORT=${port}\n"
		"SERVER_PROTOCOL=HTTP/1.1\n"
		"SERVER_SOFTWARE=webserv/1.0\n"
	)
	local expect=$(printf "%s" "${meta_vars[@]}")

	runServer "${root}/${conf}"

	assert "localhost:${port}/${root}/${uri}?${query_string}" 
					${method} ${host} ${content_type} ${body} "${expect}"

	# サーバープロセスを終了
	kill ${webserv_pid} > /dev/null 2>&1
}

function	main {

	init
	runTest "all_meta_vars_poll.conf" "this is body" "content-type text" "a=a&b=b&c=c" "GET" "hostname" "4242" "all_meta_vars.py"

	printLog

	if [ ${g_test_failed} -ne 0 ]; then
		return 1
	fi

	return 0
}

main "$@"
