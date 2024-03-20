#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "CGIHandler.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

typedef std::map<std::string, std::string> string_map;

namespace test
{
	HttpRequest	initRequest(
		const std::string& method,
		const std::string& uri,
		const std::string& version,
		const std::string& queries,
		const std::string& body,
		const string_map& headers
	)
	{
		HttpRequest request;

		request.method = method;
		request.uri = uri;
		request.version = version;
		request.queries = queries;
		request.body = body;

		std::for_each(headers.begin(), headers.end(), [&request](const std::pair<std::string, std::string> header){
			request.headers.insert(std::make_pair(header.first, header.second));
		});

		return request;
	}

	void	testCgiOutput(const std::string actual, const std::string expect)
	{
		EXPECT_EQ(actual, expect);
	}

	std::string	captureStdout(
		cgi::CGIHandler& cgi_handler,
		const std::string& cgi_path,
		const HttpRequest& http_request
	)
	{
		testing::internal::CaptureStdout();
		cgi_handler.callCgiExecutor(cgi_path, http_request);
		std::string	stdout_ = testing::internal::GetCapturedStdout();
		return stdout_;
	}

	// void	testMetaVars(const std::vector<string_map> &test_results)
	void	testMetaVars(const std::vector<std::pair<std::string, std::string> > &test_results)
	{
		for (std::vector<std::pair<std::string, std::string> >::const_iterator it = test_results.begin();
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
		auto	result = std::find_if(meta_vars.begin(), meta_vars.end(), [target](const std::string meta_var){
			return meta_var.substr(0, meta_var.size()) == (meta_var + "=");
		});
		if (result == meta_vars.end())
			return "";
		return *result;
	}
} // namespace test

TEST(cgi_executor, meta_vars)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		"GET",
		"/path/uri/",
		"HTTP/1.1",
		"one=1&two=2&three=3",
		"this is body\n",
		{
			{"Host", "tt"},
			{"content-type", "text"},
			{"CONTENT_LENGTH", "10"}
		}
	);

	const std::vector<const char*>	meta_vars = cgi_handler.getCgiExecutor().getMetaVars();
	std::vector<std::pair<std::string, std::string>> result = {
		{test::searchMetaVar(meta_vars, "AUTH_TYPE"), "AUTH_TYPE="}, // AUTH_TYPE
		{test::searchMetaVar(meta_vars, "CONTENT_LENGTH"), (std::string("CONTENT_LENGTH=") + std::to_string(request.body.size()))}, // CONTENT_LENGTH
		{test::searchMetaVar(meta_vars, "CONTENT_TYPE"), "CONTENT_TYPE=text"}, // CONTENT_TYPE
		{test::searchMetaVar(meta_vars, "GATEWAY_INTERFACE"), "GATEWAY_INTERFACE=CGI/1.1"}, // GATEWAY_INTERFACE
		{test::searchMetaVar(meta_vars, "PATH_INFO"), "PATH_INFO="}, // PATH_INFO
		{test::searchMetaVar(meta_vars, "PATH_TRANSLATED"), "PATH_TRANSLATED="}, // PATH_TRANSLATED
		{test::searchMetaVar(meta_vars, "QUERY_STRING"), "QUERY_STRING=one=1&two=2&three=3"}, // QUERY_STRING
		{test::searchMetaVar(meta_vars, "REMOTE_ADDR"), "REMOTE_ADDR=client address"}, // REMOTE_ADDR
		{test::searchMetaVar(meta_vars, "REMOTE_HOST"), "REMOTE_HOST=client address"}, // REMOTE_HOST
		{test::searchMetaVar(meta_vars, "REQUEST_METHOD"), "REQUEST_METHOD=GET"}, // REQUEST_METHOD
		{test::searchMetaVar(meta_vars, "SCRIPT_NAME"), "SCRIPT_NAME=/path/uri/"}, // SCRIPT_NAME
		{test::searchMetaVar(meta_vars, "SERVER_NAME"), "SERVER_NAME=tt"}, // SERVER_NAME
		{test::searchMetaVar(meta_vars, "SERVER_PORT"), "SERVER_PORT="}, // SERVER_PORT
		{test::searchMetaVar(meta_vars, "SERVER_PROTOCOL"), "SERVER_PROTOCOL=HTTP/1.1"}, // SERVER_PROTOCOL
		{test::searchMetaVar(meta_vars, "SERVER_SOFTWARE"), "SERVER_SOFTWARE=webserv/1.0"} // SERVER_SOFTWARE
	};

	test::testMetaVars(result);
}

TEST(cgi_executor, document_response)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		"GET",
		"/path/uri/",
		"HTTP/1.1",
		"one=1&two=2&three=3",
		"this is body\n",
		{
			{"Host", "tt"},
			{"content-type", "text"},
			{"CONTENT_LENGTH", "10"}
		}
	);

	cgi_handler.callCgiExecutor("test/cgi/cgi_files/executor/document_response.py", request);
	const std::string actual = test::captureStdout(cgi_handler,
		"test/cgi/cgi_files/executor/document_response.py",
		request
	);

	const std::string expect_header = "content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
	const std::string expect = request.body != "" ? (expect_header + request.body) : expect_header;
	test::testCgiOutput(actual, expect);
}
