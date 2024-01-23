function	assert {
	printf "test $1: "
	local	actual=$(../../webserv $1 2>&1)
	local	expect=$2

	if [ "$actual" = "$expect" ]
	then
		printf "\033[32msuccess\033[0m"
	else
		printf "\033[31mfailed\n\033[0m"
		printf "expected:${expect}---\nactual  :${actual}---"
	fi
}

assert ./config expect
