#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"
readonly test_name="Timeout Test"

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
	local	request=localhost:4242${uri}
	printf "[  test$g_test_index  ]\n${request}: "

	# http request作成
	request=$(cat <<EOT
GET ${uri} HTTP/1.1
Host: _

EOT
)

	# telnetセッション開始
	exec 3<>/dev/tcp/localhost/4242
	if [ $? -ne 0 ]; then
		echo "Failed to connect to $server_host:$server_port"
		exit 1
	fi

	# timeout時間を計測
	local	start_time=$(date +%s%N)
	echo "$request" >&3
	while read -r line <&3; do
		:  # 何もしない
	done
	local	end_time=$(date +%s%N)

	# telnetセッションを終了
	exec 3>&-

	# keepalive timeout を計算
	local	actual_nanosec=$((end_time - start_time))
	local	actual_sec=$((actual_nanosec / 1000000000))
	local	actual_millisec=$((actual_nanosec / 1000000 % 1000))

	if [ "$actual_sec" -eq "$expect_sec" ]; then
		printf "\033[32mpassed.\n\033[0mServer closed the connection after $actual_sec.$actual_millisec seconds.\n\n"
		g_test_passed=$(bc <<< "$g_test_passed + 1")
	else
		printf "\033[31mfailed.\n\033[0mServer did not timeout after $expect_sec seconds.\n"
		printf "expected:${expect_sec}\n"
		printf "actual  :${actual_sec}.${actual_millisec}\n\n"
		g_test_failed=$(bc <<< "$g_test_failed + 1")
	fi
}

function	printLog {
	printf "\n|------------------ $test_name results ------------------|\n"
	printf "[========]    ${g_total_test} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

function	runTest {
	local	root="test/integration/test_files/TimeoutTestFiles"
	local	conf=$1
	local	server_name=$2
	runServer "${root}/${conf}"

	printf "\n\033[32m<<< ${server_name} server test >>>\033[0m\n"
	assert "/" "0"
	assert "/timeout5/" "5"
	assert "/timeout10/" "10"

	g_test_index=0
	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
#runTest "keepalive_timeout_select.conf" "select" # select
#runTest "keepalive_timeout_poll.conf" "poll" # poll

printLog
