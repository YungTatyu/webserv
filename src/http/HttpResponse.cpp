#include "HttpResponse.hpp"

#include <cstring>
#include <ctime>
#include <iomanip>

#include "CgiHandler.hpp"
#include "syscall_wrapper.hpp"
#include "utils.hpp"

static const char* kAlias = "alias";
static const char* kTryFiles = "try_files";
static const char* kIndex = "index";
static const char* kReturn = "return";
static const char* kContentType = "Content-Type";
static const char* kHtml = "text/html";
static const char* kTextPlain = "text/plain";
static const char* kDefaultPage = "defaut.html";
static const int kInitStatusCode = 200;
static const char* kClose = "close";
static const char* kConnection = "Connection";
static const char* kHost = "host";
static const char* kKeepAlive = "keep-alive";
static const char* kLocation = "Location";
static const char* kScheme = "http://";
static const char* kTransferEncoding = "Transfer-Encoding";

std::map<int, const char*> HttpResponse::status_line_map_;
std::map<int, const char*> HttpResponse::default_error_page_map_;

static const char* http_version = "HTTP/1.1";

static const char* webserv_error_page_tail = "<hr><center>webserv/1.0</center>\r\n</body>\r\n</html>\r\n";

static const char* webserv_error_301_page =
    "<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved "
    "Permanently</h1></center>\r\n";

static const char* webserv_error_302_page =
    "<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n";

static const char* webserv_error_303_page =
    "<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See "
    "Other</h1></center>\r\n";

static const char* webserv_error_307_page =
    "<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary "
    "Redirect</h1></center>\r\n";

static const char* webserv_error_308_page =
    "<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent "
    "Redirect</h1></center>\r\n";

static const char* webserv_error_400_page =
    "<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad "
    "Request</h1></center>\r\n";

static const char* webserv_error_401_page =
    "<html>\r\n<head><title>401 Authorization Required</title></head>\r\n<body>\r\n<center><h1>401 "
    "Authorization Required</h1></center>\r\n";

static const char* webserv_error_402_page =
    "<html>\r\n<head><title>402 Payment Required</title></head>\r\n<body>\r\n<center><h1>402 Payment "
    "Required</h1></center>\r\n";

static const char* webserv_error_403_page =
    "<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 "
    "Forbidden</h1></center>\r\n";

static const char* webserv_error_404_page =
    "<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not "
    "Found</h1></center>\r\n";

static const char* webserv_error_405_page =
    "<html>\r\n<head><title>405 Not Allowed</title></head>\r\n<body>\r\n<center><h1>405 Not "
    "Allowed</h1></center>\r\n";

static const char* webserv_error_406_page =
    "<html>\r\n<head><title>406 Not Acceptable</title></head>\r\n<body>\r\n<center><h1>406 Not "
    "Acceptable</h1></center>\r\n";

static const char* webserv_error_408_page =
    "<html>\r\n<head><title>408 Request Time-out</title></head>\r\n<body>\r\n<center><h1>408 Request "
    "Time-out</h1></center>\r\n";

static const char* webserv_error_409_page =
    "<html>\r\n<head><title>409 Conflict</title></head>\r\n<body>\r\n<center><h1>409 "
    "Conflict</h1></center>\r\n";

static const char* webserv_error_410_page =
    "<html>\r\n<head><title>410 Gone</title></head>\r\n<body>\r\n<center><h1>410 Gone</h1></center>\r\n";

static const char* webserv_error_411_page =
    "<html>\r\n<head><title>411 Length Required</title></head>\r\n<body>\r\n<center><h1>411 Length "
    "Required</h1></center>\r\n";

static const char* webserv_error_412_page =
    "<html>\r\n<head><title>412 Precondition Failed</title></head>\r\n<body>\r\n<center><h1>412 Precondition "
    "Failed</h1></center>\r\n";

static const char* webserv_error_413_page =
    "<html>\r\n<head><title>413 Request Entity Too Large</title></head>\r\n<body>\r\n<center><h1>413 Request "
    "Entity Too Large</h1></center>\r\n";

static const char* webserv_error_414_page =
    "<html>\r\n<head><title>414 Request-URI Too Large</title></head>\r\n<body>\r\n<center><h1>414 "
    "Request-URI Too Large</h1></center>\r\n";

static const char* webserv_error_415_page =
    "<html>\r\n<head><title>415 Unsupported Media Type</title></head>\r\n<body>\r\n<center><h1>415 "
    "Unsupported Media Type</h1></center>\r\n";

