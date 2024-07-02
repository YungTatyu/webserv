cmake_minimum_required(VERSION 3.14)
project(webserv)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}-googletest 
	# src files
	# src/config
	src/config/Lexer.cpp
	src/config/Parser.cpp
	src/config/conf.cpp
	src/config/LogFd.cpp
	# src/cgi
	src/cgi/CgiHandler.cpp
	src/cgi/CgiParser.cpp
	src/cgi/CgiExecutor.cpp
	# src/server
	src/server/ConfigHandler.cpp
	src/server/ConnectionManager.cpp
	src/server/NetworkIOHandler.cpp
	src/server/EventHandler.cpp
	src/server/Timer.cpp
	src/server/TimerTree.cpp
	src/server/EpollActiveEventManager.cpp
	src/server/EpollServer.cpp
	src/server/PollActiveEventManager.cpp
	src/server/PollServer.cpp
	src/server/KqueueActiveEventManager.cpp
	src/server/KqueueServer.cpp
	src/server/SelectActiveEventManager.cpp
	src/server/SelectServer.cpp
	src/server/WebServer.cpp
	src/utils/utils.cpp
	src/utils/syscall_wrapper.cpp
	src/utils/error.cpp
	# src/http
	src/http/HttpRequest.cpp
	src/http/HttpResponse.cpp
	# test files
  test/gtest.cpp
	test/server/TimerTree_test.cpp
  test/server/ConfigHandler/allowRequest/allowRequest.cpp
  test/server/ConfigHandler/allowRequest/limitExcept.cpp
  test/server/ConfigHandler/createTiedServer/createTiedServer.cpp
  test/server/ConfigHandler/searchTimeout/searchKeepaliveTimeout.cpp
  test/server/ConfigHandler/searchTimeout/searchSendTimeout.cpp
  # test/server/ConfigHandler/searchTimeout/searchUseridExpires.cpp
  test/server/ConfigHandler/writeAccessLog/writeAccessLog.cpp
  test/server/ConfigHandler/writeErrorLog/writeErrorLog.cpp
  test/server/ConfigHandler/searchLocationConfig/searchLongestMatchLocationConfig.cpp
  test/server/ConfigHandler/searchServerConfig/searchServerConfigTest.cpp
  test/server/ConfigHandler/RootPath/RootPath.cpp
  test/server/HttpRequest/HttpRequestErrorTest.cpp 
  test/server/HttpRequest/HttpRequestOkTest.cpp 
  test/server/HttpRequest/cli_max_body_size_test.cpp 
	test/server/HttpResponse/AccessLog/accessLog_test.cpp
	test/server/HttpResponse/error/error_page_res_test.cpp
	test/server/HttpResponse/error_page/error_page_test.cpp
	test/server/HttpResponse/index/index_res_test.cpp
	test/server/HttpResponse/index/autoindex_res_test.cpp
	test/server/HttpResponse/mix/mix_directive_test.cpp
	test/server/HttpResponse/root/root_res_test.cpp
	test/server/HttpResponse/try_files/try_files_res_test.cpp
	test/server/HttpResponse/return/return_res_test.cpp
	test/server/WorkerConnectionsNumTest.cpp
  test/conf/lexer_test.cpp 
	test/conf/main_parser_test.cpp 
	test/conf/directive_parser/directives_test.cpp 
	test/conf/directive_parser/access_log/access_log_test.cpp 
	test/conf/directive_parser/alias/alias_test.cpp 
	test/conf/directive_parser/autoindex/autoindex_test.cpp 
	test/conf/directive_parser/allow/allow_test.cpp 
	test/conf/directive_parser/client_max_body_size/client_max_body_size_test.cpp 
	test/conf/directive_parser/deny/deny_test.cpp
	test/conf/directive_parser/error_log/error_log_test.cpp 
	test/conf/directive_parser/error_page/error_page_test.cpp 
	test/conf/directive_parser/index/index_test.cpp
	test/conf/directive_parser/keepalive_timeout/keepalive_timeout_test.cpp 
	test/conf/directive_parser/listen/listen_test.cpp
	test/conf/directive_parser/receive_timeout/receive_timeout_test.cpp
	test/conf/directive_parser/return/return_test.cpp
	test/conf/directive_parser/root/root_test.cpp
	test/conf/directive_parser/send_timeout/send_timeout_test.cpp
	test/conf/directive_parser/server_name/server_name_test.cpp
	test/conf/directive_parser/try_files/try_files_test.cpp
	test/conf/directive_parser/use/use_test.cpp
	# test/conf/directive_parser/userid/userid_test.cpp
	# test/conf/directive_parser/userid_domain/userid_domain_test.cpp
	# test/conf/directive_parser/userid_expires/userid_expires_test.cpp
	# test/conf/directive_parser/userid_path/userid_path_test.cpp
	# test/conf/directive_parser/userid_service/userid_service_test.cpp
	test/conf/directive_parser/worker_connections/worker_connections_test.cpp
	test/conf/init_log_test.cpp
	test/cgi/CgiExecutor_test.cpp
	test/cgi/CgiParser_test.cpp
)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
# debug
# target_link_libraries(${PROJECT_NAME}-googletest gtest_main -fsanitize=address -g)
# target_compile_options(${PROJECT_NAME}-googletest PRIVATE -fsanitize=address -g)
target_include_directories(${PROJECT_NAME}-googletest PRIVATE
	src
	src/server
	src/cgi
	src/http
	src/config
	src/utils
  test
	test/conf
	test/conf/directive_parser
  test/server/ConfigHandler
	test/server/HttpResponse
  test/server/HttpRequest
)
add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)

# target_compile_definitions(${PROJECT_NAME}-googletest PRIVATE TEST) # To avoid including main() when testing.
enable_testing()
