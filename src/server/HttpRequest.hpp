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
    PARSE_BEFORE = 0,
    PARSE_METHOD_DONE,
    PARSE_URI_DONE,
    PARSE_VERSION_DONE,
    PARSE_REQUEST_LINE_DONE,
    PARSE_HEADER_DONE,
    PARSE_BODY_DONE,
    PARSE_COMPLETE,
    PARSE_INPROGRESS,
    PARSE_NOT_IMPLEMENTED,
    PARSE_ERROR_BODY_TOO_LARGE,
    PARSE_ERROR
  };

  HttpRequest(const config::REQUEST_METHOD &method = config::UNKNOWN, const std::string &uri = "",
              const std::string &version = "",
              const std::map<std::string, std::string, Utils::CaseInsensitiveCompare> &headers =
                  std::map<std::string, std::string, Utils::CaseInsensitiveCompare>(),
              const std::string &queries = "", const std::string &body = "", const std::string &port = "",
              const ParseState parseState = PARSE_BEFORE);
  ~HttpRequest();

  static void parseRequest(std::string &rawRequest, HttpRequest &request);
  static ParseState parseChunkedBody(std::string &rawRequest, HttpRequest &request);
  static bool isInvalidLetter(unsigned char ch);
  static bool isValidContentLength(const std::string &str);
  static bool isParsePending(const HttpRequest &request);
  config::REQUEST_METHOD method;
  std::string uri;  // スキーマ、ポートは？？
  std::string version;
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  std::string queries;  // mapでもっていたが、子プロセスにQUERY_STRINGとして渡すからstringの方が良さげ。
  std::string body;
  std::string
      port_in_host;  // headerのhostにportの情報が格納されている場合がある、[:]も含んだ値で保管する(cgiのメタ変数で使用するため)

  ParseState parseState;

 private:
  std::string key_buf_;
  std::string val_buf_;
  std::string spc_buf_;
  int state_;  // より細かいフェーズのstate

  static ParseState parseMethod(std::string &rawRequest, HttpRequest &request);
  static ParseState parseUri(std::string &rawRequest, HttpRequest &request);
  static ParseState parseVersion(std::string &rawRequest, HttpRequest &request);
  static ParseState parseRequestLine(std::string &rawRequest, HttpRequest &request);
  static ParseState parseHeaders(std::string &rawRequest, HttpRequest &request);
  static ParseState parseBody(std::string &rawRequest, HttpRequest &request);
  static bool parseHost(std::string &host, HttpRequest &request);
  static std::string urlDecode(const std::string &encoded);
  static void resetBufs(HttpRequest &request);
  static bool isUniqHeaderDup(const HttpRequest &request, const std::string &header);
  static bool isValidHost(const std::string &str);
  static bool isValidUri(const std::string &str);
  static bool isChunkBytesBiggerThanCliMaxBodySize(size_t chunk_bytes, std::string &total_bytes,
                                                   size_t cli_max_body_size);
  static void clear(HttpRequest &request);
};

#endif
