#include "HttpResponse.hpp"
#include "FileUtils.hpp"
#include "string.h"

std::map<int, std::string> HttpResponse::status_line_map_;
std::map<int, const std::string*> HttpResponse::default_error_page_map_;

const static std::string http_version = "HTTP/1.1 ";

const static std::string webserv_error_301_page =
"<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n";

const static std::string webserv_error_302_page =
"<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n";

const static std::string webserv_error_303_page =
"<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See Other</h1></center>\r\n";

const static std::string webserv_error_307_page =
"<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n";

const static std::string webserv_error_308_page =
"<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent Redirect</h1></center>\r\n";

const static std::string webserv_error_400_page =
"<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n";

const static std::string webserv_error_401_page =
"<html>\r\n<head><title>401 Authorization Required</title></head>\r\n<body>\r\n<center><h1>401 Authorization Required</h1></center>\r\n";

const static std::string webserv_error_402_page =
"<html>\r\n<head><title>402 Payment Required</title></head>\r\n<body>\r\n<center><h1>402 Payment Required</h1></center>\r\n";

const static std::string webserv_error_403_page =
"<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n";

const static std::string webserv_error_404_page =
"<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n";

const static std::string webserv_error_405_page =
"<html>\r\n<head><title>405 Not Allowed</title></head>\r\n<body>\r\n<center><h1>405 Not Allowed</h1></center>\r\n";

const static std::string webserv_error_406_page =
"<html>\r\n<head><title>406 Not Acceptable</title></head>\r\n<body>\r\n<center><h1>406 Not Acceptable</h1></center>\r\n";

const static std::string webserv_error_408_page =
"<html>\r\n<head><title>408 Request Time-out</title></head>\r\n<body>\r\n<center><h1>408 Request Time-out</h1></center>\r\n";

const static std::string webserv_error_409_page =
"<html>\r\n<head><title>409 Conflict</title></head>\r\n<body>\r\n<center><h1>409 Conflict</h1></center>\r\n";

const static std::string webserv_error_410_page =
"<html>\r\n<head><title>410 Gone</title></head>\r\n<body>\r\n<center><h1>410 Gone</h1></center>\r\n";

const static std::string webserv_error_411_page =
"<html>\r\n<head><title>411 Length Required</title></head>\r\n<body>\r\n<center><h1>411 Length Required</h1></center>\r\n";

const static std::string webserv_error_412_page =
"<html>\r\n<head><title>412 Precondition Failed</title></head>\r\n<body>\r\n<center><h1>412 Precondition Failed</h1></center>\r\n";

const static std::string webserv_error_413_page =
"<html>\r\n<head><title>413 Request Entity Too Large</title></head>\r\n<body>\r\n<center><h1>413 Request Entity Too Large</h1></center>\r\n";

const static std::string webserv_error_414_page =
"<html>\r\n<head><title>414 Request-URI Too Large</title></head>\r\n<body>\r\n<center><h1>414 Request-URI Too Large</h1></center>\r\n";

const static std::string webserv_error_415_page =
"<html>\r\n<head><title>415 Unsupported Media Type</title></head>\r\n<body>\r\n<center><h1>415 Unsupported Media Type</h1></center>\r\n";

const static std::string webserv_error_416_page =
"<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body>\r\n<center><h1>416 Requested Range Not Satisfiable</h1></center>\r\n";

const static std::string webserv_error_421_page =
"<html>\r\n<head><title>421 Misdirected Request</title></head>\r\n<body>\r\n<center><h1>421 Misdirected Request</h1></center>\r\n";

const static std::string webserv_error_429_page =
"<html>\r\n<head><title>429 Too Many Requests</title></head>\r\n<body>\r\n<center><h1>429 Too Many Requests</h1></center>\r\n";

const static std::string webserv_error_494_page =
"<html>\r\n<head><title>400 Request Header Or Cookie Too Large</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>Request Header Or Cookie Too Large</center>\r\n";


const static std::string webserv_error_495_page =
"<html>\r\n<head><title>400 The SSL certificate error</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>The SSL certificate error</center>\r\n";

const static std::string webserv_error_496_page =
"<html>\r\n<head><title>400 No required SSL certificate was sent</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>No required SSL certificate was sent</center>\r\n";

const static std::string webserv_error_497_page =
"<html>\r\n<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>The plain HTTP request was sent to HTTPS port</center>\r\n";


const static std::string webserv_error_500_page =
"<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal Server Error</h1></center>\r\n";

const static std::string webserv_error_501_page =
"<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body>\r\n<center><h1>501 Not Implemented</h1></center>\r\n";

const static std::string webserv_error_502_page =
"<html>\r\n<head><title>502 Bad Gateway</title></head>\r\n<body>\r\n<center><h1>502 Bad Gateway</h1></center>\r\n";

const static std::string webserv_error_503_page =
"<html>\r\n<head><title>503 Service Temporarily Unavailable</title></head>\r\n<body>\r\n<center><h1>503 Service Temporarily Unavailable</h1></center>\r\n";

