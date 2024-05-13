#include "HttpResponse.hpp"

#include <cstring>
#include <ctime>
#include <iomanip>

#include "CGIHandler.hpp"
#include "Utils.hpp"

static const std::string kTRY_FILES = "try_files";
static const std::string kINDEX = "index";
static const std::string kRETURN = "return";
static const char* kContentType = "Content-Type";
static const char* kHtml = "text/html";
static const char* kTextPlain = "text/plain";
static const char* kDefaultPage = "defaut.html";

std::map<int, std::string> HttpResponse::status_line_map_;
std::map<int, const std::string*> HttpResponse::default_error_page_map_;

static const std::string http_version = "HTTP/1.1";

static const std::string webserv_error_page_tail =
    "<hr><center>webserv/1.0</center>\r\n</body>\r\n</html>\r\n";

static const std::string webserv_error_301_page =
    "<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved "
    "Permanently</h1></center>\r\n";

static const std::string webserv_error_302_page =
    "<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n";

static const std::string webserv_error_303_page =
    "<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See "
    "Other</h1></center>\r\n";

static const std::string webserv_error_307_page =
    "<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary "
    "Redirect</h1></center>\r\n";

static const std::string webserv_error_308_page =
    "<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent "
    "Redirect</h1></center>\r\n";

static const std::string webserv_error_400_page =
    "<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad "
    "Request</h1></center>\r\n";

static const std::string webserv_error_401_page =
    "<html>\r\n<head><title>401 Authorization Required</title></head>\r\n<body>\r\n<center><h1>401 "
    "Authorization Required</h1></center>\r\n";

static const std::string webserv_error_402_page =
    "<html>\r\n<head><title>402 Payment Required</title></head>\r\n<body>\r\n<center><h1>402 Payment "
    "Required</h1></center>\r\n";

static const std::string webserv_error_403_page =
    "<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 "
    "Forbidden</h1></center>\r\n";

static const std::string webserv_error_404_page =
    "<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not "
    "Found</h1></center>\r\n";

static const std::string webserv_error_405_page =
    "<html>\r\n<head><title>405 Not Allowed</title></head>\r\n<body>\r\n<center><h1>405 Not "
    "Allowed</h1></center>\r\n";

static const std::string webserv_error_406_page =
    "<html>\r\n<head><title>406 Not Acceptable</title></head>\r\n<body>\r\n<center><h1>406 Not "
    "Acceptable</h1></center>\r\n";

static const std::string webserv_error_408_page =
    "<html>\r\n<head><title>408 Request Time-out</title></head>\r\n<body>\r\n<center><h1>408 Request "
    "Time-out</h1></center>\r\n";

static const std::string webserv_error_409_page =
    "<html>\r\n<head><title>409 Conflict</title></head>\r\n<body>\r\n<center><h1>409 "
    "Conflict</h1></center>\r\n";

static const std::string webserv_error_410_page =
    "<html>\r\n<head><title>410 Gone</title></head>\r\n<body>\r\n<center><h1>410 Gone</h1></center>\r\n";

static const std::string webserv_error_411_page =
    "<html>\r\n<head><title>411 Length Required</title></head>\r\n<body>\r\n<center><h1>411 Length "
    "Required</h1></center>\r\n";

static const std::string webserv_error_412_page =
    "<html>\r\n<head><title>412 Precondition Failed</title></head>\r\n<body>\r\n<center><h1>412 Precondition "
    "Failed</h1></center>\r\n";

static const std::string webserv_error_413_page =
    "<html>\r\n<head><title>413 Request Entity Too Large</title></head>\r\n<body>\r\n<center><h1>413 Request "
    "Entity Too Large</h1></center>\r\n";

static const std::string webserv_error_414_page =
    "<html>\r\n<head><title>414 Request-URI Too Large</title></head>\r\n<body>\r\n<center><h1>414 "
    "Request-URI Too Large</h1></center>\r\n";

static const std::string webserv_error_415_page =
    "<html>\r\n<head><title>415 Unsupported Media Type</title></head>\r\n<body>\r\n<center><h1>415 "
    "Unsupported Media Type</h1></center>\r\n";

static const std::string webserv_error_416_page =
    "<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body>\r\n<center><h1>416 "
    "Requested Range Not Satisfiable</h1></center>\r\n";

