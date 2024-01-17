#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>
# include <map>

class ConnectionData
{
	public:
		std::vector<char> context; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<char> response;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void setConnection( const struct pollfd& pfd );
		void updateEvents( int fd, short revents );
		void removeConnection( int fd );
		void setContext( int fd, const std::vector<char>& context );
		const std::vector<char>& getContext( int fd ) const;
		void setResponse( int fd, const std::vector<char>& response );
		const std::vector<char>& getResponse( int fd ) const;
		std::vector<struct pollfd> fds;
	
	private:
		std::map<int, class ConnectionData> connections;
		// int connfd;
		// std::vector<char> context; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		// std::vector<char> response;
};

#endif
