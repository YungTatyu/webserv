#include "HttpResponse.hpp"
#include "Utils.hpp"
#include "string.h"
#include <ctime>
#include <iomanip>

static const int OK = 1;
static const int REDIRECT = 2;
static const int INTERNAL_REDIRECT = 3;
static const int ERROR_PAGE = 4;
static const std::string kTRY_FILES = "try_files";
static const std::string kINDEX = "index";

std::map<int, std::string> HttpResponse::status_line_map_;
std::map<int, const std::string*> HttpResponse::default_error_page_map_;

static const std::string http_version = "HTTP/1.1";

static const std::string webserv_error_301_page =
"<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n";

static const std::string webserv_error_302_page =
"<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n";

static const std::string webserv_error_303_page =
"<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See Other</h1></center>\r\n";

static const std::string webserv_error_307_page =
"<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n";

static const std::string webserv_error_308_page =
"<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent Redirect</h1></center>\r\n";

static const std::string webserv_error_400_page =
"<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n";

static const std::string webserv_error_401_page =
"<html>\r\n<head><title>401 Authorization Required</title></head>\r\n<body>\r\n<center><h1>401 Authorization Required</h1></center>\r\n";

static const std::string webserv_error_402_page =
"<html>\r\n<head><title>402 Payment Required</title></head>\r\n<body>\r\n<center><h1>402 Payment Required</h1></center>\r\n";

static const std::string webserv_error_403_page =
"<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n";

static const std::string webserv_error_404_page =
"<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n";

static const std::string webserv_error_405_page =
"<html>\r\n<head><title>405 Not Allowed</title></head>\r\n<body>\r\n<center><h1>405 Not Allowed</h1></center>\r\n";

static const std::string webserv_error_406_page =
"<html>\r\n<head><title>406 Not Acceptable</title></head>\r\n<body>\r\n<center><h1>406 Not Acceptable</h1></center>\r\n";

static const std::string webserv_error_408_page =
"<html>\r\n<head><title>408 Request Time-out</title></head>\r\n<body>\r\n<center><h1>408 Request Time-out</h1></center>\r\n";

static const std::string webserv_error_409_page =
"<html>\r\n<head><title>409 Conflict</title></head>\r\n<body>\r\n<center><h1>409 Conflict</h1></center>\r\n";

static const std::string webserv_error_410_page =
"<html>\r\n<head><title>410 Gone</title></head>\r\n<body>\r\n<center><h1>410 Gone</h1></center>\r\n";

static const std::string webserv_error_411_page =
"<html>\r\n<head><title>411 Length Required</title></head>\r\n<body>\r\n<center><h1>411 Length Required</h1></center>\r\n";

static const std::string webserv_error_412_page =
"<html>\r\n<head><title>412 Precondition Failed</title></head>\r\n<body>\r\n<center><h1>412 Precondition Failed</h1></center>\r\n";

static const std::string webserv_error_413_page =
"<html>\r\n<head><title>413 Request Entity Too Large</title></head>\r\n<body>\r\n<center><h1>413 Request Entity Too Large</h1></center>\r\n";

static const std::string webserv_error_414_page =
"<html>\r\n<head><title>414 Request-URI Too Large</title></head>\r\n<body>\r\n<center><h1>414 Request-URI Too Large</h1></center>\r\n";

static const std::string webserv_error_415_page =
"<html>\r\n<head><title>415 Unsupported Media Type</title></head>\r\n<body>\r\n<center><h1>415 Unsupported Media Type</h1></center>\r\n";

static const std::string webserv_error_416_page =
"<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body>\r\n<center><h1>416 Requested Range Not Satisfiable</h1></center>\r\n";

static const std::string webserv_error_421_page =
"<html>\r\n<head><title>421 Misdirected Request</title></head>\r\n<body>\r\n<center><h1>421 Misdirected Request</h1></center>\r\n";

static const std::string webserv_error_429_page =
"<html>\r\n<head><title>429 Too Many Requests</title></head>\r\n<body>\r\n<center><h1>429 Too Many Requests</h1></center>\r\n";

static const std::string webserv_error_494_page =
"<html>\r\n<head><title>494 Request Header Or Cookie Too Large</title></head>\r\n<body>\r\n<center><h1>494 Bad Request</h1></center>\r\n<center>Request Header Or Cookie Too Large</center>\r\n";