static const std::string webserv_error_421_page =
    "<html>\r\n<head><title>421 Misdirected Request</title></head>\r\n<body>\r\n<center><h1>421 Misdirected "
    "Request</h1></center>\r\n";

static const std::string webserv_error_429_page =
    "<html>\r\n<head><title>429 Too Many Requests</title></head>\r\n<body>\r\n<center><h1>429 Too Many "
    "Requests</h1></center>\r\n";

static const std::string webserv_error_494_page =
    "<html>\r\n<head><title>494 Request Header Or Cookie Too "
    "Large</title></head>\r\n<body>\r\n<center><h1>494 Bad Request</h1></center>\r\n<center>Request Header "
    "Or Cookie Too Large</center>\r\n";

static const std::string webserv_error_495_page =
    "<html>\r\n<head><title>495 The SSL certificate error</title></head>\r\n<body>\r\n<center><h1>495 Bad "
    "Request</h1></center>\r\n<center>The SSL certificate error</center>\r\n";

static const std::string webserv_error_496_page =
    "<html>\r\n<head><title>496 No required SSL certificate was "
    "sent</title></head>\r\n<body>\r\n<center><h1>496 Bad Request</h1></center>\r\n<center>No required SSL "
    "certificate was sent</center>\r\n";

static const std::string webserv_error_497_page =
    "<html>\r\n<head><title>497 The plain HTTP request was sent to HTTPS "
    "port</title></head>\r\n<body>\r\n<center><h1>497 Bad Request</h1></center>\r\n<center>The plain HTTP "
    "request was sent to HTTPS port</center>\r\n";

static const std::string webserv_error_500_page =
    "<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal "
    "Server Error</h1></center>\r\n";

static const std::string webserv_error_501_page =
    "<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body>\r\n<center><h1>501 Not "
    "Implemented</h1></center>\r\n";

static const std::string webserv_error_502_page =
    "<html>\r\n<head><title>502 Bad Gateway</title></head>\r\n<body>\r\n<center><h1>502 Bad "
    "Gateway</h1></center>\r\n";

static const std::string webserv_error_503_page =
    "<html>\r\n<head><title>503 Service Temporarily Unavailable</title></head>\r\n<body>\r\n<center><h1>503 "
    "Service Temporarily Unavailable</h1></center>\r\n";

static const std::string webserv_error_504_page =
    "<html>\r\n<head><title>504 Gateway Time-out</title></head>\r\n<body>\r\n<center><h1>504 Gateway "
    "Time-out</h1></center>\r\n";

static const std::string webserv_error_505_page =
    "<html>\r\n<head><title>505 HTTP Version Not Supported</title></head>\r\n<body>\r\n<center><h1>505 HTTP "
    "Version Not Supported</h1></center>\r\n";

static const std::string webserv_error_507_page =
    "<html>\r\n<head><title>507 Insufficient Storage</title></head>\r\n<body>\r\n<center><h1>507 "
    "Insufficient Storage</h1></center>\r\n";

