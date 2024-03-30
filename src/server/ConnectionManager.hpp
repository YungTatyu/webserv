#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>
# include <map>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

struct TiedServer;

class ConnectionData
{
	public:
		enum EVENT {
			EV_READ,
			EV_WRITE,
		};
		std::vector<unsigned char> rawRequest; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<unsigned char> final_response_;
		EVENT event;
		HttpRequest request;
		HttpResponse response_;
		const TiedServer* tied_server_;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		ConnectionManager();
		~ConnectionManager();
		void setConnection( const int fd );
		void removeConnection( const int fd );
		void setRawRequest( const int fd, const std::vector<unsigned char>& rawRequest );
		const std::vector<unsigned char>& getRawRequest( const int fd ) const;
		void setFinalResponse( const int fd, const std::vector<unsigned char>& final_response );
		const std::vector<unsigned char>& getFinalResponse( const int fd ) const;
		void setEvent( const int fd, const ConnectionData::EVENT event );
		ConnectionData::EVENT getEvent( const int fd ) const;
		void setRequest( const int fd, const HttpRequest request );
		HttpRequest &getRequest( const int fd );
		void setResponse( const int fd, const HttpResponse response );
		HttpResponse &getResponse( const int fd );
		const std::map<int, ConnectionData> &getConnections() const;
		void setTiedServer( const int fd, const TiedServer* tied_server );
		const TiedServer& getTiedServer( const int fd ) const;
		void	closeAllConnections();
	private:
		std::map<int, ConnectionData> connections_;

};

#endif
