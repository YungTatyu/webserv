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
	src/server/CGIHandler.cpp
	src/server/CGIParser.cpp
	src/server/CGIExecutor.cpp
	# src/server
	src/server/ConfigHandler.cpp
	src/server/ConnectionManager.cpp
	src/server/HttpRequest.cpp
	src/server/HttpResponse.cpp
	src/server/NetworkIOHandler.cpp
	src/server/RequestHandler.cpp
	src/server/SysCallWrapper.cpp
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
	src/utils/Utils.cpp
	
	# test files
	test/server/TimerTree_test.cpp
	test/server/config_handler_test.cpp
	test/server/HttpRequestErrorTest.cpp 
	test/server/HttpRequestOkTest.cpp 
	test/server/HttpResponseTest.cpp
	test/server/HttpResponse/error/error_page_res_test.cpp
	test/server/HttpResponse/index/index_res_test.cpp
	test/server/WorkerConnectionsNumTest.cpp
	test/conf/confLexerTest.cpp 
	test/conf/directives_test.cpp 
	test/conf/main_parser_test.cpp 
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
	test/conf/directive_parser/userid/userid_test.cpp
	test/conf/directive_parser/userid_domain/userid_domain_test.cpp
	test/conf/directive_parser/userid_expires/userid_expires_test.cpp
	test/conf/directive_parser/userid_path/userid_path_test.cpp
	test/conf/directive_parser/userid_service/userid_service_test.cpp
	test/conf/directive_parser/worker_connections/worker_connections_test.cpp
	test/conf/init_log_test.cpp
	test/cgi/CGIExecutor_test.cpp
	test/cgi/CGIParser_test.cpp
)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
target_include_directories(${PROJECT_NAME}-googletest PRIVATE
	src
	src/server
	src/config
	src/utils
	test/conf
	test/server/HttpResponse
)
add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
# target_compile_definitions(${PROJECT_NAME}-googletest PRIVATE TEST) # To avoid including main() when testing.
enable_testing()