static const char* webserv_error_416_page =
    "<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body>\r\n<center><h1>416 "
    "Requested Range Not Satisfiable</h1></center>\r\n";

static const char* webserv_error_421_page =
    "<html>\r\n<head><title>421 Misdirected Request</title></head>\r\n<body>\r\n<center><h1>421 Misdirected "
    "Request</h1></center>\r\n";

static const char* webserv_error_429_page =
    "<html>\r\n<head><title>429 Too Many Requests</title></head>\r\n<body>\r\n<center><h1>429 Too Many "
    "Requests</h1></center>\r\n";

static const char* webserv_error_494_page =
    "<html>\r\n<head><title>494 Request Header Or Cookie Too "
    "Large</title></head>\r\n<body>\r\n<center><h1>494 Bad Request</h1></center>\r\n<center>Request Header "
    "Or Cookie Too Large</center>\r\n";

static const char* webserv_error_495_page =
    "<html>\r\n<head><title>495 The SSL certificate error</title></head>\r\n<body>\r\n<center><h1>495 Bad "
    "Request</h1></center>\r\n<center>The SSL certificate error</center>\r\n";

static const char* webserv_error_496_page =
    "<html>\r\n<head><title>496 No required SSL certificate was "
    "sent</title></head>\r\n<body>\r\n<center><h1>496 Bad Request</h1></center>\r\n<center>No required SSL "
    "certificate was sent</center>\r\n";

static const char* webserv_error_497_page =
    "<html>\r\n<head><title>497 The plain HTTP request was sent to HTTPS "
    "port</title></head>\r\n<body>\r\n<center><h1>497 Bad Request</h1></center>\r\n<center>The plain HTTP "
    "request was sent to HTTPS port</center>\r\n";

static const char* webserv_error_500_page =
    "<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal "
    "Server Error</h1></center>\r\n";

static const char* webserv_error_501_page =
    "<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body>\r\n<center><h1>501 Not "
    "Implemented</h1></center>\r\n";

static const char* webserv_error_502_page =
    "<html>\r\n<head><title>502 Bad Gateway</title></head>\r\n<body>\r\n<center><h1>502 Bad "
    "Gateway</h1></center>\r\n";

static const char* webserv_error_503_page =
    "<html>\r\n<head><title>503 Service Temporarily Unavailable</title></head>\r\n<body>\r\n<center><h1>503 "
    "Service Temporarily Unavailable</h1></center>\r\n";

static const char* webserv_error_504_page =
    "<html>\r\n<head><title>504 Gateway Time-out</title></head>\r\n<body>\r\n<center><h1>504 Gateway "
    "Time-out</h1></center>\r\n";

static const char* webserv_error_505_page =
    "<html>\r\n<head><title>505 HTTP Version Not Supported</title></head>\r\n<body>\r\n<center><h1>505 HTTP "
    "Version Not Supported</h1></center>\r\n";

static const char* webserv_error_507_page =
    "<html>\r\n<head><title>507 Insufficient Storage</title></head>\r\n<body>\r\n<center><h1>507 "
    "Insufficient Storage</h1></center>\r\n";

HttpResponse::HttpResponse()
    : root_path_(""),
      res_file_path_(""),
      path_info_(""),
      state_(HttpResponse::RES_CREATING_STATIC),
      status_code_line_(""),
      status_code_(kInitStatusCode),
      body_(""),
      internal_redirect_cnt_(0) {}

HttpResponse::HttpResponse(const HttpResponse& other) { *this = other; }

HttpResponse::~HttpResponse() {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
  if (this != &other) {
    this->root_path_ = other.root_path_;
    this->res_file_path_ = other.res_file_path_;
    this->path_info_ = other.path_info_;
    this->state_ = other.state_;
    this->status_code_line_ = other.status_code_line_;
    this->status_code_ = other.status_code_;
    this->headers_ = other.headers_;
    this->body_ = other.body_;
    this->internal_redirect_cnt_ = other.internal_redirect_cnt_;
  }
  return *this;
}

/**
 * @brief HttpResponseのstaticメンバ変数を初期化する
 */