const static std::string webserv_error_504_page =
"<html>\r\n<head><title>504 Gateway Time-out</title></head>\r\n<body>\r\n<center><h1>504 Gateway Time-out</h1></center>\r\n";

const static std::string webserv_error_505_page =
"<html>\r\n<head><title>505 HTTP Version Not Supported</title></head>\r\n<body>\r\n<center><h1>505 HTTP Version Not Supported</h1></center>\r\n";

const static std::string webserv_error_507_page =
"<html>\r\n<head><title>507 Insufficient Storage</title></head>\r\n<body>\r\n<center><h1>507 Insufficient Storage</h1></center>\r\n";

HttpResponse::HttpResponse( const ConfigHandler& config_handler )
	: status_code_(200), body_(""), config_handler_(config_handler)
{
	// status_line
	this->status_line_map_[200] = http_version + "200 OK";
	this->status_line_map_[201] = http_version + "201 Created";
	this->status_line_map_[202] = http_version + "202 Accepted";
	this->status_line_map_[203] = http_version + "203 Non-Authoritative Information";
	this->status_line_map_[204] = http_version + "204 No Content";
	this->status_line_map_[205] = http_version + "205 Reset Content";
	this->status_line_map_[206] = http_version + "206 Partial Content";
	this->status_line_map_[207] = http_version + "207 Multi-Status";
	this->status_line_map_[300] = http_version + "300 Multiple Choices";
	this->status_line_map_[301] = http_version + "301 Moved Permanently";
	this->status_line_map_[302] = http_version + "302 Moved Temporarily";
	this->status_line_map_[303] = http_version + "303 See Other";
	this->status_line_map_[304] = http_version + "304 Not Modified";

	this->status_line_map_[307] = http_version + "307 Temporary Redirect";
	this->status_line_map_[308] = http_version + "308 Permanent Redirect";

	this->status_line_map_[400] = http_version + "400 Bad Request";
	this->status_line_map_[401] = http_version + "401 Unauthorized";
	this->status_line_map_[402] = http_version + "402 Payment Required";
	this->status_line_map_[403] = http_version + "403 Forbidden";
	this->status_line_map_[404] = http_version + "404 Not Found";
	this->status_line_map_[405] = http_version + "405 Not Allowed";
	this->status_line_map_[406] = http_version + "406 Not Acceptable";
	this->status_line_map_[407] = http_version + "407 Proxy Authentication Required";
	this->status_line_map_[408] = http_version + "408 Request Time-out";
	this->status_line_map_[409] = http_version + "409 Conflict";
	this->status_line_map_[410] = http_version + "410 Gone";
	this->status_line_map_[411] = http_version + "411 Length Required";
	this->status_line_map_[412] = http_version + "412 Precondition Failed";
	this->status_line_map_[413] = http_version + "413 Request Entity Too Large";
	this->status_line_map_[414] = http_version + "414 Request-URI Too Large";
	this->status_line_map_[415] = http_version + "415 Unsupported Media Type";
	this->status_line_map_[416] = http_version + "416 Requested Range Not Satisfiable";
	this->status_line_map_[417] = http_version + "417 Expectation Failed";
	this->status_line_map_[418] = http_version + "418 unused";
	this->status_line_map_[419] = http_version + "419 unused";
	this->status_line_map_[420] = http_version + "420 unused";
	this->status_line_map_[421] = http_version + "421 Misdirected Request";
	this->status_line_map_[422] = http_version + "422 Unprocessable Entity";
	this->status_line_map_[423] = http_version + "423 Locked";
	this->status_line_map_[424] = http_version + "424 Failed Dependency";
	this->status_line_map_[425] = http_version + "425 unused";
	this->status_line_map_[426] = http_version + "426 Upgrade Required";
	this->status_line_map_[427] = http_version + "427 unused";
	this->status_line_map_[428] = http_version + "428 Precondition Required";
	this->status_line_map_[429] = http_version + "429 Too Many Requests";
	this->status_line_map_[430] = http_version + "430";

	this->status_line_map_[500] = http_version + "500 Internal Server Error";
	this->status_line_map_[501] = http_version + "501 Not Implemented";
	this->status_line_map_[502] = http_version + "502 Bad Gateway";
	this->status_line_map_[503] = http_version + "503 Service Temporarily Unavailable";
	this->status_line_map_[504] = http_version + "504 Gateway Time-out";
	this->status_line_map_[505] = http_version + "505 HTTP Version Not Supported";
	this->status_line_map_[506] = http_version + "506 Variant Also Negotiates";
	this->status_line_map_[507] = http_version + "507 Insufficient Storage";
	this->status_line_map_[508] = http_version + "508 unused";
	this->status_line_map_[509] = http_version + "509 unused";
	this->status_line_map_[510] = http_version + "510 Not Extended";


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

std::string	HttpResponse::createStaticResponse()
{
	std::stringstream response;

	// status line
	response << this->status_line_map_[this->status_code_] << "\r\n";

	// headers
	for (std::map<std::string, std::string>::iterator it = this->connections_.begin();
		it != this->connections_.end();
		++it
		)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";

	// body
	response << this->body_;
	return response.str();
}

void	HttpResponse::prepareResponse( const HttpRequest& request, const struct TiedServer& tied_servers, const int client_sock )
{
	// server 探す
	const config::Server&	server = config_handler_.searchServerConfig(tied_servers, request.headers.find("Host")->second);

	// clientのip_addressを取る
	struct sockaddr_in client_addr;
	socklen_t client_addrlen = sizeof(client_addr);
	if (getsockname(client_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen) != 0)
	{
		std::cerr << "webserv: [emerge] getsockname() \"" << client_sock << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	}

	// parseが失敗していれば、400 Bad Request
	if (request.parseState == HttpRequest::PARSE_ERROR)
	{
		prepareErrorResponse(request, server, NULL, client_addr, 400);
		return;
	}

	// 内部リダイレクトなどはこれ再帰で回る
	responseHandler(request, server, client_addr);
}

void	HttpResponse::responseHandler( const HttpRequest& request, const config::Server& server, struct sockaddr_in client_addr)
{
	// server の return を見に行く。今はreturn はlocationにしかない
	const config::Location*	location = config_handler_.searchLongestMatchLocationConfig(server, request.uri);
	// location の　return を見に行く。
	// 以下は再帰用関数に入れる
	// allowReuestがfalseなら403 Forbidden
	if (!config_handler_.allowRequest(server, location, request, client_addr))
	{
		prepareErrorResponse(request, server, location, client_addr, 403);
		return;
	}


	// uriが'/'で終わってない、かつdirectoryでないとき301MovedPermanently
	if (request.uri[request.uri.length() - 1] != '/' && FileUtils::isDirectory(server.root.getPath() + request.uri))
	{
		prepareErrorResponse(request, server, location, client_addr, 301);
		return;
	}

	// ここらへんでメソッドごとに分岐

	/* ~ try_filesとindex/autoindexのファイル検索 ~
	 * try_filesはlocationのuriを探すファイルのルートにいれずに内部リダイレクト
	 * index/autoindex はrequestのuriにindexのファイル名を足して探す
	 * 3つともなかったら上位のcontextで検索する
	 */

	// try_filesとindex/autoindexをuriが属するcontextから探して返す。見つからなければ403エラー


}

void	HttpResponse::prepareErrorResponse( const HttpRequest& request, const config::Server& server, const config::Location* location, const struct sockaddr_in client_addr, const unsigned int code )
{
	long tmp_code;

	const config::ErrorPage* ep = config_handler_.searchErrorPage(server, location, code);
	if (ep)
	{
		if ((tmp_code = ep->getResponse()) != config::ErrorPage::kResponseUnset)
			status_code_ = tmp_code;
		internalRedirect(request, server, client_addr, ep->getUri()); // 新しいリクエストを作ってもう一度responseHandlerをする
	}
	else
	{
		status_code_ = code;
		body_ = *default_error_page_map_[code];
	}
}

void	HttpResponse::returnResponse( config::Return& return_directive )
{
	const std::string	http = "http://"; 
	const std::string	https = "https://"; 
	std::string	url = return_directive.getUrl();
	int	code = return_directive.getCode();

	if (code == config::Return::kCodeUnset)
	{
		this->headers["Location"] = url;
		this->status_code_ = 302;
		this->headers["Content-Type"] = "text/html"; // ここでやるべきか
	}
	else if (301 <= code && code <= 8)
	{
		this->headers["Location"] = url;
		this->status_code_ = code;
		this->headers["Content-Type"] = "text/html";
	}
	else
	{
		this->body_ = url;
		this->status_code_ = code;
		this->headers["Content-Type"] = "text/plain";
	}
}

//alias: ディレクトリ内のリクエストに対して、別の場所からファイルを提供するために使用される。内部的にリダイレクトが発生する可能性があります。

//error_page: エラーページを指定するディレクティブ。指定されたエラーが発生した場合に、指定されたエラーページに対して再度処理が行われる可能性があります。

//return: 特定の条件に基づいてレスポンスを生成し、クライアントに返すディレクティブ。return を使用して新しいURIにリダイレクトすることができます。

//try_files: ファイルの存在を確認し、存在すればそのファイルを提供し、存在しなければ指定されたファイルまたはURIに対して再度処理が行われる可能性があります。

void	HttpResponse::internalRedirect( const HttpRequest& request, const config::Server& server, const struct sockaddr_in& client_addr, std::string redirect_uri )
{
	// status_codeはerror_pageからのリダイレクトの場合変更する可能性がある
	if (!redirect_uri.empty())
	{
		HttpRequest	redirect_request = request;
		redirect_request.uri = redirect_uri;
		responseHandler(redirect_request, server, client_addr);
	}

	// methodは必ずＧＥＴになる?
	responseHandler(request, server, client_addr);
}

