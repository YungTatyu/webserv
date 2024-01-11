#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>
# include <stdio.h>
# include <string>
# include "ConnectionManager.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		void handle( ConnectionManager &connManager );
};

#endif
