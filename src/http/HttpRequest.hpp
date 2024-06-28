#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "LimitExcept.hpp"
#include "utils.hpp"

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

  HttpRequest(config::REQUEST_METHOD method = config::UNKNOWN, const std::string &uri = "",
              const std::string &version = "",
              const std::map<std::string, std::string, utils::CaseInsensitiveCompare> &headers =
                  std::map<std::string, std::string, utils::CaseInsensitiveCompare>(),
              const std::string &queries = "", const std::string &body = "", const std::string &port = "",
              ParseState parse_state = PARSE_BEFORE);
  HttpRequest(const HttpRequest &other);
  ~HttpRequest();
  HttpRequest &operator=(const HttpRequest &other);
  static void parseRequest(std::string &raw_request, HttpRequest &request);
  static ParseState parseChunkedBody(std::string &raw_request, HttpRequest &request);
  static bool isInvalidLetter(unsigned char ch);
  static bool isValidContentLength(const std::string &str);
  static bool isParsePending(const HttpRequest &request);

  config::REQUEST_METHOD method_;
  std::string uri_;  // スキーマ、ポートは？？
  std::string version_;
  std::map<std::string, std::string, utils::CaseInsensitiveCompare> headers_;
  std::string queries_;  // mapでもっていたが、子プロセスにQUERY_STRINGとして渡すからstringの方が良さげ。
  std::string body_;
  // headerのhostにportの情報が格納されている場合がある、[:]も含んだ値で保管する(cgiのメタ変数で使用するため)
  std::string port_in_host_;
  ParseState parse_state_;

 private:
  static ParseState parseMethod(std::string &raw_request, HttpRequest &request);
  static ParseState parseUri(std::string &raw_request, HttpRequest &request);
  static ParseState parseVersion(std::string &raw_request, HttpRequest &request);
  static ParseState parseRequestLine(std::string &raw_request, HttpRequest &request);
  static ParseState parseHeaders(std::string &raw_request, HttpRequest &request);
  static ParseState parseBody(std::string &raw_request, HttpRequest &request);
  static bool parseHost(std::string &host, HttpRequest &request);
  static std::string urlDecode(const std::string &encoded);
  static void resetBufs(HttpRequest &request);
  static bool isUniqHeaderDup(const HttpRequest &request, const std::string &header);
  static bool isValidHost(const std::string &str);
  static bool isValidUri(const std::string &str);
  static bool isChunkBytesBiggerThanCliMaxBodySize(size_t chunk_bytes, std::string &total_bytes,
                                                   size_t cli_max_body_size);
  static void clear(HttpRequest &request);

  std::string key_buf_;
  std::string val_buf_;
  std::string spc_buf_;
  int state_;  // より細かいフェーズのstate
};

#endif
