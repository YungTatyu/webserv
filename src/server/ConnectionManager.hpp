#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>
# include <map>

class ConnectionData
{
	public:
		enum EVENT {
			READ,
			WRITE,
		};
		std::vector<char> rawRequest; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<char> response;
		EVENT event;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void setConnection( const int fd );
		void removeConnection( const int fd );
		void setRawRequest( const int fd, const std::vector<char>& rawRequest );
		const std::vector<char>& getRawRequest( const int fd ) const;
		void setResponse( const int fd, const std::vector<char>& response );
		const std::vector<char>& getResponse( const int fd ) const;
		void setEvent( const int fd, const ConnectionData::EVENT event );
		ConnectionData::EVENT getEvent( const int fd ) const;
		const std::map<int, ConnectionData> &getConnections() const;
	private:
		std::map<int, class ConnectionData> connections_;
};

#endif
