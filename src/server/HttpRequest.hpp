#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "LimitExcept.hpp"
#include "Utils.hpp"

class HttpRequest {
 public:
  enum ParseState {
    PARSE_BEFORE,
    PARSE_COMPLETE,
    PARSE_INPROGRESS,
    PARSE_METHOD_DONE,
    PARSE_REQUEST_LINE_DONE,
    PARSE_URI_DONE,
    PARSE_VERSION_DONE,
    PARSE_HEADER_DONE,
    PARSE_BODY_DONE,
    PARSE_ERROR,
    PARSE_VERSION_ERROR
  };

  HttpRequest(const config::REQUEST_METHOD &method = config::UNKNOWN, const std::string &uri = "",
              const std::string &version = "",
              const std::map<std::string, std::string, Utils::CaseInsensitiveCompare> &headers =
                  std::map<std::string, std::string, Utils::CaseInsensitiveCompare>(),
              const std::string &queries = "", const std::string &body = "",
              const ParseState parseState = PARSE_BEFORE);
  ~HttpRequest();

  // static HttpRequest parseRequest(std::string& rawRequest, HttpRequest&
  // oldRequest);
  static void parseRequest(std::string &rawRequest, HttpRequest &oldRequest);
  static HttpRequest doParseRequest(std::string &rawRequest, HttpRequest &oldRequest);
  static ParseState doParseChunked(std::string &rawRequest, HttpRequest &request);

  config::REQUEST_METHOD method;
  std::string key_buf_;
  std::string val_buf_;
  std::string uri;  // スキーマ、ポートは？？
  std::string version;
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  std::string queries;  // mapでもっていたが、子プロセスにQUERY_STRINGとして渡すからstringの方が良さげ。
  std::string body;

  ParseState parseState;
  int state_; // より細かいフェーズのstate

 private:
  static ParseState parseMethod(std::string &rawRequest, HttpRequest &newRequest);
  static ParseState parseUri(std::string &rawRequest, HttpRequest &newRequest);
  static ParseState parseVersion(std::string &rawRequest, HttpRequest &newRequest);
  static ParseState parseRequestLine(std::string &rawRequest, HttpRequest &newRequest);
  static ParseState parseHeaders(std::string &rawRequest, HttpRequest &newRequest);
  static ParseState parseBody(std::string &rawRequest, HttpRequest &newRequest);
  static std::string urlDecode(const std::string &encoded);
  static void clearBuf(HttpRequest &request);
};

#endif