HttpResponse::HttpResponse()
    : root_path_(""),
      res_file_path_(""),
      state_(HttpResponse::RES_CREATING_STATIC),
      status_code_line_(""),
      status_code_(200),
      body_(""),
      internal_redirect_cnt_(0) {
  // status_line
  this->status_line_map_[200] = "200 OK";
  this->status_line_map_[201] = "201 Created";
  this->status_line_map_[202] = "202 Accepted";
  this->status_line_map_[203] = "203 Non-Authoritative Information";
  this->status_line_map_[204] = "204 No Content";
  this->status_line_map_[205] = "205 Reset Content";
  this->status_line_map_[206] = "206 Partial Content";
  this->status_line_map_[207] = "207 Multi-Status";
  this->status_line_map_[300] = "300 Multiple Choices";
  this->status_line_map_[301] = "301 Moved Permanently";
  this->status_line_map_[302] = "302 Moved Temporarily";
  this->status_line_map_[303] = "303 See Other";
  this->status_line_map_[304] = "304 Not Modified";

  this->status_line_map_[307] = "307 Temporary Redirect";
  this->status_line_map_[308] = "308 Permanent Redirect";

  this->status_line_map_[400] = "400 Bad Request";
  this->status_line_map_[401] = "401 Unauthorized";
  this->status_line_map_[402] = "402 Payment Required";
  this->status_line_map_[403] = "403 Forbidden";
  this->status_line_map_[404] = "404 Not Found";
  this->status_line_map_[405] = "405 Not Allowed";
  this->status_line_map_[406] = "406 Not Acceptable";
  this->status_line_map_[407] = "407 Proxy Authentication Required";
  this->status_line_map_[408] = "408 Request Time-out";
  this->status_line_map_[409] = "409 Conflict";
  this->status_line_map_[410] = "410 Gone";
  this->status_line_map_[411] = "411 Length Required";
  this->status_line_map_[412] = "412 Precondition Failed";
  this->status_line_map_[413] = "413 Request Entity Too Large";
  this->status_line_map_[414] = "414 Request-URI Too Large";
  this->status_line_map_[415] = "415 Unsupported Media Type";
  this->status_line_map_[416] = "416 Requested Range Not Satisfiable";
  this->status_line_map_[417] = "417 Expectation Failed";
  this->status_line_map_[418] = "418 I'm a teapot";
  this->status_line_map_[419] = "419 unused";
  this->status_line_map_[420] = "420 unused";
  this->status_line_map_[421] = "421 Misdirected Request";
  this->status_line_map_[422] = "422 Unprocessable Entity";
  this->status_line_map_[423] = "423 Locked";
  this->status_line_map_[424] = "424 Failed Dependency";
  this->status_line_map_[425] = "425 unused";
  this->status_line_map_[426] = "426 Upgrade Required";
  this->status_line_map_[427] = "427 unused";
  this->status_line_map_[428] = "428 Precondition Required";
  this->status_line_map_[429] = "429 Too Many Requests";
  this->status_line_map_[430] = "430";

  this->status_line_map_[500] = "500 Internal Server Error";
  this->status_line_map_[501] = "501 Not Implemented";
  this->status_line_map_[502] = "502 Bad Gateway";
  this->status_line_map_[503] = "503 Service Temporarily Unavailable";
  this->status_line_map_[504] = "504 Gateway Time-out";
  this->status_line_map_[505] = "505 HTTP Version Not Supported";
  this->status_line_map_[506] = "506 Variant Also Negotiates";
  this->status_line_map_[507] = "507 Insufficient Storage";
  this->status_line_map_[508] = "508 unused";
  this->status_line_map_[509] = "509 unused";
  this->status_line_map_[510] = "510 Not Extended";

  // default error_page
  this->default_error_page_map_[301] = &webserv_error_301_page;
  this->default_error_page_map_[302] = &webserv_error_302_page;
  this->default_error_page_map_[303] = &webserv_error_303_page;
  this->default_error_page_map_[307] = &webserv_error_307_page;
  this->default_error_page_map_[308] = &webserv_error_308_page;

  this->default_error_page_map_[400] = &webserv_error_400_page;
  this->default_error_page_map_[401] = &webserv_error_401_page;
  this->default_error_page_map_[402] = &webserv_error_402_page;
  this->default_error_page_map_[403] = &webserv_error_403_page;
  this->default_error_page_map_[404] = &webserv_error_404_page;
  this->default_error_page_map_[405] = &webserv_error_405_page;
  this->default_error_page_map_[406] = &webserv_error_406_page;
  this->default_error_page_map_[408] = &webserv_error_408_page;
  this->default_error_page_map_[409] = &webserv_error_409_page;
  this->default_error_page_map_[410] = &webserv_error_410_page;
  this->default_error_page_map_[411] = &webserv_error_411_page;
  this->default_error_page_map_[412] = &webserv_error_412_page;
  this->default_error_page_map_[413] = &webserv_error_413_page;
  this->default_error_page_map_[414] = &webserv_error_414_page;
  this->default_error_page_map_[415] = &webserv_error_415_page;
  this->default_error_page_map_[416] = &webserv_error_416_page;
  this->default_error_page_map_[421] = &webserv_error_421_page;
  this->default_error_page_map_[429] = &webserv_error_429_page;

  this->default_error_page_map_[494] = &webserv_error_494_page;
  this->default_error_page_map_[495] = &webserv_error_495_page;
  this->default_error_page_map_[496] = &webserv_error_496_page;
  this->default_error_page_map_[497] = &webserv_error_497_page;
  this->default_error_page_map_[498] = &webserv_error_404_page;

  this->default_error_page_map_[500] = &webserv_error_500_page;
  this->default_error_page_map_[501] = &webserv_error_501_page;
  this->default_error_page_map_[502] = &webserv_error_502_page;
  this->default_error_page_map_[503] = &webserv_error_503_page;
  this->default_error_page_map_[504] = &webserv_error_504_page;
  this->default_error_page_map_[505] = &webserv_error_505_page;
  this->default_error_page_map_[507] = &webserv_error_507_page;
}

