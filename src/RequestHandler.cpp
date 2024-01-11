#include "RequestHandler.hpp"
#include "HttpMessage.hpp"

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
    const std::vector<char>& context = connManager.getContext();
    puts( context.data() );

    // HTTP::parseRequest();

    std::string response = HttpMessage::responseGenerater( context.data() );

    std::vector<char> vec(response.begin(), response.end());
    connManager.addResponse( vec );
}

