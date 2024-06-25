#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestSearchKeepaliveTimeout, http) {
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf", "127.0.0.1", 8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[0]});

  test.sameTime(test.config_handler_.config_->http_.keepalive_timeout_.getTime(),
                test.config_handler_.searchKeepaliveTimeout(test.tied_server_, test.request_.headers["Host"],
                                                            test.request_.uri));
}

TEST(ConfigHandlerTestSearchKeepaliveTimeout, server) {
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf", "127.0.0.2", 8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[1]});

  test.sameTime(test.config_handler_.config_->http_.server_list_[1].keepalive_timeout_.getTime(),
                test.config_handler_.searchKeepaliveTimeout(test.tied_server_, test.request_.headers["Host"],
                                                            test.request_.uri));
}

TEST(ConfigHandlerTestSearchKeepAliveTimeout, location) {
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf", "127.0.0.1", 8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http_.server_list_[0]});

  test.sameTime(
      test.config_handler_.config_->http_.server_list_[0].location_list_[1].keepalive_timeout_.getTime(),
      test.config_handler_.searchKeepaliveTimeout(test.tied_server_, test.request_.headers["Host"],
                                                  test.request_.uri));
}
