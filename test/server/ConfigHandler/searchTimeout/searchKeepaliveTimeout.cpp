#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTest, searchKeepaliveTimeout_http)
{
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[0]});

  test.sameTime(test.config_handler_.config_->http.keepalive_timeout.getTime(),
                 test.config_handler_.searchKeepaliveTimeout(
                    test.tied_server_,
                    test.request_.headers["Host"],
                    test.request_.uri
                 )
                 );   
}

TEST(ConfigHandlerTest, searchKeepaliveTimeout_server)
{
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[1]});

  test.sameTime(test.config_handler_.config_->http.server_list[1].keepalive_timeout.getTime(),
                 test.config_handler_.searchKeepaliveTimeout(
                   test.tied_server_,
                   test.request_.headers["Host"],
                   test.request_.uri
                 )
                 );   
}

TEST(ConfigHandlerTest, SearchKeepAliveTimeout_location)
{
  test::ConfigHandlerTest test("searchTimeout/searchKeepaliveTimeout_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[0]});

  test.sameTime(test.config_handler_.config_->http.server_list[0].location_list[1].keepalive_timeout.getTime(),
                 test.config_handler_.searchKeepaliveTimeout(
                    test.tied_server_,
                    test.request_.headers["Host"],
                    test.request_.uri
                 )
                 );   
}
