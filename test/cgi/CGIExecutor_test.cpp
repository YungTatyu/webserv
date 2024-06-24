#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CgiHandler.hpp"
#include "ConnectionManager.hpp"
#include "LimitExcept.hpp"
#include "Utils.hpp"
#include "WebServer.hpp"

typedef std::map<std::string, std::string> string_map;
typedef std::pair<std::string, std::string> string_pair;

namespace test {
HttpRequest initRequest(const config::REQUEST_METHOD method, const std::string& uri,
                        const std::string& version, const std::string& queries, const std::string& body,
                        const string_map& headers) {
  HttpRequest request;

  request.method = method;
  request.uri = uri;
  request.version = version;
  request.queries = queries;
  request.body = body;
  std::for_each(headers.begin(), headers.end(), [&request](const string_pair header) {
    request.headers.insert(std::make_pair(header.first, header.second));
  });

  return request;
}

HttpResponse initResponse(const std::string& root_path, const std::string& script_path,
                          const std::string& path_info) {
  HttpResponse response;

  response.root_path_ = root_path;
  response.res_file_path_ = script_path;
  response.path_info_ = path_info;

  return response;
}

std::string recvCgiResponse(cgi::CgiHandler& cgi_handler) {
  std::string response;
  const ssize_t buffer_size = 1024;

  while (true) {
    char buffer[buffer_size + 1];
    ssize_t bytes = recv(cgi_handler.getCgiSocket(), buffer, buffer_size, 0);
    if (bytes == -1) {
      std::cerr << "recv() " << std::strerror(errno) << "\n";
      return "";
    }
    buffer[bytes] = '\0';
    response += buffer;
    if (bytes < buffer_size) break;
  }
  return response;
}

void sendBody(const std::string& body, const int socket) {
  size_t sent_bytes = 0;
  const size_t buffersize = 1024;

  while (sent_bytes < body.size()) {
    size_t size = std::min(buffersize, body.size() - sent_bytes);
    std::string chunk = body.substr(sent_bytes, size);

    ssize_t ret = send(socket, chunk.c_str(), chunk.size(), 0);
    if (ret == -1) {
      std::cerr << "send() " << std::strerror(errno) << "\n";
      return;
    }
    sent_bytes += ret;
  }
}

int waitProcess(pid_t pid) {
  int status;
  if (waitpid(pid, &status, 0) == -1) std::cerr << "waitpid() " << std::strerror(errno) << "\n";
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  return -1;
}

void testCgiOutput(ConnectionData& cd, const std::string expect) {
  const HttpRequest& http_request = cd.request_;
  const HttpResponse& http_response = cd.response_;

  ASSERT_TRUE(cd.cgi_handler_.callCgiExecutor(http_response, http_request, 0));
  if (!http_request.body.empty()) sendBody(http_request.body, cd.cgi_handler_.getCgiSocket());
  waitProcess(cd.cgi_handler_.getCgiProcessId());
  const std::string actual = test::recvCgiResponse(cd.cgi_handler_);

  EXPECT_EQ(actual, expect);
}

void testMetaVars(const std::vector<string_pair>& test_results) {
  for (std::vector<string_pair>::const_iterator it = test_results.begin(); it != test_results.end(); ++it) {
    EXPECT_EQ(it->first, it->second);
  }
}

std::string searchMetaVar(const std::vector<const char*> meta_vars, const std::string target) {
  auto result = std::find_if(meta_vars.begin(), meta_vars.end(), [target](const std::string meta_var) {
    return meta_var.substr(0, meta_var.size()) == (meta_var + "=");
  });
  if (result == meta_vars.end()) return "";
  return *result;
}
}  // namespace test

