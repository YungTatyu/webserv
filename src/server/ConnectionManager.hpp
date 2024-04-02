#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>
# include <map>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "CGIHandler.hpp"

struct TiedServer;
class CGIHandler;

class ConnectionData
{
	public:
		enum EVENT {
			EV_READ,
			EV_WRITE,
			EV_CGI_READ,
			EV_CGI_WRITE,
		};
		std::vector<unsigned char> rawRequest; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<unsigned char> final_response_;
		std::vector<unsigned char> cgi_response_;
		size_t	sent_bytes_; // responseやcgi bodyをsendする際に送信したbyte数を記録する
		EVENT event;
		HttpRequest request;
		HttpResponse response_;
		cgi::CGIHandler cgi_handler_;
		const TiedServer* tied_server_;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		ConnectionManager();
		~ConnectionManager();
		void setConnection( const int fd );
		void setCgiConnection( const int cli_sock, const ConnectionData::EVENT event );
		void removeConnection( const int fd, const bool del );
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
		void	addCgiResponse( const int fd, const std::vector<unsigned char>& v );
		const std::vector<unsigned char>&	getCgiResponse( const int fd ) const;
		const std::map<int, ConnectionData*> &getConnections() const;
		void setTiedServer( const int fd, const TiedServer* tied_server );
		const TiedServer& getTiedServer( const int fd ) const;
		const cgi::CGIHandler& getCgiHandler( const int fd ) const;
		size_t	getSentBytes( const int fd ) const;
		void	addSentBytes( const int fd, size_t bytes );
		void	resetSentBytes( const int fd );
		void	resetCgiSockets( const int fd );
		void	closeAllConnections();
		bool	isCgiSocket( const int fd ) const;
	private:
		std::map<int, ConnectionData*> connections_;
};

#endif
