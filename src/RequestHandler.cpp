#include "RequestHandler.hpp"
#include "HttpMessage.hpp"

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
    const std::vector<char>& context = connManager.getContext();
    puts( context.data() );

    std::string requestData = context.data();
    HttpRequest request = HttpMessage::requestParser( requestData );
    std::string response = HttpMessage::responseGenerater( request );

    std::vector<char> vec(response.begin(), response.end());
    connManager.addResponse( vec );
}