std::string HttpResponse::createCurrentGmtTime() {
  // 現在の時間を取得
  std::time_t currentTime = std::time(NULL);

  // 構造体 tm に変換
  std::tm* gmTime = std::gmtime(&currentTime);

  // 曜日の文字列
  const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  // 月の文字列
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  // 指定されたフォーマットで文字列に変換
  std::ostringstream oss;
  oss << days[gmTime->tm_wday] << ", " << std::setfill('0') << std::setw(2) << gmTime->tm_mday << " "
      << months[gmTime->tm_mon] << " " << 1900 + gmTime->tm_year << " " << std::setfill('0') << std::setw(2)
      << gmTime->tm_hour << ":" << std::setfill('0') << std::setw(2) << gmTime->tm_min << ":"
      << std::setfill('0') << std::setw(2) << gmTime->tm_sec << " GMT";

  return oss.str();
}

std::string HttpResponse::transformLetter(const std::string& key_str) {
  std::string result = key_str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);

  if (result.empty()) return result;

  result[0] = std::toupper(result[0]);
  for (size_t i = 1; i < result.size(); i++)
    if (result[i - 1] == '-') result[i] = std::toupper(result[i]);
  return result;
}

std::string HttpResponse::createResponse(const HttpResponse& response) {
  std::stringstream stream;

  // status line
  stream << http_version << " " << response.status_code_line_ << "\r\n";

  // headers
  // cgi responseの場合は、ヘッダーの大文字小文字変換をしないのもあるがどうしよう？
  // 　ex) Content-Type　Content-Length　は文字が整形される
  // 　Locationなどは整形されない ex) loCAtion

  // TODO: 以下の場合に、responseをchunkしたい
  // headerに Content-Lengthがない場合（主にcgiレスポンス）
  // headerに responseが長い場合、例えばbuffer size以上
  for (std::map<std::string, std::string>::const_iterator it = response.headers_.begin();
       it != response.headers_.end(); ++it)
    stream << transformLetter(it->first) << ": " << it->second << "\r\n";
  stream << "\r\n";

  // body
  stream << response.body_;
  return stream.str();
}

/*
 * HttpResponseオブジェクトを生成し、send用のresponseを生成する
 */
