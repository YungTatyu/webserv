SRCS_DIR="src"
TEST_DIR="test"

function main {
  find ${SRCS_DIR} ${TEST_DIR} -name "*.cpp" -o -name "*.hpp" -o -name "*.c" | xargs clang-format -i
  black ${TEST_DIR}
  shfmt -w -l -i 2 ${TEST_DIR}
  return 0
}

main "$@"