static const std::string webserv_error_495_page =
"<html>\r\n<head><title>495 The SSL certificate error</title></head>\r\n<body>\r\n<center><h1>495 Bad Request</h1></center>\r\n<center>The SSL certificate error</center>\r\n";

static const std::string webserv_error_496_page =
"<html>\r\n<head><title>496 No required SSL certificate was sent</title></head>\r\n<body>\r\n<center><h1>496 Bad Request</h1></center>\r\n<center>No required SSL certificate was sent</center>\r\n";

static const std::string webserv_error_497_page =
"<html>\r\n<head><title>497 The plain HTTP request was sent to HTTPS port</title></head>\r\n<body>\r\n<center><h1>497 Bad Request</h1></center>\r\n<center>The plain HTTP request was sent to HTTPS port</center>\r\n";


static const std::string webserv_error_500_page =
"<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal Server Error</h1></center>\r\n";

static const std::string webserv_error_501_page =
"<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body>\r\n<center><h1>501 Not Implemented</h1></center>\r\n";

static const std::string webserv_error_502_page =
"<html>\r\n<head><title>502 Bad Gateway</title></head>\r\n<body>\r\n<center><h1>502 Bad Gateway</h1></center>\r\n";

static const std::string webserv_error_503_page =
"<html>\r\n<head><title>503 Service Temporarily Unavailable</title></head>\r\n<body>\r\n<center><h1>503 Service Temporarily Unavailable</h1></center>\r\n";

static const std::string webserv_error_504_page =
"<html>\r\n<head><title>504 Gateway Time-out</title></head>\r\n<body>\r\n<center><h1>504 Gateway Time-out</h1></center>\r\n";

static const std::string webserv_error_505_page =
"<html>\r\n<head><title>505 HTTP Version Not Supported</title></head>\r\n<body>\r\n<center><h1>505 HTTP Version Not Supported</h1></center>\r\n";

static const std::string webserv_error_507_page =
"<html>\r\n<head><title>507 Insufficient Storage</title></head>\r\n<body>\r\n<center><h1>507 Insufficient Storage</h1></center>\r\n";