std::string HttpResponse::generateResponse(HttpRequest& request, HttpResponse& response,
                                           const struct TiedServer& tied_servers, const int client_sock,
                                           const ConfigHandler& config_handler) {
  const static char* kHost = "Host";
  // chunkなどでparse途中の場合。
  if (request.parseState == HttpRequest::PARSE_INPROGRESS) return std::string();

  const config::Server& server =
      config_handler.searchServerConfig(tied_servers, request.headers.find(kHost)->second);
  const config::Location* location = NULL;
  struct sockaddr_in client_addr;

  enum ResponsePhase phase = sw_start_phase;
  if (response.state_ == RES_PARSED_CGI)
    phase = sw_end_phase;
  else if (response.state_ == RES_CGI_ERROR) {
    phase = sw_error_page_phase;
    response.status_code_ = 502;  // bad gate error
  }

  while (phase != sw_end_phase) {
    switch (phase) {
      case sw_start_phase:
        config_handler.writeErrorLog("webserv: [debug] start phase\n");
        phase = sw_pre_search_location_phase;
        break;

      case sw_pre_search_location_phase:
        config_handler.writeErrorLog("webserv: [debug] pre search location phase\n");
        phase = handlePreSearchLocationPhase(request.parseState, response, client_sock, client_addr);
        break;

      case sw_search_location_phase:
        config_handler.writeErrorLog("webserv: [debug] search location phase\n");
        phase = handleSearchLocationPhase(response, request, server, &location, config_handler);
        if (location)
          config_handler.writeErrorLog("webserv: [debug] location found -> " + location->uri + "\n");
        break;

      case sw_post_search_location_phase:
        config_handler.writeErrorLog("webserv: [debug] post search location phase\n");
        response.root_path_ = config_handler.searchRootPath(server, location);
        config_handler.writeErrorLog("webserv: [debug] root path is " + response.root_path_ + "\n");
        phase = sw_return_phase;
        break;

      case sw_return_phase:
        config_handler.writeErrorLog("webserv: [debug] return phase\n");
        phase = handleReturnPhase(response, location, config_handler);
        break;

      case sw_allow_phase:
        config_handler.writeErrorLog("webserv: [debug] allow phase\n");
        phase = handleAllowPhase(response, request, server, location, client_addr, config_handler);
        break;

      case sw_uri_check_phase:
        config_handler.writeErrorLog("webserv: [debug] uri check phase\n");
        phase = handleUriCheckPhase(response, request, server, location);
        break;

      case sw_search_res_file_phase:
        config_handler.writeErrorLog("webserv: [debug] search response file phase\n");
        phase = handleSearchResFilePhase(response, request, server, location, config_handler);
        break;

      case sw_content_phase:
        config_handler.writeErrorLog("webserv: [debug] content phase\n");
        phase = handleContentPhase(response);
        break;

      case sw_error_page_phase:
        config_handler.writeErrorLog("webserv: [debug] error page phase\n");
        phase = handleErrorPagePhase(response, request, server, location, config_handler);
        break;

      case sw_log_phase:
        config_handler.writeErrorLog("webserv: [debug] log phase\n");
        //  TODO: cgi errorの場合、アクセスログを二回かきこまないようにする
        config_handler.writeAccessLog(
            server, location,
            config_handler.createAcsLogMsg(client_addr.sin_addr.s_addr, response.status_code_,
                                           response.body_.size(), request));
        phase = sw_end_phase;
        break;

      default:
        phase = sw_end_phase;
        break;
    }
  }

  if (response.state_ == RES_EXECUTE_CGI) return "";
  config_handler.writeErrorLog("webserv: [debug] header filter\n");
  headerFilterPhase(response,
                    config_handler.searchKeepaliveTimeout(tied_servers, request.headers[kHost], request.uri));

  config_handler.writeErrorLog("webserv: [debug] create final response\n");
  config_handler.writeErrorLog("webserv: [debug] final response file path " + response.res_file_path_ +
                               "\n\n");
  return createResponse(response);
}

