#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestSearchSendTimeout, http) {
  test::ConfigHandlerTest test("searchTimeout/searchSendTimeout_test.conf", "127.0.0.1", 8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[0]});

  test.sameTime(test.config_handler_.config_->http_.send_timeout_.getTime(),
                test.config_handler_.searchSendTimeout(test.tied_server_, test.request_.headers_["Host"],
                                                       test.request_.uri_));
}

TEST(ConfigHandlerTestSearchSendTimeout, server) {
  test::ConfigHandlerTest test("searchTimeout/searchSendTimeout_test.conf", "127.0.0.2", 8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[1]});

  test.sameTime(test.config_handler_.config_->http_.server_list_[1].send_timeout_.getTime(),
                test.config_handler_.searchSendTimeout(test.tied_server_, test.request_.headers_["Host"],
                                                       test.request_.uri_));
}

TEST(ConfigHandlerTestSearchSendTimeout, location) {
  test::ConfigHandlerTest test("searchTimeout/searchSendTimeout_test.conf", "127.0.0.1", 8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[0]});

  test.sameTime(test.config_handler_.config_->http_.server_list_[0].location_list_[1].send_timeout_.getTime(),
                test.config_handler_.searchSendTimeout(test.tied_server_, test.request_.headers_["Host"],
                                                       test.request_.uri_));
}
