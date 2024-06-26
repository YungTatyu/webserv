#ifndef CGI_CGI_PARSER_HPP
#define CGI_CGI_PARSER_HPP

#include <map>
#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"

namespace cgi {
enum PARSE_STATE {
  PARSE_BEFORE = 0,
  PARSE_COMPLETE,
  PARSE_INPROGRESS,
  PARSE_HEADER_DONE,
  PARSE_BODY_DONE,
  PARSE_ERROR
};

typedef std::map<std::string, std::string, utils::CaseInsensitiveCompare> string_map_case_insensitive;

class CgiParser {
 private:
  CgiParser(const CgiParser& other);
  CgiParser& operator=(const CgiParser& other);
  void init(HttpResponse& http_response);
  void parseHeaders(const std::string& cgi_response);
  void parseBody(const std::string& cgi_response);
  void finishParsing();
  bool isValidStatusCode(const std::string& status_code) const;
  void setStatusCode(const std::string& value);
  void finalizeStatusCode();
  void eraseHeader(const std::string& header);

  // 以下のメンバ変数はHttpResponseのメンバ変数のポインタ
  string_map_case_insensitive* headers_;
  std::string* body_;
  long* status_code_;
  std::string* status_code_line_;
  size_t ri_;  // cgi response index

 public:
  CgiParser();
  ~CgiParser();
  bool parse(HttpResponse& http_response, const std::string& cgi_response, PARSE_STATE init_state);

  PARSE_STATE state_;
};
}  // namespace cgi

#endif
