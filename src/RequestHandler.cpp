#include "RequestHandler.hpp"

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
    const std::vector<char>& context = connManager.getContext();
    puts( context.data() );

	// HTTP::parseRequest();
	// HTTP::generateRequest(); 

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

    std::vector<char> vec(response.begin(), response.end());
    connManager.addContext( vec );
}

