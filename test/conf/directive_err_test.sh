#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"
if [ -e $webserv_path ]
then
	printf "|------------------ webserv test start ------------------|\n\n"
else
	echo "${webserv_path}: command not found"
	echo "run \"make\" first to test"
	exit 1
fi

g_test_directive=""
g_test_index=0
g_test_passed=0
g_test_failed=0

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")
	local	conf_path=$1
	printf "[  ${g_test_directive} test $g_test_index  ]\n${conf_path}: "
	local	actual=$(${webserv_path} $conf_path 2>&1)
	local	expect=$2

	if [ "$actual" = "$expect" ]
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
	printf "|------------------ webserv test results ------------------|\n"
	printf "[========]    ${g_test_index} tests ran\n"
	printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
	printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

readonly err_start_with="webserv: [emerg]"


# allow
g_test_directive="allow"
g_test_index=0
conf_path="test/conf/conf_files/directive_error/allow_ipv4_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"127.a.0.0\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/allow_ipv4_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"127.256.0.0\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/allow_ipv6_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"FF:4E:CC:50:2B:A0:9A:0Q\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/allow_ipv6_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"9A\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/allow_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/allow_ipv4_subnetmask_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"192.168.0.0/33\" ${err_path}:8"

conf_path="test/conf/conf_files/directive_error/allow_ipv6_subnetmask_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"A0:48:9F:C3:FF:BD:12:FF/-1\" ${err_path}:8"

# autoindex
g_test_directive="autoindex"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/autoindex_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"onf\" in \"autoindex\" directive, it must be \"on\" or \"off\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/autoindex_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"\" in \"autoindex\" directive, it must be \"on\" or \"off\" ${err_path}:6"

# client_max_body_size
g_test_directive="client_max_body_size"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/clientMaxBodySize_unit_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"client_max_body_size\" directive invalid value ${err_path}:5"


conf_path="test/conf/conf_files/directive_error/clientMaxBodySize_over_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"client_max_body_size\" directive invalid value ${err_path}:5"

conf_path="test/conf/conf_files/directive_error/clientMaxBodySize_minus_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"client_max_body_size\" directive invalid value ${err_path}:5"

# deny
g_test_directive="deny"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/deny_ipv4_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"127.a.0.0\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/deny_ipv4_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"127.256.0.0\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/deny_ipv6_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"FF:4E:CC:50:2B:A0:9A:0Q\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/deny_ipv6_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"FF\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/deny_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/deny_ipv4_subnetmask_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"192.168.0.0/-1\" ${err_path}:8"

conf_path="test/conf/conf_files/directive_error/deny_ipv6_subnetmask_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"A0:48:9F:C3:FF:BD:12:FF/129\" ${err_path}:8"

# error_page
g_test_directive="error_page"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/errorPage_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/errorPage_invalid_code_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"300a\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/errorPage_invalid_code_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} value \"600\" must be between 300 and 599 ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/errorPage_invalid_option_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"=300err\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/errorPage_invalid_option_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"=9223372036854775808\" ${err_path}:6"

# index
g_test_directive="index"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/index_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} index \"\" in \"index\" directive is invalid ${err_path}:6"

# keepalive_timeout
g_test_directive="keepalive_timeout"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/keepaliveTimeout_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"keepalive_timeout\" directive invalid value ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/keepaliveTimeout_invalid_unit_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"keepalive_timeout\" directive invalid value ${err_path}:6"

# listen
g_test_directive="listen"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/listen_empty_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} host not found in \"\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_too_many_args_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid parameter \"0.0.0.0:80:8080\" ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_only_commma_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid port in \":\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_address_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} no host in \":80\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_port_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid port in \"0.0.0.0:\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_port_over_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid port in \"0.0.0.0:65536\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_not_address_and_port_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid port in \"-1\" of the \"listen\" directive ${err_path}:6"

conf_path="test/conf/conf_files/directive_error/listen_duplicate.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} a duplicate default server for 127.0.0.1:9090 ${err_path}:14"

# return
g_test_directive="return"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/return_invalid_code_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid return code \"a300\" ${err_path}:7"

conf_path="test/conf/conf_files/directive_error/return_over_code_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid return code \"1000\" ${err_path}:7"

conf_path="test/conf/conf_files/directive_error/return_not_httpurl.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid return code \"\" ${err_path}:7"

conf_path="test/conf/conf_files/directive_error/return_not_httpsurl.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid return code \"https:/\" ${err_path}:7"

# send_timeout
g_test_directive="send_timeout"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/sendTimeout_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"send_timeout\" directive invalid value ${err_path}:5"

conf_path="test/conf/conf_files/directive_error/sendTimeout_invalid_unit_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"send_timeout\" directive invalid value ${err_path}:5"

# try_files
g_test_directive="try_files"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/tryFiles_invalid_code_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid code \"=1000\" ${err_path}:7"

conf_path="test/conf/conf_files/directive_error/tryFiles_invalid_code_err2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid code \"=a\" ${err_path}:7"

# use
g_test_directive="use"
g_test_index=0

os=$(uname -s)

if [ "$os" = "Linux" ]; then
	printf "\033[32m====================Runnning on Linux====================\033[0m\n\n"

	conf_path="test/conf/conf_files/directive_error/use_linux_invalid_event.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"kqueue\" ${err_path}:2"

	conf_path="test/conf/conf_files/directive_error/use_linux_invalid_event2.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"EPOLL\" ${err_path}:2"

elif [ "$os" = "Darwin" ]; then
	printf "\033[32m====================Runnning on macOS====================\033[0m\n\n"

	conf_path="test/conf/conf_files/directive_error/use_mac_invalid_event.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"epoll\" ${err_path}:2"

	conf_path="test/conf/conf_files/directive_error/use_mac_invalid_event2.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"POLL\" ${err_path}:2"

else
	printf "\033[32m====================Runnning on Unknown OS====================\033[0m\n\n"

	conf_path="test/conf/conf_files/directive_error/use_unknownOS_invalid_event.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"epoll\" ${err_path}:2"

	conf_path="test/conf/conf_files/directive_error/use_unknownOS_invalid_event2.conf"
	err_path="in $(readlink -f $conf_path)"
	assert $conf_path "${err_start_with} invalid event type \"kqueue\" ${err_path}:2"

fi

	printf "\033[32m==========================================================\033[0m\n\n"

# userid
g_test_directive="userid"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/userid_value_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid value \"onf\" in \"userid\" directive, it must be \"on\" or \"off\" ${err_path}:5"

# userid_expires
g_test_directive="userid_expires"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/useridExpires_time_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"userid_expires\" directive invalid value ${err_path}:5"

# userid_service
g_test_directive="userid_service"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/useridService_num_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"userid_service\" directive invalid value ${err_path}:5"

# worker_connections
g_test_directive="worker_conections"
g_test_index=0

conf_path="test/conf/conf_files/directive_error/workerConnections_invalid_num_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid number \"10a0\" ${err_path}:2"

conf_path="test/conf/conf_files/directive_error/workerConnections_over_longmax_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} invalid number \"9223372036854775808\" ${err_path}:2"

conf_path="test/conf/conf_files/directive_error/workerConnections_not_enough_err.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"1\" worker_connections are not enough for 1 listening sockets"

printLog
