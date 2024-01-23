g_test_index=0

function	assert {
	g_test_index=$(bc <<< "$g_test_index + 1")
	printf "test$g_test_index $1: "
	local	actual=$(../../webserv $1 2>&1)
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

conf_path="./conf_files/error/double_quote_not_closed.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \"\"\" in $(readlink -f $conf_path):1\n"

conf_path="./conf_files/error/double_quote_not_closed.conf"
err_path="in $(readlink -f $conf_path)"
assert $conf_path "${err_start_with} unexpected \"\"\" ${err_path}:1\n"
