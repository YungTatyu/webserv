#! /bin/bash
readonly DIR_PATH=$(dirname "$0")
readonly SCRIPT_NAME="test_static_res.js"

function err {
	printf "$@\n" >&2
}

function run_test {
	local virtual_client="$1" # リクエストを送るclientの数
	local duration="$2"       # test実施時間
	k6 run "${DIR_PATH}/${SCRIPT_NAME}" --vus ${virtual_client} --duration ${duration}
}

function main {
	if [[ -z "$1" ]] || [[ -z "$2" ]]; then
		err "Usage: $0 <vus> <duration>"
		return 1
	fi
	run_test "$@"
	return 0
}

main "$@"
