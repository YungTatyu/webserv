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

	// void	initConfigHandler(const std::vector<ip_address_pair> &ip_addresses) {
	void	initConfigHandler(const ip_address_pair &ip_addresses) {
		this->tied_server = this->config_handler_.createTiedServer(ip_addresses.first, ip_addresses.second);
		// for (std::vector<ip_address_pair>::const_iterator it = 0; it != ip_addresses.end(); ++it)
		// 	this->tied_servers.push_back(this->config_handler_.createTiedServer(it->first, it->second));
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
		this->final_response_ = HttpResponse::generateResponse(this->request_, this->response_, this->tied_server, this->sockets_[0], this->config_handler_);
	}

	void	testHeaders(const std::vector<string_map> &expects) const {
		for (std::vector<string_map>::const_iterator vit = expects.begin(); vit != expects.end(); ++vit)
		{
			for (string_map::const_iterator sit = vit->begin(); sit != vit->end(); ++sit)
			{
				EXPECT_NO_THROW(
					EXPECT_EQ(this->response_.headers_.at(sit->first), sit->second)
				);
			}
		}
		EXPECT_EQ(this->response_.headers_.size(), expects.size());
	}

	void	testBody(const std::string &expect) const {
		EXPECT_EQ(this->response_.body_, expect);
	}

	void	testResponse(const std::string &expect) const {
		EXPECT_EQ(this->final_response_, expect);
	}

	int	sockets_[2];
	const std::string	conf_path_;
	std::string	final_response_;
	ConfigHandler	config_handler_;
	HttpRequest	request_;
	HttpResponse	response_;
	TiedServer	tied_server;
	std::vector<TiedServer>	tied_servers_;
	std::vector<HttpResponse>	responses_;
	std::vector<std::string>	final_responses_;
	int	getSocket() const {return sockets_[0];}
};
} // namespace test

#endif