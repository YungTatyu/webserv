#!/bin/bash

# init
readonly script_dir=$(dirname "$0")
readonly webserb_path="${script_dir}/../../webserv"
if [ -e $webserb_path ]
then
	printf "|------------------ webserv test start ------------------|\n\n"
else
	echo "${webserb_path}: command not found"
	echo "run \"make\" first to test"
	exit 1
fi

g_test_index=0

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")
	local	conf_path=$1
	printf "test$g_test_index $conf_path: "
	local	actual=$(${webserb_path} $conf_path 2>&1)
	local	expect=$2

	if [ "$actual" = "$expect" ]
	then
		printf "\033[32mpassed\033[0m\n\n"
	else
		printf "\033[31mfailed\n\033[0m"
		printf "expected:${expect}---\n"
		printf "actual  :${actual}---\n\n"
	fi
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
