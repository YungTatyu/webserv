#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "CGIHandler.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

typedef std::pair<std::string, std::string> string_pair;

namespace test
{
	HttpRequest	initRequest(
		const std::string& method,
		const std::vector<string_pair>& queries
	)
	{
		HttpRequest request;
		request.method = method;
		
		std::for_each(queries.begin(), queries.end(), [](string_pair pair){
			request.queries.insert(pair);
		});
	}

	void	testCgiOutput(const std::string actual, const std::string expect)
	{
		EXPECT_EQ(actual, expect);
	}

	std::string	captureStdout(
		const CGIHandler& cgi_handler,
		const std::string& cgi_path,
		const HttpRequest& http_request,
		const int socket
	)
	{
		testing::internal::CaptureStdout();
		cgi_handler.callCgiExecutor(cgi_path, http_request, socket);
		std::string	stdout_ = testing::internal::GetCapturedStdout();
		return stdout_;
	}

	void	testMetaVars(const std::vector<string_pair> &test_results)
	{
		for (std::vector<string_pair>::const_iterator it = test_results.begin();
			it != test_results.end();
			++it
		)
		{
			EXPECT_EQ(it->first, it->second);
		}
	}

	std::string	searchMetaVar(
		const std::vector<const char*>	meta_vars,
		const std::string target
	)
	{
		std::for_each(meta_vars.begin(), meta_vars.end(), [target](const std::string meta_var){
			if (meta_var.starts_with(meta_var + "="))
				return meta_var.substr(meta_var.size() + 1);
		});
		return "";
	}
} // namespace test

TEST(cgi_executor, meta_vars)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		"GET",
		{
			{"one", "1"},
			{"two", "2"},
			{"three", "3"},
		}
	);

	const std::vector<const char*>	meta_vars = cgi_handler.getCgiExecutor().getMetaVars();
	test::testMetaVars(
		{test::searchMetaVar(meta_vars, "AUTH_TYPE"), ""}, // AUTH_TYPE
		{test::searchMetaVar(meta_vars, "CONTENT_LENGTH"), ""}, // CONTENT_LENGTH
		{test::searchMetaVar(meta_vars, "CONTENT_TYPE"), ""}, // CONTENT_TYPE
		{test::searchMetaVar(meta_vars, "GATEWAY_INTERFACE"), ""}, // GATEWAY_INTERFACE
		{test::searchMetaVar(meta_vars, "PATH_INFO"), ""}, // PATH_INFO
		{test::searchMetaVar(meta_vars, "PATH_TRANSLATED"), ""}, // PATH_TRANSLATED
		{test::searchMetaVar(meta_vars, "QUERY_STRING"), ""}, // QUERY_STRING
		{test::searchMetaVar(meta_vars, "REMOTE_ADDR"), ""}, // REMOTE_ADDR
		{test::searchMetaVar(meta_vars, "REMOTE_HOST"), ""}, // REMOTE_HOST
		{test::searchMetaVar(meta_vars, "REMOTE_IDENT"), ""}, // REMOTE_IDENT
		{test::searchMetaVar(meta_vars, "REMOTE_USER"), ""}, // REMOTE_USER
		{test::searchMetaVar(meta_vars, "REQUEST_METHOD"), ""}, // REQUEST_METHOD
		{test::searchMetaVar(meta_vars, "SCRIPT_NAME"), ""}, // SCRIPT_NAME
		{test::searchMetaVar(meta_vars, "SERVER_NAME"), ""}, // SERVER_NAME
		{test::searchMetaVar(meta_vars, "SERVER_PORT"), ""}, // SERVER_PORT
		{test::searchMetaVar(meta_vars, "SERVER_PROTOCOL"), ""}, // SERVER_PROTOCOL
		{test::searchMetaVar(meta_vars, "SERVER_SOFTWARE"), ""}, // SERVER_SOFTWARE
		{test::searchMetaVar(meta_vars, "SERVER_SOFTWARE"), ""}, // SERVER_SOFTWARE
	);
}

TEST(cgi_executor, document_response)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		"GET",
		{
			{"one", "1"},
			{"two", "2"},
			{"three", "3"},
		}
	);

	cgi_handler.createCgiProcess();
	const std::string actul = captureStdout(cgi_handler,
		"test/cgi/cgi_files/executor/document_response.py",
		cgi_handler[cgi::SOCKET_PARENT]
	);

	const std::string expect_header = "Content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
	const std::string expect = cgi_handler.method == "POST" ? (expect_header + cgi_handler.body) : expect_header;
	testCgiOutput(actual, expect);
}