HttpResponse::HttpResponse()
	: serv_type_(HttpResponse::STATIC), status_code_(200), body_(""), internal_redirect_cnt_(0)
{
	this->headers_["Server"] = "webserv/1";
	this->headers_["Connection"] = "keep-alive";
	this->headers_["Date"] = getCurrentGMTTime();

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

std::string	HttpResponse::getCurrentGMTTime()
{
	// 現在の時間を取得
	std::time_t currentTime = std::time(NULL);

	// 構造体 tm に変換
	std::tm *gmTime = std::gmtime(&currentTime);

	// 曜日の文字列
	const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	// 月の文字列
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	// 指定されたフォーマットで文字列に変換
	std::ostringstream oss;
	oss << days[gmTime->tm_wday] << ", "
		<< std::setfill('0') << std::setw(2) << gmTime->tm_mday << " "
		<< months[gmTime->tm_mon] << " "
		<< 1900 + gmTime->tm_year << " "
		<< std::setfill('0') << std::setw(2) << gmTime->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << gmTime->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << gmTime->tm_sec
		<< " GMT";

	return oss.str();
}

std::string	HttpResponse::transformLetter( const std::string& key_str )
{
	std::string result = key_str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	if (!result.empty())
	{
		result[0] = std::toupper(result[0]);
		for (size_t i = 1; i < result.size(); i++)
			if (result[i - 1] == '-')
				result[i] = std::toupper(result[i]);
	}
	return result;
}

std::string	HttpResponse::createResponse( const HttpResponse& response )
{
	std::stringstream stream;
	std::map<int, std::string>::iterator it = status_line_map_.find(response.status_code_);

	// status line
	stream << http_version << " ";
	if (it != status_line_map_.end())
		stream << status_line_map_[response.status_code_] << "\r\n";
	else
		stream << response.status_code_ << "\r\n";

	// headers
	for (std::map<std::string, std::string>::const_iterator it = response.headers_.begin();
		it != response.headers_.end();
		++it
		)
		stream << transformLetter(it->first) << ": " << it->second << "\r\n";
	stream << "\r\n";

	// body
	stream << response.body_;
	return stream.str();
}

/*
 * HttpResponseオブジェクトを生成し、send用のresponseを生成する
 */
std::string	HttpResponse::generateResponse( HttpRequest& request, HttpResponse& response, const struct TiedServer& tied_servers, const int client_sock, const ConfigHandler& config_handler )
{
	// chunkなどでparse途中の場合。
	if (request.parseState == HttpRequest::PARSE_INPROGRESS)
		return std::string();

	const config::Server&	server = config_handler.searchServerConfig(tied_servers, request.headers.find("Host")->second);
	const config::Location*	location = NULL;
	struct sockaddr_in	client_addr;
	socklen_t	client_addrlen = sizeof(client_addr);
	int	ret;

	enum PrepareResponsePhase {
		START_PHASE = 0,
		PRE_SEARCH_LOCATION_PHASE,
		SEARCH_LOCATION_PHASE,
		POST_SEARCH_LOCATION_PHASE,
		RETURN_PHASE,
		ALLOW_PHASE,
		URI_CHECK_PHASE,
		CONTENT_PHASE,
		ERROR_PAGE_PHASE,
		LOG_PHASE,
		END_PHASE
	} state;

	state = START_PHASE;

	while (state != END_PHASE) {
		switch (state) {
		case START_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] start phase\n");
			state = PRE_SEARCH_LOCATION_PHASE;
			break;
		case PRE_SEARCH_LOCATION_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] pre search location phase\n");
			// parse error
			if (request.parseState == HttpRequest::PARSE_ERROR)
			{
				response.status_code_ = 400;
				state = ERROR_PAGE_PHASE;
				break;
			}

			// clientのip_addressを取る
			// retry するか？
			if (getsockname(client_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen) != 0)
			{
				std::cerr << "webserv: [emerge] getsockname() \"" << client_sock << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
				state = END_PHASE;
			}
			else
				state = SEARCH_LOCATION_PHASE;
			break;
		case SEARCH_LOCATION_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] search location phase\n");
			if (response.internal_redirect_cnt_++ > kMaxInternalRedirect)
			{
				config_handler.writeErrorLog(server, location, "webserv: [error] too continuous internal redirect\n");
				response.status_code_ = 500;
				response.body_ = *default_error_page_map_[500];
				response.headers_["Connection"] = "close";
				state = END_PHASE;
				break;
			}
			location = config_handler.searchLongestMatchLocationConfig(server, request.uri);
			state = POST_SEARCH_LOCATION_PHASE;
			break;
		case POST_SEARCH_LOCATION_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] post search location phase\n");
			response.root_path_ = config_handler.searchRootPath(server, location);
			state = RETURN_PHASE;
			break;
		case RETURN_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] return phase\n");
			if (returnPhase(response, location) == REDIRECT)
				state = ERROR_PAGE_PHASE;
			else
				state = ALLOW_PHASE;
			break;
		case ALLOW_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] allow phase\n");
			ret = config_handler.allowRequest(server, location, request, client_addr);
			if (ret == ConfigHandler::ACCESS_DENY)
			{
				response.status_code_ = 403;
				state = ERROR_PAGE_PHASE;
			}
			else if (ret == ConfigHandler::METHOD_DENY)
			{
				response.status_code_ = 405;
				state = ERROR_PAGE_PHASE;
			}
			else
				state = URI_CHECK_PHASE;
			break;
		case URI_CHECK_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] uri check phase\n");
			// uriが'/'で終わってない、かつdirectoryであるとき301MovedPermanently
			if (request.uri[request.uri.length() - 1] != '/' && Utils::isDirectory(server.root.getPath() + request.uri))
			{
				response.status_code_ = 301;
				state = ERROR_PAGE_PHASE;
			}
			else if (request.uri[request.uri.length() - 1] == '/' && !location)
			{
				response.status_code_ = 404;
				state = ERROR_PAGE_PHASE;
			}
			else
				state = CONTENT_PHASE;
			break;
		case CONTENT_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] content phase\n");
			ret = contentHandler(response, request, server, location, config_handler);
			if (ret == INTERNAL_REDIRECT)
				state = SEARCH_LOCATION_PHASE;
			else if (ret == ERROR_PAGE)
				state = ERROR_PAGE_PHASE;
			else
				state = LOG_PHASE;
			break;
		case ERROR_PAGE_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] error page phase\n");
			if (errorPagePhase(response, request, server, location, config_handler) == INTERNAL_REDIRECT)
				state = SEARCH_LOCATION_PHASE;
			else
				state = LOG_PHASE;
			break;
		case LOG_PHASE:
			config_handler.writeErrorLog(server, location, "webserv: [debug] log phase\n");
			config_handler.writeAccessLog(server, location, config_handler.createAcsLogMsg(client_addr.sin_addr.s_addr, response.status_code_, request));
			state = END_PHASE;
			break;
		default:
			state = END_PHASE;
			break;
		}
	}

	config_handler.writeErrorLog(server, location, "webserv: [debug] header filter\n");
	if (response.serv_type_ == HttpResponse::STATIC)
	{
		headerFilterPhase(response);
	}

	config_handler.writeErrorLog(server, location, "webserv: [debug] create final response\n");
	return createResponse(response);
}

