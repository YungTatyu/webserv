#include "RequestHandler.hpp"
#include "HttpMessage.hpp"
#include <fstream>

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
    const std::vector<char>& context = connManager.getContext();
    puts( context.data() );

    std::string requestData = context.data();
    HttpMessage::requestParser( requestData );

    std::string response = HttpMessage::responseGenerater( requestData );

    std::vector<char> vec(response.begin(), response.end());
    connManager.addResponse( vec );
}

