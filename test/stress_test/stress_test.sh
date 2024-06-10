#! /bin/bash
readonly DIR_PATH=$(dirname "$0")
readonly ROOT="${DIR_PATH}/../.."
readonly WEBSERV_PATH="${ROOT}/webserv"

function err {
	printf "$@\n" >&2
}

function run_test {
	local script="$1"
	local virtual_client="$2" # リクエストを送るclientの数
	local duration="$3"       # test実施時間
	k6 run "${DIR_PATH}/${script}" --vus ${virtual_client} --duration ${duration}
}

function main {
	if [[ -z "$1" ]] || [[ -z "$2" ]] || [[ -z "$3" ]]; then
		err "Usage: $0 <script> <vus> <duration>"
		return 1
	fi
	run_test "$@"
	return 0
}

main "$@"
