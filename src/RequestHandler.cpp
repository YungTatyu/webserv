#include "RequestHandler.hpp"
#include "HttpMessage.hpp"

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
    const std::vector<char>& context = connManager.getContext();
	// std::cout << "----- request -----" << std::endl;
	std::cout << context.data() << std::endl;
	// std::cout << "------- end -------" << std::endl;

    std::string requestData = context.data();
    HttpRequest request = HttpMessage::requestParser( requestData );
    std::string response = HttpMessage::responseGenerater( request );

    std::vector<char> vec(response.begin(), response.end());
    connManager.setResponse( vec );
}

