#!/bin/bash

# init
readonly script_dir_path=$(dirname "$0")
readonly webserv_path="${script_dir_path}/../../webserv"

function init {
  if [ -e $webserv_path ]; then
    printf "|------------------ webserv test start ------------------|\n\n"
  else
    printErr "${webserv_path}: command not found"
    printErr "run \"make\" first to test"
    exit 1
  fi
}

g_test_index=0
g_test_passed=0
g_test_failed=0

function printLog {
  printf "|------------------ webserv test results ------------------|\n"
  printf "[========]    ${g_test_index} tests ran\n"
  printf "[ \033[32mPASSED\033[0m ]    ${g_test_passed} tests\n"
  printf "[ \033[31mFAILED\033[0m ]    ${g_test_failed} tests\n"
}

function printErr {
  printf "${*}\n" >&2
}

function assert {
  ((++g_test_index))
  local conf_path=$1
  printf "[  test$g_test_index  ]\n${conf_path}: "
  local actual=$(${webserv_path} $conf_path 2>&1)
  local expect=$2

  if [ "$actual" = "$expect" ]; then
    printf "\033[32mpassed\033[0m\n\n"
    g_test_passed=$(bc <<<"$g_test_passed + 1")
  else
    printErr "\033[31mfailed\n\033[0m"
    printErr "expected:\"${expect}\"\n"
    printErr "actual  :\"${actual}\"\n\n"
    ((++g_test_failed))
  fi
}

function main {
  init
  readonly err_start_with="webserv: [emerg]"

  # test unexpect token
  conf_path="test/conf/conf_files/error/double_quote_not_closed.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} unexpected end of file, expecting \";\" or \"}\" ${err_path}:2"

  conf_path="test/conf/conf_files/error/unexpect_open_curly_brace1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} unexpected \"{\" ${err_path}:3"

  conf_path="test/conf/conf_files/error/unexpect_close_curly_brace2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} unexpected \"}\" ${err_path}:6"

  conf_path="test/conf/conf_files/error/unexpect_semicolon1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} unexpected \";\" ${err_path}:3"

  # test unknown directive
  conf_path="test/conf/conf_files/error/unknown_directive1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} unknown directive \";\" ${err_path}:3"

  conf_path="test/conf/conf_files/error/unknown_directive2.conf"
  err_path="in $(readlink -f $conf_path)"
  # 改行がtestで一致しない
  # assert $conf_path "${err_start_with} unknown directive \"\n\n\nindex\" ${err_path}:4"
  assert $conf_path "${err_start_with} unknown directive \"


index\" ${err_path}:4"

  # test invalid directive in context levle
  conf_path="test/conf/conf_files/error/invalid_directive1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"index\" directive is not allowed here ${err_path}:1"

  conf_path="test/conf/conf_files/error/invalid_directive_server1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"server\" directive is not allowed here ${err_path}:2"

  conf_path="test/conf/conf_files/error/invalid_directive_server2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"server\" directive is not allowed here ${err_path}:1"

  conf_path="test/conf/conf_files/error/invalid_directive_server3.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"server\" directive is not allowed here ${err_path}:16"

  conf_path="test/conf/conf_files/error/invalid_directive_server4.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"server\" directive is not allowed here ${err_path}:12"

  conf_path="test/conf/conf_files/error/invalid_directive_server6.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"server\" directive is not allowed here ${err_path}:6"

  conf_path="test/conf/conf_files/error/invalid_directive_http1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"http\" directive is not allowed here ${err_path}:12"

  conf_path="test/conf/conf_files/error/invalid_directive_http2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"http\" directive is not allowed here ${err_path}:5"

  conf_path="test/conf/conf_files/error/invalid_directive_http3.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"http\" directive is not allowed here ${err_path}:6"

  conf_path="test/conf/conf_files/error/invalid_directive_http4.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"http\" directive is not allowed here ${err_path}:15"

  conf_path="test/conf/conf_files/error/invalid_directive_location1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"location\" directive is not allowed here ${err_path}:2"

  conf_path="test/conf/conf_files/error/invalid_directive_location2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"location\" directive is not allowed here ${err_path}:4"

  conf_path="test/conf/conf_files/error/invalid_directive_location3.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"location\" directive is not allowed here ${err_path}:6"

  conf_path="test/conf/conf_files/error/invalid_directive_location4.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"location\" directive is not allowed here ${err_path}:7"

  conf_path="test/conf/conf_files/error/invalid_directive_location5.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"location\" directive is not allowed here ${err_path}:17"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:2"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:9"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except3.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:7"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except3.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:7"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except4.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:16"

  conf_path="test/conf/conf_files/error/invalid_directive_limit_except4.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is not allowed here ${err_path}:16"

  # test invalid args num
  conf_path="test/conf/conf_files/error/invalid_args_http1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"http\" directive ${err_path}:2"

  conf_path="test/conf/conf_files/error/invalid_args_http2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"http\" directive ${err_path}:2"

  conf_path="test/conf/conf_files/error/invalid_args_events1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"events\" directive ${err_path}:14"

  conf_path="test/conf/conf_files/error/invalid_args_events2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"events\" directive ${err_path}:14"

  conf_path="test/conf/conf_files/error/invalid_args_server1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"server\" directive ${err_path}:3"

  conf_path="test/conf/conf_files/error/invalid_args_server2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"server\" directive ${err_path}:3"

  conf_path="test/conf/conf_files/error/invalid_args_location1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"location\" directive ${err_path}:4"

  conf_path="test/conf/conf_files/error/invalid_args_limit_except1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid number of arguments in \"limit_except\" directive ${err_path}:5"

  # test no evnents section
  conf_path="test/conf/conf_files/error/only_comments.conf"
  assert $conf_path "${err_start_with} no \"events\" section in configuration"

  conf_path="test/conf/conf_files/error/empty.conf"
  assert $conf_path "${err_start_with} no \"events\" section in configuration"

  conf_path="test/conf/conf_files/error/newline.conf"
  assert $conf_path "${err_start_with} no \"events\" section in configuration"

  # test http
  conf_path="test/conf/conf_files/error/http_duplicate.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"http\" directive is duplicate ${err_path}:13"

  # test location
  conf_path="test/conf/conf_files/error/location_duplicate1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} duplicate location \"path\" ${err_path}:8"

  conf_path="test/conf/conf_files/error/location_duplicate2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} duplicate location \"path\" ${err_path}:8"

  # test limit_except
  conf_path="test/conf/conf_files/error/limit_except_duplicate1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"limit_except\" directive is duplicate ${err_path}:10"

  conf_path="test/conf/conf_files/error/limit_except_invalid_method1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid method \"none\" ${err_path}:5"

  conf_path="test/conf/conf_files/error/limit_except_invalid_method2.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} invalid method \"test\" ${err_path}:5"

  # test context
  conf_path="test/conf/conf_files/error/invalid_context1.conf"
  err_path="in $(readlink -f $conf_path)"
  assert $conf_path "${err_start_with} \"index\" directive is not allowed here ${err_path}:2"

  printLog

  if [ ${g_test_failed} -ne 0 ]; then
    return 1
  fi
  return 0
}

main "$@"