TEST(cgi_executor, document_response) {
  ConnectionData cd;
  cd.request_ =
      test::initRequest(config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "", {{"Host", "tt"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/document_response.py", "");

  const std::string expect_header = "content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
  const std::string expect = !cd.request_.body.empty() ? (expect_header + cd.request_.body) : expect_header;
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, local_redirect_res) {
  ConnectionData cd;
  cd.request_ =
      test::initRequest(config::REQUEST_METHOD::GET, "test/cgi/cgi_files/executor/local_redirect_res.cgi",
                        "HTTP/1.1", "", "", {{"Host", "tt"}});
  cd.response_ = test::initResponse("./", "/test/cgi/cgi_files/executor/local_redirect_res.cgi", "");

  const std::string expect = "Location: /\r\n\r\n";
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, client_redirect_res) {
  ConnectionData cd;
  cd.request_ =
      test::initRequest(config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "", {{"Host", "tt"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/client_redirect_res.cgi", "");

  const std::string expect = "Location: https://www.google.com/\r\nMETHOD: GET\r\nSERVER_NAME: tachu\r\n\r\n";
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, client_redirect_res_doc) {
  ConnectionData cd;
  cd.request_ =
      test::initRequest(config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "", "", {{"Host", "tt"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/client_redirect_res_doc.cgi", "");

  const std::string expect_header = "Location: /\r\nStatus: 301\r\nContent-Type: text/html\r\n\r\n";
  const std::string expect = expect_header + "<h1>cgi response</h1><h2>client-redirdoc-response<h2>\n";
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, body) {
  ConnectionData cd;
  cd.request_ = test::initRequest(config::REQUEST_METHOD::POST, "/path/uri/", "HTTP/1.1", "",
                                 "<h1>cgi response</h1><h2>body<h2><p>this is body message\ntesting</p>\n",
                                 {{"Host", "tt"}, {"Content-Length", "59"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/body.py", "");

  const std::string expect_header = "Status: 200\r\nContent-Type: text/html\r\n\r\n";
  const std::string expect =
      expect_header + "<h1>cgi response</h1><h2>body<h2><p>this is body message\ntesting</p>\n";
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, meta_vars) {
  ConnectionData cd;
  cd.request_ =
      test::initRequest(config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "one=1&two=2&three=3", "",
                        {{"Host", "tt"}, {"content-type", "text/html"}, {"CONTENT_LENGTH", "10"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/meta_vars.py", "/a/b/c/d//e");

  const std::string expect_header = "content-type: text/html\r\nStatus: 200 OK\r\n\r\n";
  const std::string expect = expect_header + "<h1>env vars list</h1>" + "<h2>AUTH_TYPE=</h2>" +
                             "<h2>CONTENT_LENGTH=" + std::to_string(cd.request_.body.size()) + "</h2>" +
                             "<h2>CONTENT_TYPE=text/html</h2>" + "<h2>GATEWAY_INTERFACE=CGI/1.1</h2>" +
                             "<h2>PATH_INFO=/a/b/c/d//e</h2>" + "<h2>PATH_TRANSLATED=</h2>" +
                             "<h2>QUERY_STRING=one=1&two=2&three=3</h2>"
                             // + "<h2>REMOTE_ADDR=127.0.0.1</h2>" テスト不可のため、別のテストを追加
                             // + "<h2>REMOTE_HOST=127.0.0.1</h2>" テスト不可のため、別のテストを追加
                             + "<h2>REQUEST_METHOD=GET</h2>" +
                             "<h2>SCRIPT_NAME=test/cgi/cgi_files/executor/meta_vars.py</h2>" +
                             "<h2>SERVER_NAME=tt</h2>"
                             // + "<h2>SERVER_PORT=4242</h2>" テスト不可のため、別のテストを追加
                             + "<h2>SERVER_PROTOCOL=HTTP/1.1</h2>" + "<h2>SERVER_SOFTWARE=webserv/1.0</h2>";
  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, path_info_GET) {
  ConnectionData cd;
  cd.request_ = test::initRequest(config::REQUEST_METHOD::GET, "/path/uri/", "HTTP/1.1", "one=1&two=2&three=3",
                                 "", {{"Host", "tt"}, {"content-type", "text/html"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/path_info.py",
                                    "/test/cgi/cgi_files/executor/path_info_dir/");

  const std::string expect_header = "Content-Type: text/html\r\nStatus: 200 OK\r\n\r\n";
  const std::string expect = expect_header + "<ul>\r\n" + "<li><a href=\"a\">a</a></li>\r\n" +
                             "<li><a href=\"b\">b</a></li>\r\n" + "<li><a href=\"c\">c</a></li>\r\n" +
                             "</ul>\r\n";

  test::testCgiOutput(cd, expect);
}

TEST(cgi_executor, path_info_POST) {
  ConnectionData cd;
  cd.request_ = test::initRequest(config::REQUEST_METHOD::POST, "/path/uri/", "HTTP/1.1", "", "name=mahayase",
                                 {{"Host", "tt"}, {"content-type", "text/html"}, {"Content-Length", "13"}});
  cd.response_ = test::initResponse("./", "test/cgi/cgi_files/executor/post_and_pathinfo.py",
                                    "/test/cgi/cgi_files/executor/path_info_dir/");

  const std::string expect_header = "Content-Type: text/html\r\nStatus: 200 OK\r\n\r\n";
  const std::string expect =
      expect_header + "<!doctype html>\n" + "<html>\n" + "<body>\n" + "<h2>Hello, mahayase!</h2>\n\n" +
      "<h2>Enter your name</h2>\n" + "<form method=\"post\" action=\"\">\n" +
      "    <label for=\"name\">Name:</label>\n" + "    <input type=\"text\" id=\"name\" name=\"name\">\n" +
      "    <input type=\"submit\" value=\"Submit\">\n" + "</form>\n\n" + "<ul>\n" +
      "    <li><a href=\"a\">a</a></li>\n" + "    <li><a href=\"b\">b</a></li>\n" +
      "    <li><a href=\"c\">c</a></li>\n" + "</ul>\n" + "</body>\n" + "</html>\n";

  test::testCgiOutput(cd, expect);
}
