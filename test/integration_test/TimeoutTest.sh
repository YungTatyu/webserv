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

	# telnetセッション開始
	local start_time
	local end_time
	local	actual_nanosec
	local	actual_sec
	local	actual_millisec


	case "$OS" in
	Linux)
		start_time=$(date +%s%N)
		#echo $request | nc "$host" "$port" > /dev/null
		echo "$request" | nc "$host" "$port"
		end_time=$(date +%s%N)
		actual_nanosec=$((end_time - start_time))
		actual_sec=$((actual_nanosec / 1000000000))
		actual_millisec=$((actual_nanosec / 1000000 % 1000))
		#exec 3<>/dev/tcp/$host/$port
		#if [ $? -ne 0 ]; then
		#	printf "\033[31mfailed to connect to $host:$port\n\033[0m" >&2
		#	exit 1
		#fi

		## timeout時間を計測
		#start_time=$(date +%s%N)
		#echo "$request" >&3
		#while read -r line <&3; do #	:  # 何もしない
		#done
		#end_time=$(date +%s%N)
		# telnetセッションを終了
		# exec 3>&-
		;;
#
		Darwin)
		start_time=$(date +%s)
		#(printf "GET / HTTP/1.1\nhost:tt\n\n"; sleep 5) | telnet localhost 8000 &

		(printf "$request"; sleep 15) | telnet $host $port &
		sleep $(bc <<< "$expect_sec + 1") | ps | grep sleep | grep -v grep
		# sleep $expect_sec | ps | grep sleep | grep -v grep
		ps | grep telnet | grep -v grep
		local exitstatus=$?
		echo exitstatus:  $exitstatus
		if [ "$exitstatus" == "1" ]
		then
			kill $(ps | grep sleep | grep -v grep | awk '{ print $1}')
			echo expected
			printf "\033[32mpassed.\n\033[0mServer closed the connection\n\n"
		else
			printf "\033[31mfailed.\n\033[0mServer did not timeout\n"
			kill $(ps | grep telnet | grep -v grep | awk '{ print $1}')
		fi
		echo $exitstatus

		#(printf "$request"; sleep 5) | telnet $host $port
		#(echo "$request" | nc -w 10 $host $port)

		#(
		#	echo "$request";
		#	#cat <&0;
		#	sleep 1
		#) | nc $host $port
		#nc_pid =$$
		#wait $nc_pid

		end_time=$(date +%s)
		actual_sec=$((end_time - start_time))
		actual_millisec=0
		;;

	*)
		printf "\033[31mNot supported os: $OS\n\033[0m" >&2
		exit 1
		;;
	esac

	# keepalive timeout を計算
	# 	if [ "$actual_sec" -eq "$expect_sec" ]; then
	# 	printf "\033[32mpassed.\n\033[0mServer closed the connection after $actual_sec.$actual_millisec seconds.\n\n"
	# 	g_test_passed=$(bc <<< "$g_test_passed + 1")
	# else
	# 	printf "\033[31mfailed.\n\033[0mServer did not timeout after $expect_sec seconds.\n"
	# 	printf "expected:${expect_sec}\n"
	# 	printf "actual  :${actual_sec}.${actual_millisec}\n\n"
	# 	g_test_failed=$(bc <<< "$g_test_failed + 1")
	# fi
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
	assert "/" "0"
	assert "/timeout5/" "3"
	assert "/timeout10/" "8"

	g_test_index=0
	# サーバープロセスを終了
	kill $WEBSERV_PID > /dev/null 2>&1
}

runTest "keepalive_timeout.conf" "kqueue or epoll" # kqueue or epoll
#runTest "keepalive_timeout_select.conf" "select" # select
#runTest "keepalive_timeout_poll.conf" "poll" # poll

printLog
