#include "RequestHandler.hpp"

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
	const std::vector<char>& context = connManager.getContext();
	puts( context.data() );
}


