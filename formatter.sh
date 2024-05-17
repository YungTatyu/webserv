#! /bin/bash

SRCS_DIR="src"

function main {
  find ${SRCS_DIR} ${TEST_DIR} -name "*.cpp" -o -name "*.hpp" -o -name "*.c" |
    xargs clang-format -i || return 1
  black . || return 1
  shfmt -w -l -i 2 . || return 1
  return 0
}

main "$@"
