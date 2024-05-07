#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestSearchUseridExpires, http)
{
  test::ConfigHandlerTest test("searchTimeout/searchUseridExpires_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[0]});

  test.sameTime(test.config_handler_.config_->http.userid_expires.getTime(),
                 test.config_handler_.searchUseridExpires(
                    test.tied_server_,
                    test.request_.headers["Host"],
                    test.request_.uri
                 )
                 );   
}

TEST(ConfigHandlerTestSearchUseridExpires, server)
{
  test::ConfigHandlerTest test("searchTimeout/searchUseridExpires_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[1]});

  test.sameTime(test.config_handler_.config_->http.server_list[1].userid_expires.getTime(),
                 test.config_handler_.searchUseridExpires(
                   test.tied_server_,
                   test.request_.headers["Host"],
                   test.request_.uri
                 )
                 );   
}

TEST(ConfigHandlerTestSearchUseridExpires, location)
{
  test::ConfigHandlerTest test("searchTimeout/searchUseridExpires_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({&test.config_handler_.config_->http.server_list[0]});

  test.sameTime(test.config_handler_.config_->http.server_list[0].location_list[1].userid_expires.getTime(),
                 test.config_handler_.searchUseridExpires(
                    test.tied_server_,
                    test.request_.headers["Host"],
                    test.request_.uri
                 )
                 );   
}
