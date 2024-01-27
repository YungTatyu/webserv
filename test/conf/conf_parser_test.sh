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

g_test_index=0
g_test_passed=0
g_test_failed=0

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")
	local	conf_path=$1
	printf "test$g_test_index $conf_path: "
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

# test unexpect token
conf_path="test/conf/conf_files/error/double_quote_not_closed.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected end of file, expecting \";\" or \"}\" ${err_path}:2\n"

conf_path="test/conf/conf_files/error/double_quote_not_closed.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected end of file, expecting \";\" or \"}\" ${err_path}:2\n"

conf_path="test/conf/conf_files/error/unexpect_open_curly_brace1.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \"{\" ${err_path}:3\n"

conf_path="test/conf/conf_files/error/unexpect_open_curly_brace2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \"{\" ${err_path}:6\n"

conf_path="test/conf/conf_files/error/unexpect_close_curly_brace2.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \"}\" ${err_path}:6\n"

conf_path="test/conf/conf_files/error/unecpect_semicolon1.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \";\" ${err_path}:3\n"



# test unknown directive
conf_path="test/conf/conf_files/error/unknown_directive1.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unknown directive \";\" ${err_path}:3\n"




# test invalid directive in context levle
conf_path="test/conf/conf_files/error/invalid_directive1.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} \"index\" directive is not allowed here ${err_path}:1\n"




# test no evnents section
conf_path="test/conf/conf_files/error/only_comments.conf"
assert $conf_path "${err_start_with} no \"events\" section in configuration\n"

conf_path="test/conf/conf_files/error/empty.conf"
assert $conf_path "${err_start_with} no \"events\" section in configuration\n"

conf_path="test/conf/conf_files/error/newline.conf"
assert $conf_path "${err_start_with} no \"events\" section in configuration\n"

# test location

# test limit_excepted

printLog