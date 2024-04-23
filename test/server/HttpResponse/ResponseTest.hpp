#ifndef TEST_HTTP_RESPONSE_TEST_HPP
#define TEST_HTTP_RESPONSE_TEST_HPP

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <string>
#include <cstring>
#include <vector>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ConfigHandler.hpp"

namespace test
{

typedef std::pair<std::string, unsigned int> ip_address_pair;
typedef std::map<std::string, std::string, Utils::CaseInsensitiveCompare> string_map_case_insensitive;
typedef std::map<std::string, std::string> string_map;

class ResponseTest
{
private:
	ResponseTest();
	void	err(const std::string &err_msg)
	{
		FAIL() << err_msg << std::strerror(errno);
	}

public:
	ResponseTest(const std::string &conf_path) : conf_path_(conf_path) {}

	~ResponseTest() {
		delete this->config_handler_.config_;
		close(this->sockets_[0]);
		close(this->sockets_[1]);
	}

	/**
	 * @brief Set the Up object
	 * 
	 * constructorでASSERT_NE()やFAIL()を呼べないのでここで処理する
	 * 
	 */
	void	setUp()
	{
		this->config_handler_.loadConfiguration(config::initConfig(this->conf_path_));
		ASSERT_NE(this->config_handler_.config_, nullptr);
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockets_) == -1)
			err("socketpair(): ");
	}

	void	initConfigHandler(const std::vector<ip_address_pair> &ip_addresses) {
		for (std::vector<ip_address_pair>::const_iterator it = ip_addresses.begin(); it != ip_addresses.end(); ++it)
			this->tied_servers_.push_back(this->config_handler_.createTiedServer(it->first, it->second));
		this->responses_.resize(ip_addresses.size());
	}

	void	initRequest(const string_map_case_insensitive &headers,
		const std::string &uri,
		const HttpRequest::ParseState state,
		const std::string &body="",
		const std::string &queries="",
		const std::string &version="HTTP/1.1"
		) {
		this->request_.headers = headers;
		this->request_.uri = uri;
		this->request_.parseState = state;
		this->request_.body = body;
		this->request_.queries = queries;
		this->request_.version = version;
	}

	/**
	 * @brief testを実行する前に呼ぶ必要がある
	 * 
	 */
	void	generateResponse() {
		int	i = 0;
		std::for_each(this->tied_servers_.begin(), this->tied_servers_.end(), [this, &i](TiedServer tied_server) {
			this->final_responses_.push_back(HttpResponse::generateResponse(this->request_, this->responses_[i], tied_server, this->sockets_[0], this->config_handler_));
			++i;
		});
	}

	void	testHeaders(const string_map &expects) const {
		std::for_each(this->responses_.begin(), this->responses_.end(), [&expects](HttpResponse response) {
			for (string_map::const_iterator it = expects.begin(); it != expects.end(); ++it)
			{
				EXPECT_NO_THROW(
					EXPECT_EQ(response.headers_.at(it->first), it->second)
				);
			}
			EXPECT_EQ(response.headers_.size(), expects.size());
		});
	}

	void	testBody(const std::string &expect) const {
		std::for_each(this->responses_.begin(), this->responses_.end(), [&expect](HttpResponse response) {
			EXPECT_EQ(response.body_, expect);
		});
	}

	void	testResponse(const std::string &expect) const {
		std::for_each(this->final_responses_.begin(), this->final_responses_.end(), [&expect](std::string response) {
			EXPECT_EQ(response, expect);
		});
	}

	int	sockets_[2];
	const std::string	conf_path_;
	ConfigHandler	config_handler_;
	HttpRequest	request_;
	std::vector<TiedServer>	tied_servers_;
	std::vector<HttpResponse>	responses_;
	std::vector<std::string>	final_responses_;
};
} // namespace test

#endif