HttpResponse::ResponsePhase HttpResponse::handlePreSearchLocationPhase(
    const HttpRequest::ParseState parse_state, HttpResponse& response, const int client_sock,
    struct sockaddr_in& client_addr) {
  // parse error
  if (parse_state == HttpRequest::PARSE_ERROR) {
    response.status_code_ = 400;
    return sw_error_page_phase;
  }

  // clientのip_addressを取る
  // retry するか？
  socklen_t client_addrlen = sizeof(client_addr);
  if (getsockname(client_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen) != 0) {
    std::cerr << "webserv: [emerge] getsockname() \"" << client_sock << "\" failed (" << errno << ": "
              << strerror(errno) << ")" << std::endl;
    // getsockname()ダメだったらどうするか？
    return sw_end_phase;
  } else
    return sw_search_location_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleSearchLocationPhase(HttpResponse& response,
                                                                    const HttpRequest& request,
                                                                    const config::Server& server,
                                                                    const config::Location** location,
                                                                    const ConfigHandler& config_handler) {
  if (response.internal_redirect_cnt_++ > kMaxInternalRedirect) {
    config_handler.writeErrorLog(server, *location, "webserv: [error] too continuous internal redirect\n");
    response.status_code_ = 500;
    response.body_ = *default_error_page_map_[500] + webserv_error_page_tail;
    response.res_file_path_ = kDefaultPage;
    return sw_end_phase;
  }
  *location = config_handler.searchLongestMatchLocationConfig(server, request.uri);
  if (*location) config_handler.writeErrorLog("webserv: [debug] a request access " + (*location)->uri + "\n");
  return sw_post_search_location_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleAllowPhase(HttpResponse& response, const HttpRequest& request,
                                                           const config::Server& server,
                                                           const config::Location* location,
                                                           struct sockaddr_in client_addr,
                                                           const ConfigHandler& config_handler) {
  int ret = config_handler.allowRequest(server, location, request, client_addr);
  if (ret == ConfigHandler::ACCESS_DENY) {
    response.status_code_ = 403;
    return sw_error_page_phase;
  } else if (ret == ConfigHandler::METHOD_DENY) {
    response.status_code_ = 405;
    return sw_error_page_phase;
  } else
    return sw_uri_check_phase;
}

void HttpResponse::prepareReturn(HttpResponse& response, const config::Return& return_directive) {
  std::string url = return_directive.getUrl();
  int code = return_directive.getCode();
  const char* kLocation = "Location";

  if (code == config::Return::kCodeUnset) {
    response.status_code_ = 302;
    response.headers_[kLocation] = url;
    return;
  }
  if (config::Return::isRedirectCode(code)) {
    response.status_code_ = code;
    response.headers_[kLocation] = url;
    return;
  }
  // textの場合
  response.status_code_ = code;
  if (!url.empty()) response.body_ = url;
}

HttpResponse::ResponsePhase HttpResponse::handleReturnPhase(HttpResponse& response,
                                                            const config::Location* location,
                                                            const ConfigHandler& config_handler) {
  if (!location || location->directives_set.find(kRETURN) == location->directives_set.end())
    return sw_allow_phase;

  prepareReturn(response, location->return_list[0]);
  config_handler.writeErrorLog("webserv: [debug] redirect occured\n");
  return sw_error_page_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleUriCheckPhase(HttpResponse& response,
                                                              const HttpRequest& request,
                                                              const config::Server& server,
                                                              const config::Location* location) {
  // uriが'/'で終わってない、かつdirectoryであるとき301MovedPermanently
  if (lastChar(request.uri) != '/' && Utils::isDirectory(server.root.getPath() + request.uri)) {
    response.status_code_ = 301;
    return sw_error_page_phase;
  } else if (lastChar(request.uri) == '/' && request.uri != "/" && !location) {
    if (response.internal_redirect_cnt_ > 1)
      response.status_code_ = 500;
    else
      response.status_code_ = 404;
    return sw_error_page_phase;
  }
  return sw_search_res_file_phase;
}

/* try_files
 *
 * fileが見つかればbodyにセットして返す。
 * codeならErrorPage探すように返す。
 * URIなら内部リダイレクト
 */
HttpResponse::ResponsePhase HttpResponse::TryFiles(HttpResponse& response, HttpRequest& request,
                                                   const config::TryFiles& try_files) {
  std::vector<std::string> file_list = try_files.getFileList();

  for (size_t i = 0; i < file_list.size(); i++) {
    std::string full_path = response.root_path_ + file_list[i];
    if (Utils::wrapperAccess(full_path, F_OK, false) == 0 &&
        Utils::wrapperAccess(full_path, R_OK, false) == 0) {
      response.res_file_path_ = full_path;
      return sw_content_phase;
    }
  }

  // uri
  if (try_files.getCode() == config::TryFiles::kCodeUnset) {
    request.uri = try_files.getUri();
    return sw_search_location_phase;
  }
  // code
  response.status_code_ = try_files.getCode();
  return sw_error_page_phase;
}

std::string HttpResponse::autoIndex(const std::string& directory_path, const std::string& index_dir) {
  std::vector<std::string> contents = Utils::createDirectoryContents(directory_path);
  // もしディレクトリが存在しなければ空文字を返す。
  if (contents.empty()) return std::string("");

  std::stringstream buffer;

  /*
   * header
   */
  buffer << "<!DOCTYPE html>";
  buffer << "<html>";
  buffer << "<head>";
  buffer << "<meta charset=\"utf-8\"><title>Index of " << index_dir << "</title>";
  // css
  buffer << "<style>";
  buffer << ".right-align {position: absolute;right: 0;}";  // 親要素の中での位置を右寄せにする。
  buffer << "</style>";
  buffer << "</head>\r\n";

  /*
   * directory listing
   */
  buffer << "<body>";
  buffer << "<h1>Index of " << index_dir << "</h1>";
  buffer << "<hr>";
  buffer << "<pre>";

  // 要素ごとにリンクと最終修正時刻とサイズを出力
  for (std::vector<std::string>::iterator it = contents.begin(); it != contents.end(); ++it) {
    buffer << "<a href='";
    if (!directory_path.empty() && directory_path[directory_path.size() - 1] != '/') buffer << "/";
    buffer << *it << "'>" << *it << "</a>";

    struct stat file_stat;
    std::string full_path = directory_path + *it;
    if (*it != "../" && stat(full_path.c_str(), &file_stat) == 0) {
      buffer << "<span class=\"right-align\">";
      // file 最終修正時刻
      struct tm last_modify_time;
      localtime_r(&file_stat.st_mtime, &last_modify_time);
      char date[1024];
      std::strftime(date, sizeof(date), "%d-%b-%Y %H:%M", &last_modify_time);
      buffer << date;

      // ファイルバイト数
      std::string space;
      if (S_ISREG(file_stat.st_mode)) {
        std::stringstream ss;
        ss << file_stat.st_size;
        for (size_t i = 0; i < 15 - ss.str().length() + 1; i++) space += " ";
        buffer << space << file_stat.st_size << "  ";
      } else {
        for (size_t i = 0; i < 15; i++) space += " ";
        buffer << space << "-"
               << "  ";
      }
      buffer << "</span>";
    }

    buffer << "\r\n";
  }
  buffer << "</pre>";

  /*
   * footer
   */
  buffer << "<hr>";
  buffer << "</body></html>";
  return buffer.str();
}

HttpResponse::ResponsePhase HttpResponse::Index(HttpResponse& response, HttpRequest& request,
                                                const std::vector<config::Index>& index_list,
                                                bool is_autoindex_on, const std::string& index_dir) {
  std::string directory_path = response.root_path_ + request.uri;
  for (size_t i = 0; i < index_list.size(); i++) {
    std::string full_path = directory_path + index_list[i].getFile();
    if (Utils::wrapperAccess(full_path, F_OK, false) == 0 ||
        Utils::wrapperAccess(full_path, R_OK, false) == 0) {
      response.res_file_path_ = full_path;
      return sw_content_phase;
    }
  }

  if (is_autoindex_on) {
    response.body_ = autoIndex(directory_path, index_dir);
    // autoindexでディレクトリが見つからなかったら404エラー
    if (response.body_.empty()) {
      response.status_code_ = 404;
      return sw_error_page_phase;
    }
    response.res_file_path_ = kDefaultPage;
    return sw_log_phase;
  }

  // autoindex offなら403
  response.status_code_ = 403;
  return sw_error_page_phase;
}

/*
 * uriが'/'で終わっていなければ直接探しに行き、
 * そうでなければ、ディレクティブを順番に適用する。
 */
HttpResponse::ResponsePhase HttpResponse::searchResPath(HttpResponse& response, HttpRequest& request,
                                                        const config::Server& server,
                                                        const config::Location* location,
                                                        const ConfigHandler& config_handler) {
  // request uriが/で終わっていなければ直接ファイルを探しに行く。
  if (lastChar(request.uri) != '/') {
    std::string full_path = response.root_path_ + request.uri;
    if (Utils::wrapperAccess(full_path, F_OK, false) != 0 ||
        Utils::wrapperAccess(full_path, R_OK, false) != 0) {
      response.status_code_ = 404;
      return sw_error_page_phase;
    }
    response.res_file_path_ = full_path;
    return sw_content_phase;
  }

  /* ~ try_filesとindex/autoindexのファイル検索 ~
   * try_filesはlocationのuriを探すファイルのルートにいれずに内部リダイレクト
   * index/autoindex はrequestのuriにindexのファイル名を足して探す
   * 3つともなかったら上位のcontextで検索する
   */
  bool is_autoindex_on = config_handler.isAutoIndexOn(server, location);
  std::string index_dir;
  if (location)
    index_dir = location->uri;
  else
    index_dir = "/";

  // location context
  if (location && location->directives_set.find(kTRY_FILES) != location->directives_set.end())
    return TryFiles(response, request, location->try_files);
  else if (location && location->directives_set.find(kINDEX) != location->directives_set.end())
    return Index(response, request, location->index_list, is_autoindex_on, index_dir);

  // server context
  if (server.directives_set.find(kTRY_FILES) != server.directives_set.end())
    return TryFiles(response, request, server.try_files);
  else if (server.directives_set.find(kINDEX) != server.directives_set.end())
    return Index(response, request, server.index_list, is_autoindex_on, index_dir);

  // http contextにindexディレクティブがあればその設定値をみるし、
  // なくとも、デフォルトのindexディレクティブを見る
  return Index(response, request, config_handler.config_->http.index_list, is_autoindex_on, index_dir);
}

/**
 * respone fileのpathをさがす
 */
HttpResponse::ResponsePhase HttpResponse::handleSearchResFilePhase(HttpResponse& response,
                                                                   HttpRequest& request,
                                                                   const config::Server& server,
                                                                   const config::Location* location,
                                                                   const ConfigHandler& config_handler) {
  return searchResPath(response, request, server, location, config_handler);
}

HttpResponse::ResponsePhase HttpResponse::handleContentPhase(HttpResponse& response) {
  if (cgi::CGIHandler::isCgi(response.res_file_path_)) {
    response.state_ = RES_EXECUTE_CGI;
    return sw_log_phase;
  }
  response.body_ = Utils::readFile(response.res_file_path_);
  return sw_log_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleErrorPagePhase(HttpResponse& response, HttpRequest& request,
                                                               const config::Server& server,
                                                               const config::Location* location,
                                                               const ConfigHandler& config_handler) {
  const config::ErrorPage* ep = config_handler.searchErrorPage(server, location, response.status_code_);

  if (response.body_.empty() &&
      default_error_page_map_.find(response.status_code_) != default_error_page_map_.end()) {
    response.res_file_path_ = kDefaultPage;
    response.body_ = *default_error_page_map_[response.status_code_] + webserv_error_page_tail;
  }
  if (!ep) return sw_log_phase;

  // error page process
  long tmp_code;  // error_pageの=responseはlong_maxまで許容
  if ((tmp_code = ep->getResponse()) != config::ErrorPage::kResponseUnset) response.status_code_ = tmp_code;
  request.uri = ep->getUri();
  return sw_search_location_phase;
}

void HttpResponse::headerFilterPhase(HttpResponse& response, const config::Time& time) {
  const static char* kClose = "close";
  const static char* kConnection = "Connection";
  const static char* kKeepAlive = "keep-alive";
  const static char* kTransferEncoding = "Transfer-Encoding";
  const std::map<int, std::string>::iterator default_status_line =
      status_line_map_.find(response.status_code_);

  response.headers_["Server"] = "webserv/1.0";
  response.headers_["Date"] = createCurrentGmtTime();
  response.headers_["Content-Length"] = Utils::toStr(response.body_.size());
  // conetent-lengthで対応するので、Transfer-Encodingは削除する
  if (response.headers_.find(kTransferEncoding) != response.headers_.end())
    response.headers_.erase(kTransferEncoding);

  // requestエラーの場合は、接続を切る
  if (time.isNoTime() || (400 <= response.status_code_ && response.status_code_ < 500))
    response.headers_[kConnection] = kClose;
  else
    response.headers_[kConnection] = kKeepAlive;

  // cgiでstatus code lineの場合
  if (!response.status_code_line_.empty()) return;
  if (default_status_line != status_line_map_.end())
    response.status_code_line_ = default_status_line->second;
  else
    response.status_code_line_ = Utils::toStr(response.status_code_);
  // staticのファイルの場合のみ、contetn-typeをつけてあげる
  if (response.state_ != RES_CREATING_STATIC) return;

  if (response.headers_.find(kContentType) == response.headers_.end())
    response.headers_[kContentType] = detectContentType(response.res_file_path_);
}

std::string HttpResponse::detectContentType(const std::string& res_file_path) {
  const char* kHtmlExt = ".html";
  const char* kCssExt = ".css";
  const char* kJsExt = ".js";

  if (Utils::isExtensionFile(res_file_path, kHtmlExt)) return kHtml;
  if (Utils::isExtensionFile(res_file_path, kCssExt)) return "text/css";
  if (Utils::isExtensionFile(res_file_path, kJsExt)) return "text/javascript";
  return kTextPlain;
}

char HttpResponse::lastChar(const std::string& str) { return str[str.size() - 1]; }
