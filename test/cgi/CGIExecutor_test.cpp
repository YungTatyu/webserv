#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "CGIHandler.hpp"
#include "ConnectionManager.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "Utils.hpp"
#include <cstring>
#include "LimitExcept.hpp"

typedef std::map<std::string, std::string> string_map;
typedef std::pair<std::string, std::string> string_pair;

namespace test
{
	HttpRequest	initRequest(
		const config::REQUEST_METHOD method,
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

		std::for_each(headers.begin(), headers.end(), [&request](const string_pair header){
			request.headers.insert(std::make_pair(header.first, header.second));
		});

		return request;
	}

	std::string	readCgiResponse(cgi::CGIHandler& cgi_handler)
	{
		std::string	response;
		const size_t	buffer_size = 1024;
		ssize_t	total_read_bytes = 0;
	
		while (true)
		{
			char	buffer[buffer_size + 1];
			ssize_t	bytes = recv(cgi_handler.getCgiSocket(), buffer, buffer_size, 0);
			if (bytes == -1)
			{
				std::cerr << "recv() " << std::strerror(errno) << "\n";
				return "";	
			}
			buffer[bytes] = '\0';
			response += buffer;
			if (bytes < buffer_size)
				break;
			total_read_bytes += bytes;
		}
		return response;
	}

	void	sendBody(const std::string& body, const int socket)
	{
		Utils::wrapperWrite(socket, body);
	}

	int	waitProcess(pid_t pid)
	{
		int	status;
		if (waitpid(pid, &status, 0) == -1)
			std::cerr << "waitpid() " << std::strerror(errno) << "\n";
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		return -1;
	}

	void	testCgiOutput(
		cgi::CGIHandler& cgi_handler,
		const std::string& cgi_path,
		const HttpRequest& http_request,
		const std::string expect
	)
	{
		cgi_handler.callCgiExecutor(cgi_path, http_request, 0);
		if (!http_request.body.empty())
		{
			sendBody(http_request.body, cgi_handler.getCgiSocket());
		// 	waitProcess(cgi_handler.getCgiProcessId());
		}
		const std::string actual = test::readCgiResponse(cgi_handler);

		EXPECT_EQ(actual, expect);
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
		auto	result = std::find_if(meta_vars.begin(), meta_vars.end(), [target](const std::string meta_var){
			return meta_var.substr(0, meta_var.size()) == (meta_var + "=");
		});
		if (result == meta_vars.end())
			return "";
		return *result;
	}
} // namespace test

TEST(cgi_executor, document_response)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "",
		{{"Host", "tt"}}
	);

	const std::string expect_header = "content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
	const std::string expect = !request.body.empty() ? (expect_header + request.body) : expect_header;
	test::testCgiOutput(
		cgi_handler,
		"test/cgi/cgi_files/executor/document_response.py",
		request,
		expect
	);
}

TEST(cgi_executor, local_redirect_res)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "",
		{{"Host", "tt"}}
	);

	const std::string expect = "Location: /\r\n\r\n";
	test::testCgiOutput(
		cgi_handler,
		"test/cgi/cgi_files/executor/local_redirect_res.php",
		request,
		expect
	);
}

TEST(cgi_executor, client_redirect_res)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "",
		{{"Host", "tt"}}
	);

	const std::string expect = "Location: https://www.google.com/\r\nMETHOD: GET\r\nSERVER_NAME: tachu\r\n\r\n";
	test::testCgiOutput(
		cgi_handler,
		"test/cgi/cgi_files/executor/client_redirect_res.cgi",
		request,
		expect
	);
}

TEST(cgi_executor, client_redirect_res_doc)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "",
		{{"Host", "tt"}}
	);

	const std::string expect_header = "Location: /\r\nStatus: 301\r\nContent-Type: text/html\r\n\r\n";
	const std::string expect = expect_header + "<h1>cgi response</h1><h2>client-redirdoc-response<h2>\n";
	test::testCgiOutput(
		cgi_handler,
		"test/cgi/cgi_files/executor/client_redirect_res_doc.cgi",
		request,
		expect
	);
}

TEST(cgi_executor, body)
{
	cgi::CGIHandler	cgi_handler;
	HttpRequest	request = test::initRequest(
		config::REQUEST_METHOD::POST, "/path/uri/", "HTTP/1.1", "",
		"<h1>cgi response</h1><h2>body<h2><p>this is body message\ntesting</p>\n",
		{{"Host", "tt"}}
	);

	const std::string expect_header = "Status: 200\r\nContent-Type: text/html\r\n\r\n";
	const std::string expect = expect_header + "<h1>cgi response</h1><h2>body<h2><p>this is body message\ntesting</p>\n";
	test::testCgiOutput(
		cgi_handler,
		"test/cgi/cgi_files/executor/body.py",
		// "test/cgi/cgi_files/executor/body.cgi",
		request,
		expect
	);
}


TEST(cgi_executor, meta_vars)
{
	ConnectionData	cd;
	cd.request = test::initRequest(
		config::REQUEST_METHOD::GET,
		"/path/uri/",
		"HTTP/1.1",
		"one=1&two=2&three=3",
		"",
		{
			{"Host", "tt"},
			{"content-type", "text/html"},
			{"CONTENT_LENGTH", "10"}
		}
	);

	const std::string expect_header = "content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
	const std::string expect = expect_header + "<h1>env vars list</h1>"
		+ "<h2>AUTH_TYPE=</h2>"
		+ "<h2>CONTENT_LENGTH=" + std::to_string(cd.request.body.size()) + "</h2>"
		+ "<h2>CONTENT_TYPE=text/html</h2>"
		+ "<h2>GATEWAY_INTERFACE=CGI/1.1</h2>"
		+ "<h2>PATH_INFO=</h2>"
		+ "<h2>PATH_TRANSLATED=</h2>"
		+ "<h2>QUERY_STRING=one=1&two=2&three=3</h2>"
		// + "<h2>REMOTE_ADDR=127.0.0.1</h2>" テスト不可のため、別のテストを追加
		// + "<h2>REMOTE_HOST=127.0.0.1</h2>" テスト不可のため、別のテストを追加
		+ "<h2>REQUEST_METHOD=GET</h2>"
		+ "<h2>SCRIPT_NAME=/path/uri/</h2>"
		+ "<h2>SERVER_NAME=tt</h2>"
		// + "<h2>SERVER_PORT=4242</h2>" テスト不可のため、別のテストを追加
		+ "<h2>SERVER_PROTOCOL=HTTP/1.1</h2>"
		+ "<h2>SERVER_SOFTWARE=webserv/1.0</h2>"
	;
	test::testCgiOutput(
		cd.cgi_handler_,
		"test/cgi/cgi_files/executor/meta_vars.py",
		cd.request,
		expect
	);
}