int	HttpResponse::returnPhase( HttpResponse& response, const config::Location* location )
{
	if (!location || location->directives_set.find("return") == location->directives_set.end())
		return OK;

	returnResponse(response, location->return_list[0]);
	return REDIRECT;
}

void	HttpResponse::returnResponse( HttpResponse& response, const config::Return& return_directive )
{
	std::string	url = return_directive.getUrl();
	int	code = return_directive.getCode();

	if (code == config::Return::kCodeUnset)
	{
		response.status_code_ = 302;
		response.headers_["Location"] = url;
	}
	else if (config::Return::isRedirectCode(code))
	{
		response.status_code_ = code;
		response.headers_["Location"] = url;
	}
	else
	{
		response.status_code_ = code;
		if (!url.empty())
			response.body_ = url;
	}
}

/* try_files
 *
 * fileが見つかればbodyにセットして返す。
 * codeならErrorPage探すように返す。
 * URIなら内部リダイレクト
 */
int	HttpResponse::TryFiles( HttpResponse& response, HttpRequest& request, const config::TryFiles& try_files )
{
	std::vector<std::string>	file_list = try_files.getFileList();

	for (size_t i = 0; i < file_list.size(); i++)
	{
		std::string	full_path = response.root_path_ + file_list[i];
		if (Utils::wrapperAccess(full_path, F_OK, false) == 0 &&
			Utils::wrapperAccess(full_path, R_OK, false) == 0)
		{
			response.body_ = Utils::readFile(full_path);
			return OK;
		}
	}

	// uri
	if (try_files.getCode() == config::TryFiles::kCodeUnset)
	{
		request.uri = try_files.getUri();
		return INTERNAL_REDIRECT;
	}
	else // code
	{
		response.status_code_ = try_files.getCode();
		return ERROR_PAGE;
	}
}

std::string HttpResponse::autoIndex( const std::string& directory_path, const std::string& index_dir )
{
	std::vector<std::string> contents = Utils::createDirectoryContents(directory_path);
	// もしディレクトリが存在しなければ空文字を返す。
	if (contents.empty())
		return std::string("");

	std::stringstream buffer;
	buffer << "<!DOCTYPE html>";
	buffer << "<html>";
	buffer << "<head>";
	buffer << "<meta charset=\"utf-8\"><title>Index of " << index_dir << "</title>";
	// css
	buffer << "<style>";
	buffer << ".right-align {position: absolute;right: 0;}"; // 親要素の中での位置を右寄せにする。
	buffer << "</style>";
	buffer << "</head>\r\n";
	buffer << "<body>";
	buffer << "<h1>Index of " << index_dir << "</h1>";
	buffer << "<hr>";
	buffer << "<pre>";

	for (std::vector<std::string>::iterator it = contents.begin(); it != contents.end(); ++it)
	{
		buffer << "<a href='";
		if (!directory_path.empty() && directory_path[directory_path.size() - 1] != '/')
		    buffer << "/";
		buffer << *it << "'>" << *it << "</a>";

		struct stat	file_stat;
		std::string	full_path = directory_path + *it;
		if (*it != "../" && stat(full_path.c_str(), &file_stat) == 0)
		{
			buffer << "<span class=\"right-align\">";
			// file 最終修正時刻
			struct tm last_modify_time;
			localtime_r(&file_stat.st_mtime, &last_modify_time);
			char date[1024];
			std::strftime(date, sizeof(date), "%d-%b-%Y %H:%M", &last_modify_time);
			buffer << date;

			// ファイルバイト数
			std::string	space;
			if (S_ISREG(file_stat.st_mode))
			{
				std::stringstream	ss;
				ss << file_stat.st_size;
				for (size_t i = 0; i < 15 - ss.str().length() + 1; i++)
					space += " ";
				buffer << space << file_stat.st_size << "  ";
			}
			else
			{
				for (size_t i = 0; i < 15; i++)
					space += " ";
				buffer << space << "-" << "  ";
			}
			buffer << "</span>";
		}

		buffer << "\r\n";
	}

	buffer << "</pre>";
	buffer << "<hr>";
	buffer << "</body></html>";
	return buffer.str();
}