void HttpResponse::setup() {
  // status_line
  status_line_map_[200] = "200 OK";
  status_line_map_[201] = "201 Created";
  status_line_map_[202] = "202 Accepted";
  status_line_map_[203] = "203 Non-Authoritative Information";
  status_line_map_[204] = "204 No Content";
  status_line_map_[205] = "205 Reset Content";
  status_line_map_[206] = "206 Partial Content";
  status_line_map_[207] = "207 Multi-Status";
  status_line_map_[300] = "300 Multiple Choices";
  status_line_map_[301] = "301 Moved Permanently";
  status_line_map_[302] = "302 Moved Temporarily";
  status_line_map_[303] = "303 See Other";
  status_line_map_[304] = "304 Not Modified";

  status_line_map_[307] = "307 Temporary Redirect";
  status_line_map_[308] = "308 Permanent Redirect";

  status_line_map_[400] = "400 Bad Request";
  status_line_map_[401] = "401 Unauthorized";
  status_line_map_[402] = "402 Payment Required";
  status_line_map_[403] = "403 Forbidden";
  status_line_map_[404] = "404 Not Found";
  status_line_map_[405] = "405 Not Allowed";
  status_line_map_[406] = "406 Not Acceptable";
  status_line_map_[407] = "407 Proxy Authentication Required";
  status_line_map_[408] = "408 Request Time-out";
  status_line_map_[409] = "409 Conflict";
  status_line_map_[410] = "410 Gone";
  status_line_map_[411] = "411 Length Required";
  status_line_map_[412] = "412 Precondition Failed";
  status_line_map_[413] = "413 Request Entity Too Large";
  status_line_map_[414] = "414 Request-URI Too Large";
  status_line_map_[415] = "415 Unsupported Media Type";
  status_line_map_[416] = "416 Requested Range Not Satisfiable";
  status_line_map_[417] = "417 Expectation Failed";
  status_line_map_[418] = "418 I'm a teapot";
  status_line_map_[419] = "419 unused";
  status_line_map_[420] = "420 unused";
  status_line_map_[421] = "421 Misdirected Request";
  status_line_map_[422] = "422 Unprocessable Entity";
  status_line_map_[423] = "423 Locked";
  status_line_map_[424] = "424 Failed Dependency";
  status_line_map_[425] = "425 unused";
  status_line_map_[426] = "426 Upgrade Required";
  status_line_map_[427] = "427 unused";
  status_line_map_[428] = "428 Precondition Required";
  status_line_map_[429] = "429 Too Many Requests";
  status_line_map_[430] = "430";

  status_line_map_[500] = "500 Internal Server Error";
  status_line_map_[501] = "501 Not Implemented";
  status_line_map_[502] = "502 Bad Gateway";
  status_line_map_[503] = "503 Service Temporarily Unavailable";
  status_line_map_[504] = "504 Gateway Time-out";
  status_line_map_[505] = "505 HTTP Version Not Supported";
  status_line_map_[506] = "506 Variant Also Negotiates";
  status_line_map_[507] = "507 Insufficient Storage";
  status_line_map_[508] = "508 unused";
  status_line_map_[509] = "509 unused";
  status_line_map_[510] = "510 Not Extended";

  // default error_page
  default_error_page_map_[301] = webserv_error_301_page;
  default_error_page_map_[302] = webserv_error_302_page;
  default_error_page_map_[303] = webserv_error_303_page;
  default_error_page_map_[307] = webserv_error_307_page;
  default_error_page_map_[308] = webserv_error_308_page;

  default_error_page_map_[400] = webserv_error_400_page;
  default_error_page_map_[401] = webserv_error_401_page;
  default_error_page_map_[402] = webserv_error_402_page;
  default_error_page_map_[403] = webserv_error_403_page;
  default_error_page_map_[404] = webserv_error_404_page;
  default_error_page_map_[405] = webserv_error_405_page;
  default_error_page_map_[406] = webserv_error_406_page;
  default_error_page_map_[408] = webserv_error_408_page;
  default_error_page_map_[409] = webserv_error_409_page;
  default_error_page_map_[410] = webserv_error_410_page;
  default_error_page_map_[411] = webserv_error_411_page;
  default_error_page_map_[412] = webserv_error_412_page;
  default_error_page_map_[413] = webserv_error_413_page;
  default_error_page_map_[414] = webserv_error_414_page;
  default_error_page_map_[415] = webserv_error_415_page;
  default_error_page_map_[416] = webserv_error_416_page;
  default_error_page_map_[421] = webserv_error_421_page;
  default_error_page_map_[429] = webserv_error_429_page;

  default_error_page_map_[494] = webserv_error_494_page;
  default_error_page_map_[495] = webserv_error_495_page;
  default_error_page_map_[496] = webserv_error_496_page;
  default_error_page_map_[497] = webserv_error_497_page;
  default_error_page_map_[498] = webserv_error_404_page;

  default_error_page_map_[500] = webserv_error_500_page;
  default_error_page_map_[501] = webserv_error_501_page;
  default_error_page_map_[502] = webserv_error_502_page;
  default_error_page_map_[503] = webserv_error_503_page;
  default_error_page_map_[504] = webserv_error_504_page;
  default_error_page_map_[505] = webserv_error_505_page;
  default_error_page_map_[507] = webserv_error_507_page;
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

std::string HttpResponse::createResponse(config::REQUEST_METHOD method) const {
  std::stringstream stream;

  // status line
  stream << http_version << " " << this->status_code_line_ << "\r\n";

  // headers
  // cgi responseの場合は、ヘッダーの大文字小文字変換をしないのもあるがどうしよう？
  // 　ex) Content-Type　Content-Length　は文字が整形される
  // 　Locationなどは整形されない ex) loCAtion

  for (std::map<std::string, std::string>::const_iterator it = this->headers_.begin();
       it != this->headers_.end(); ++it)
    stream << transformLetter(it->first) << ": " << it->second << "\r\n";
  stream << "\r\n";
  // HEAD methodか204 status codeだったらbodyを含めない
  if (method == config::HEAD || this->status_code_ == 204) return stream.str();

  // body
  stream << this->body_;
  return stream.str();
}

/*
 * HttpResponseオブジェクトを生成し、send用のresponseを生成する
 */
std::string HttpResponse::generateResponse(HttpRequest& request, HttpResponse& response,
                                           const struct TiedServer& tied_servers, int socket,
                                           const ConfigHandler& config_handler) {
  // chunkなどでparse途中の場合。
  if (request.parse_state_ == HttpRequest::PARSE_INPROGRESS) return std::string();

  // requestにホストヘッダーがない時は、空文字のホストヘッダーを追加する。
  // この場合、まずserver_nameが空文字列のサーバーが適用される。
  // それがなければ、デフォルトサーバーの設定が適用される。
  if (request.headers_.find(kHost) == request.headers_.end()) {
    request.headers_[kHost] = "";
  }

  const config::Server& server =
      config_handler.searchServerConfig(tied_servers, request.headers_.find(kHost)->second);
  const config::Location* location = NULL;
  struct sockaddr_in client_addr;
  enum ResponsePhase phase = sw_start_phase;

  while (phase != sw_end_phase) {
    switch (phase) {
      case sw_start_phase:
        config_handler.writeErrorLog("start phase", config::DEBUG);
        phase = sw_pre_search_location_phase;
        break;

      case sw_pre_search_location_phase:
        config_handler.writeErrorLog("pre search location phase", config::DEBUG);
        phase = handlePreSearchLocationPhase(request.parse_state_, response, socket, client_addr);
        break;

      case sw_search_location_phase:
        config_handler.writeErrorLog("search location phase", config::DEBUG);
        phase = handleSearchLocationPhase(response, request, server, &location, config_handler);
        if (location) config_handler.writeErrorLog("location found -> " + location->uri_, config::DEBUG);
        break;

      case sw_post_search_location_phase:
        config_handler.writeErrorLog("post search location phase", config::DEBUG);
        response.root_path_ = config_handler.searchRootPath(server, location);
        config_handler.writeErrorLog("root path is " + response.root_path_, config::DEBUG);
        phase = sw_return_phase;
        break;

      case sw_return_phase:
        config_handler.writeErrorLog("return phase", config::DEBUG);
        phase = handleReturnPhase(response, location, config_handler);
        break;

      case sw_allow_phase:
        config_handler.writeErrorLog("allow phase", config::DEBUG);
        phase = handleReturnPhase(response, location, config_handler);
        phase = handleAllowPhase(response, request, server, location, client_addr, config_handler);
        break;

      case sw_uri_check_phase:
        config_handler.writeErrorLog("uri check phase", config::DEBUG);
        phase = handleUriCheckPhase(response, request, location, tied_servers.port_);
        break;

      case sw_search_res_file_phase:
        config_handler.writeErrorLog("search response file phase", config::DEBUG);
        phase = handleSearchResFilePhase(response, request, server, location, config_handler);
        break;

      case sw_content_phase:
        config_handler.writeErrorLog("content phase", config::DEBUG);
        phase = handleContentPhase(response, request);
        break;

      case sw_error_page_phase:
        config_handler.writeErrorLog("error page phase", config::DEBUG);
        phase = handleErrorPagePhase(response, request, server, location, config_handler);
        break;

      case sw_log_phase:
        config_handler.writeErrorLog("log phase", config::DEBUG);
        config_handler.writeAccessLog(
            server, location,
            config_handler.createAcsLogMsg(client_addr.sin_addr.s_addr, response.getStatusCode(),
                                           response.body_.size(), request));
        phase = sw_end_phase;
        break;

      default:
        phase = sw_end_phase;
        break;
    }
  }

  if (response.state_ == RES_EXECUTE_CGI) return "";
  config_handler.writeErrorLog("header filter", config::DEBUG);
  headerFilterPhase(
      response, config_handler.searchKeepaliveTimeout(tied_servers, request.headers_[kHost], request.uri_));

  config_handler.writeErrorLog("create final response", config::DEBUG);
  config_handler.writeErrorLog("final response file path " + response.res_file_path_, config::DEBUG);
  response.state_ = RES_COMPLETE;
  return response.createResponse(request.method_);
}

HttpResponse::ResponsePhase HttpResponse::handlePreSearchLocationPhase(HttpRequest::ParseState parse_state,
                                                                       HttpResponse& response, int socket,
                                                                       struct sockaddr_in& client_addr) {
  switch (response.state_) {
    case RES_COMPLETE:
      response.clear();
      break;
    case RES_PARSED_CGI:
      return sw_end_phase;
    case RES_CGI_EXIT_FAILURE:  // cgi exited non zero value
      response.setStatusCode(500);
      return sw_error_page_phase;
    case RES_CGI_ERROR:  // fork childp error or parse error
      response.setStatusCode(502);
      return sw_error_page_phase;
    case RES_CGI_TIMEOUT:  // cgi timeout
      response.setStatusCode(504);
      return sw_error_page_phase;
    default:
      break;
  }

  switch (parse_state) {
    case HttpRequest::PARSE_ERROR:
      response.setStatusCode(400);
      return sw_error_page_phase;
    case HttpRequest::PARSE_NOT_IMPLEMENTED:
      response.setStatusCode(501);
      return sw_error_page_phase;
    case HttpRequest::PARSE_ERROR_BODY_TOO_LARGE:
      response.setStatusCode(413);
      return sw_error_page_phase;
    default:
      break;
  }

  // get client ip_address
  if (!utils::resolveSocketAddr(socket, client_addr)) {
    // client のアドレスを取得できなかったら403エラーを返す。
    response.setStatusCode(403);
    return sw_error_page_phase;
  }
  return sw_search_location_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleSearchLocationPhase(HttpResponse& response,
                                                                    const HttpRequest& request,
                                                                    const config::Server& server,
                                                                    const config::Location** location,
                                                                    const ConfigHandler& config_handler) {
  if (response.internal_redirect_cnt_ > kMaxInternalRedirect) {
    config_handler.writeErrorLog(server, *location, "too continuous internal redirect", config::ERROR);
    response.setStatusCode(500);
    response.body_ = std::string(default_error_page_map_[500]) + webserv_error_page_tail;
    response.res_file_path_ = kDefaultPage;
    return sw_end_phase;
  }
  ++(response.internal_redirect_cnt_);
  *location = config_handler.searchLongestMatchLocationConfig(server, request.uri_);
  if (*location) config_handler.writeErrorLog("a request access " + (*location)->uri_, config::DEBUG);
  return sw_post_search_location_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleAllowPhase(HttpResponse& response, const HttpRequest& request,
                                                           const config::Server& server,
                                                           const config::Location* location,
                                                           struct sockaddr_in client_addr,
                                                           const ConfigHandler& config_handler) {
  int ret = config_handler.allowRequest(server, location, request, client_addr);
  if (ret == ConfigHandler::ACCESS_DENY) {
    response.setStatusCode(403);
    return sw_error_page_phase;
  }
  if (ret == ConfigHandler::METHOD_DENY) {
    response.setStatusCode(405);
    return sw_error_page_phase;
  }
  return sw_uri_check_phase;
}

void HttpResponse::prepareReturn(HttpResponse& response, const config::Return& return_directive) {
  std::string url = return_directive.getUrl();
  int code = return_directive.getCode();

  if (code == config::Return::kCodeUnset_) {
    response.setStatusCode(302);
    response.headers_[kLocation] = url;
    return;
  }
  if (config::Return::isRedirectCode(code)) {
    response.setStatusCode(code);
    response.headers_[kLocation] = url;
    return;
  }
  // textの場合
  response.setStatusCode(code);
  if (!url.empty()) response.body_ = url;
}

HttpResponse::ResponsePhase HttpResponse::handleReturnPhase(HttpResponse& response,
                                                            const config::Location* location,
                                                            const ConfigHandler& config_handler) {
  if (!location || (location && !utils::hasDirective(*location, kReturn))) return sw_allow_phase;

  prepareReturn(response, location->return_list_[0]);
  config_handler.writeErrorLog("redirect occured", config::DEBUG);
  return sw_error_page_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleUriCheckPhase(HttpResponse& response, HttpRequest& request,
                                                              const config::Location* location,
                                                              unsigned int request_port) {
  // uriにcgi_pathがあれば、path info処理をする
  if (setPathinfoIfValidCgi(response, request)) {
    return sw_content_phase;
  }
  // uriが'/'で終わってない、かつdirectoryであるとき301MovedPermanently
  if (lastChar(request.uri_) != '/' && utils::isDirectory(response.root_path_ + request.uri_, false)) {
    response.setStatusCode(301);
    response.headers_[kLocation] =
        kScheme + request.headers_[kHost] + ":" + utils::toStr(request_port) + request.uri_ + "/";
    return sw_error_page_phase;
  }
  // uriがディレクトリを指定しているのにlocationがなくて、root_pathとuriをつなげたものが存在しなければエラー
  if (lastChar(request.uri_) == '/' && !location &&
      !utils::isDirectory(response.root_path_ + request.uri_, false)) {
    response.setStatusCode(response.internal_redirect_cnt_ > 1 ? 500 : 404);
    return sw_error_page_phase;
  }
  // root_path_が存在しなければ404エラー
  if (!utils::isDirectory(response.root_path_, false)) {
    response.setStatusCode(404);
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
    if (isAccessibleFile(full_path)) {
      response.res_file_path_ = file_list[i];
      return sw_content_phase;
    }
  }

  // uri
  if (try_files.getCode() == config::TryFiles::kCodeUnset_) {
    request.uri_ = try_files.getUri();
    return sw_search_location_phase;
  }
  // code
  response.setStatusCode(try_files.getCode());
  return sw_error_page_phase;
}

std::string HttpResponse::autoIndex(const std::string& directory_path, const std::string& index_dir) {
  std::vector<std::string> contents = utils::createDirectoryContents(directory_path);
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
  static const std::string& format = "%d-%b-%Y %H:%M";
  for (std::vector<std::string>::iterator it = contents.begin(); it != contents.end(); ++it) {
    buffer << "<a href='";
    if (!directory_path.empty() && directory_path[directory_path.size() - 1] != '/') buffer << "/";
    buffer << *it << "'>" << *it << "</a>";

    struct stat file_stat;
    std::string full_path = directory_path + *it;
    if (*it != "../" && stat(full_path.c_str(), &file_stat) == 0) {
      buffer << "<span class=\"right-align\">";
      // file 最終修正時刻
      struct tm* last_modify_time = std::localtime(&file_stat.st_mtime);
      buffer << utils::formatTm(last_modify_time, format);

      // ファイルバイト数
      std::string space;
      if (S_ISREG(file_stat.st_mode)) {
        std::stringstream ss;
        ss << file_stat.st_size;
        for (size_t i = 0; i < 15 - ss.str().length() + 1; i++) space += " ";
        buffer << space << file_stat.st_size << "  ";
      } else {
        for (size_t i = 0; i < 15; i++) space += " ";
        buffer << space << "-  ";
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

HttpResponse::ResponsePhase HttpResponse::Index(HttpResponse& response, std::string& request_uri,
                                                const std::vector<config::Index>& index_list, bool is_alias,
                                                bool is_autoindex_on) {
  std::string directory_path = (is_alias) ? response.root_path_ : response.root_path_ + request_uri;

  for (size_t i = 0; i < index_list.size(); i++) {
    std::string full_path = directory_path + index_list[i].getFile();
    if (isAccessibleFile(full_path)) {
      response.res_file_path_ = (is_alias) ? index_list[i].getFile() : request_uri + index_list[i].getFile();
      return sw_content_phase;
    }
  }
  if (index_list.size() == 0) {
    std::string full_path = directory_path + config::Index::kDefaultFile_;
    if (isAccessibleFile(full_path)) {
      response.res_file_path_ =
          (is_alias) ? config::Index::kDefaultFile_ : request_uri + config::Index::kDefaultFile_;
      return sw_content_phase;
    }
  }

  if (is_autoindex_on) {
    response.body_ = autoIndex(directory_path, request_uri);
    // autoindexでディレクトリが見つからなかったら404エラー
    if (response.body_.empty()) {
      response.setStatusCode(404);
      return sw_error_page_phase;
    }
    response.res_file_path_ = kDefaultPage;
    return sw_log_phase;
  }

  // autoindex offなら403
  response.setStatusCode(403);
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
  bool is_autoindex_on = config_handler.isAutoIndexOn(server, location);
  bool is_alias = (location && utils::hasDirective(*location, kAlias)) ? true : false;

  // request uriが/で終わっていなければ直接ファイルを探しに行く。
  if (lastChar(request.uri_) != '/') {
    // aliasがある時は、request uriのlocation uriの部分をaliasに差し替える。
    if (is_alias) {
      std::string::size_type pos = request.uri_.find(location->uri_);
      if (pos != std::string::npos) request.uri_ = request.uri_.substr(pos + location->uri_.length());
    }
    std::string full_path = response.root_path_ + request.uri_;
    if (isAccessibleFile(full_path)) {
      response.res_file_path_ = request.uri_;
      return sw_content_phase;
    }
    if (!utils::isDirectory(response.root_path_ + request.uri_, false)) {
      response.setStatusCode(404);
      return sw_error_page_phase;
    }
  }

  /* ~ try_filesとindex/autoindexのファイル検索 ~
   * try_filesはlocationのuriを、ファイルを探す時のルートにいれずに内部リダイレクト
   * index/autoindex はrequestのuriにindexのファイル名を足して探す
   * 3つともなかったら上位のcontextで検索する
   */

  // location context
  if (location && utils::hasDirective(*location, kTryFiles))
    return TryFiles(response, request, location->try_files_);
  else if (location && utils::hasDirective(*location, kIndex))
    return Index(response, request.uri_, location->index_list_, is_alias, is_autoindex_on);

  // server context
  if (utils::hasDirective(server, kTryFiles))
    return TryFiles(response, request, server.try_files_);
  else if (utils::hasDirective(server, kIndex))
    return Index(response, request.uri_, server.index_list_, is_alias, is_autoindex_on);

  // http contextにindexディレクティブがあればその設定値をみるし、
  // なくとも、デフォルトのindexディレクティブを見る
  return Index(response, request.uri_, config_handler.config_->http_.index_list_, is_alias, is_autoindex_on);
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

HttpResponse::ResponsePhase HttpResponse::handleContentPhase(HttpResponse& response, HttpRequest& request) {
  if (cgi::CgiHandler::isCgi(response.res_file_path_)) {
    if (!isExecutable(response.root_path_ + response.res_file_path_)) {
      response.setStatusCode(403);
      return sw_error_page_phase;
    }
    response.state_ = RES_EXECUTE_CGI;
    return sw_end_phase;
  }
  if (request.method_ == config::POST || request.method_ == config::DELETE) {
    response.setStatusCode(405);
    return sw_error_page_phase;
  }
  response.body_ = utils::readFile(response.root_path_ + response.res_file_path_);
  return sw_log_phase;
}

HttpResponse::ResponsePhase HttpResponse::handleErrorPagePhase(HttpResponse& response, HttpRequest& request,
                                                               const config::Server& server,
                                                               const config::Location* location,
                                                               const ConfigHandler& config_handler) {
  const config::ErrorPage* ep = config_handler.searchErrorPage(server, location, response.getStatusCode());

  if (response.body_.empty() &&
      default_error_page_map_.find(response.getStatusCode()) != default_error_page_map_.end()) {
    response.res_file_path_ = kDefaultPage;
    response.body_ = std::string(default_error_page_map_[response.getStatusCode()]) + webserv_error_page_tail;
  }
  if (!ep) return sw_log_phase;

  // error page process
  long tmp_code;  // error_pageの=responseはlong_maxまで許容
  if ((tmp_code = ep->getResponse()) != config::ErrorPage::kResponseUnset) response.setStatusCode(tmp_code);
  request.uri_ = ep->getUri();
  return sw_search_location_phase;
}

void HttpResponse::headerFilterPhase(HttpResponse& response, const config::Time& time) {
  const int status_code = response.getStatusCode();
  const std::map<int, const char*>::iterator default_status_line = status_line_map_.find(status_code);

  response.headers_["Server"] = "webserv/1.0";
  response.headers_["Date"] = createCurrentGmtTime();
  response.headers_["Content-Length"] = utils::toStr(response.body_.size());
  // conetent-lengthで対応するので、Transfer-Encodingは削除する
  if (response.headers_.find(kTransferEncoding) != response.headers_.end())
    response.headers_.erase(kTransferEncoding);

  // requestエラーの場合は、接続を切る
  response.headers_[kConnection] = (time.isNoTime() || isErrorResponse(response) ? kClose : kKeepAlive);

  // cgiでstatus code lineの場合
  if (!response.status_code_line_.empty()) return;
  response.status_code_line_ = (default_status_line != status_line_map_.end()) ? default_status_line->second
                                                                               : utils::toStr(status_code);
  // staticのファイルの場合のみ、contetn-typeをつけてあげる
  if (response.state_ != RES_CREATING_STATIC) return;

  if (response.headers_.find(kContentType) == response.headers_.end())
    response.headers_[kContentType] = detectContentType(response.res_file_path_);
}

std::string HttpResponse::detectContentType(const std::string& res_file_path) {
  const char* kHtmlExt = ".html";
  const char* kCssExt = ".css";
  const char* kJsExt = ".js";

  if (utils::isExtensionFile(res_file_path, kHtmlExt)) return kHtml;
  if (utils::isExtensionFile(res_file_path, kCssExt)) return "text/css";
  if (utils::isExtensionFile(res_file_path, kJsExt)) return "text/javascript";
  return kTextPlain;
}

bool HttpResponse::isKeepaliveConnection(const HttpResponse& response) {
  std::map<std::string, std::string>::const_iterator it = response.headers_.find(kConnection);
  if (it == response.headers_.end()) return false;
  return it->second == kKeepAlive;
}

/**
 *
 * クライアントエラーリスポンス：400 – 499
 * サーバーエラーリスポンス：500 – 599
 */
bool HttpResponse::isErrorResponse(const HttpResponse& response) {
  return response.getStatusCode() >= 400 && response.getStatusCode() < 500;
}

char HttpResponse::lastChar(const std::string& str) { return str[str.size() - 1]; }

int HttpResponse::getStatusCode() const { return this->status_code_; }

void HttpResponse::setStatusCode(int code) { this->status_code_ = code; }

bool HttpResponse::isAccessibleFile(const std::string& file_path) {
  return utils::isFile(file_path, false) && syscall_wrapper::Access(file_path, R_OK, false) == 0;
}

bool HttpResponse::isExecutable(const std::string& file_path) {
  return syscall_wrapper::Access(file_path, X_OK, false) == 0;
}

bool HttpResponse::setPathinfoIfValidCgi(HttpResponse& response, HttpRequest& request) {
  std::istringstream uri_stream(request.uri_);
  std::vector<std::string> segments;
  std::string segment;
  while (std::getline(uri_stream, segment, '/')) {
    segments.push_back(segment);
  }

  std::string path = "";
  for (size_t i = 0; i < segments.size(); ++i) {
    if (i != 0) path += "/";
    path += segments[i];

    if (cgi::CgiHandler::isCgi(response.root_path_ + path) &&
        utils::isFile(response.root_path_ + path, false)) {
      response.separatePathinfo(request.uri_, path.size());
      request.uri_ = path;
      return true;
    }
  }
  return false;
}

void HttpResponse::separatePathinfo(const std::string& uri, size_t pos) {
  this->res_file_path_ = uri.substr(0, pos);
  this->path_info_ = uri.substr(pos);
}

void HttpResponse::clear() {
  this->root_path_.clear();
  this->res_file_path_.clear();
  this->path_info_.clear();
  this->state_ = RES_CREATING_STATIC;
  this->status_code_line_.clear();
  this->status_code_ = kInitStatusCode;
  this->headers_.clear();
  this->body_.clear();
  this->internal_redirect_cnt_ = 0;
}
