#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void setConnection( int connfd );
		int getConnection();
		void removeConnection();
		void setContext( const std::vector<char>& context );
		const std::vector<char>& getContext() const;
		void setResponse( const std::vector<char>& context );
		const std::vector<char>& getResponse() const;
	
	private:
		//std::map<int connfd, std:;string context> connections;
		int connfd_;
		std::vector<char> context_; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<char> response_;
};

#endif
