#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

# include <vector>

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void addConnection( int connfd );
		int getConnection();
		void removeConnection();
		void addContext( const std::vector<char>& context );
		const std::vector<char>& getContext() const;
		void addResponse( const std::vector<char>& context );
		const std::vector<char>& getResponse() const;
	
	private:
		//std::map<int connfd, std:;string context> connections;
		int connfd;
		std::vector<char> context; // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
		std::vector<char> response;
};

#endif
