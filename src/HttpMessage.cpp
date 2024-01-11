#include "HttpMessage.hpp"

// HttpRequest HttpMessage::requestParser( std::string &rawRequest )
// {
// }

std::string HttpMessage::responseGenerater( std::string request )
{
    (void)request;
    std::string response;

    std::string responseBody = "<html><body><h1>Hello, World!</h1></body></html>";

    // ステータスライン
    response += "HTTP/1.1 200 OK\r\n";

    // ヘッダー
    response += "Content-Type: text/html; charset=UTF-8\r\n";
    response += "Content-Length: " + std::to_string(responseBody.length()) + "\r\n";
    response += "\r\n";

    // ボディ
    response += responseBody;

    return response;
}