int HttpResponse::Index( HttpResponse& response, HttpRequest& request, const std::vector<config::Index>& index_list, bool is_autoindex_on, const std::string& index_dir )
{
	std::string	directory_path = response.root_path_ + request.uri;
	for (size_t i = 0; i < index_list.size(); i++)
	{
		std::string	full_path = directory_path + index_list[i].getFile();
		if (Utils::wrapperAccess(full_path, F_OK, false) == 0 ||
			Utils::wrapperAccess(full_path, R_OK, false) == 0)
		{
			response.body_ = Utils::readFile(full_path);
			return OK;
		}
	}

	if (is_autoindex_on)
	{
		response.body_ = autoIndex(directory_path, index_dir);
		// autoindexでディレクトリが見つからなかったら404エラー
		if (response.body_.empty())
		{
			response.status_code_ = 404;
			return ERROR_PAGE;
		}
		return OK;
	}

	// autoindex offなら403
	response.status_code_ = 403;
	return ERROR_PAGE;
}

/*
 * uriが'/'で終わっていなければ直接探しに行き、
 * そうでなければ、ディレクティブを順番に適用する。
 */
int	HttpResponse::staticHandler( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler& config_handler )
{
	response.serv_type_ = HttpResponse::STATIC;
	// request uriが/で終わっていなければ直接ファイルを探しに行く。
	if (request.uri[request.uri.length() - 1] != '/')
	{
		std::string full_path = response.root_path_ + request.uri;
		if (Utils::wrapperAccess(full_path, F_OK, false) != 0 ||
			Utils::wrapperAccess(full_path, R_OK, false) != 0)
		{
			response.status_code_ = 404;
			return ERROR_PAGE;
		}
		response.body_ = Utils::readFile(full_path);
		return OK;
	}

	/* ~ try_filesとindex/autoindexのファイル検索 ~
	 * try_filesはlocationのuriを探すファイルのルートにいれずに内部リダイレクト
	 * index/autoindex はrequestのuriにindexのファイル名を足して探す
	 * 3つともなかったら上位のcontextで検索する
	 */
	bool	is_autoindex_on = config_handler.isAutoIndexOn(server, location);
	std::string	index_dir;
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

int	HttpResponse::contentHandler( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler &config_handler )
{

	if (config_handler.convertRequestMethod(request.method) == config::GET)
		return staticHandler(response, request, server, location, config_handler);
	return OK;
}

int	HttpResponse::errorPagePhase( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler &config_handler )
{
	const config::ErrorPage* ep = config_handler.searchErrorPage(server, location, response.status_code_);

	if (response.body_.empty() && default_error_page_map_.find(response.status_code_) != default_error_page_map_.end())
		response.body_ = *default_error_page_map_[response.status_code_];
	if (!ep)
	{
		return OK;
	}

	// error page process
	long	tmp_code; // error_pageの=responseはlong_maxまで許容
	if ((tmp_code = ep->getResponse()) != config::ErrorPage::kResponseUnset)
		response.status_code_ = tmp_code;
	request.uri = ep->getUri();
	return INTERNAL_REDIRECT;
}

void	HttpResponse::headerFilterPhase( HttpResponse& response )
{
	if (!response.body_.empty())
	{
		std::stringstream stream;
		stream << response.body_.length();
		response.headers_["Content-Length"] = stream.str();
	}

	response.headers_["Content-Type"] = detectContentTypeFromBody(response.body_);
}

std::string	HttpResponse::detectContentTypeFromBody(const std::string& body)
{
	// ボディが空の場合はデフォルトのContent-Typeを返す
	if (body.empty())
		return "text/plain";

	if (body.find("<html") != std::string::npos)
		return "text/html";
	else
		return "text/plain";
}

bool	HttpResponse::isURL( const std::string uri ) const
{
	const std::string	http = "http://";
	const std::string	https = "https://";
	if (uri.substr(0, http.length()) != http &&
		uri.substr(0, https.length()) != https)
		return true;
	return false;
}

