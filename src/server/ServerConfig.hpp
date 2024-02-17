#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

# include "NetworkIOHandler.hpp"
# include "Main.hpp"

# include <map>

/**
 * ServerConfig (ConfigReader) 方針
 * 基本的に、直接publicのconfig_を見てもらう
 * 個々のメソッドは補助関数のようなものだけ
 * この場合、すべてのクラスがこのクラスのオブジェクトを持たなければならない
*/ 

/**
 *IPv4
 *
  typedef __uint32_t      in_addr_t;      // base type for internet address
  typedef __uint16_t      in_port_t;

  struct in_addr {
        in_addr_t s_addr;
  };

  struct sockaddr_in {
        __uint8_t       sin_len;
        sa_family_t     sin_family;
        in_port_t       sin_port;
        struct  in_addr sin_addr;
        char            sin_zero[8];
  };
  *
  *
  *IPv6
  *
  struct in6_addr {
        union {
                __uint8_t   __u6_addr8[16];
                __uint16_t  __u6_addr16[8];
                __uint32_t  __u6_addr32[4];
        } __u6_addr;                    // 128-bit IP6 address
  };

  struct sockaddr_in6 {
        __uint8_t       sin6_len;       // length of this struct(sa_family_t)
        sa_family_t     sin6_family;    // AF_INET6 (sa_family_t)
        in_port_t       sin6_port;      // Transport layer port # (in_port_t)
        __uint32_t      sin6_flowinfo;  // IP6 flow information
        struct in6_addr sin6_addr;      // IP6 address 
        __uint32_t      sin6_scope_id;  // scope zone index
  };
 */

struct TiedServer;

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		// data
		const config::Main*	config_;

		// initialize
		ServerConfig() : config_(NULL) {};
		void	loadConfiguration( const config::Main* config );

		// method
		// getsockname()でcli_sockからアドレスとるようにしているが
		// 問題があれば、引数をsockaddr_storageにして、sockaddr_in[6]をreinterpret_cast<sockaddr_storage>()で渡してもいいかもしれない
		// それか、ipv4用とipv6用を両方つくる
		bool	allowRequest( const struct TiedServer& server_config,
							const std::string& server_name,
							const std::string& uri,
							const int cli_sock ) const;
		const std::string&	searchFile( const struct TiedServer& server_config,
									const std::string& server_name,
									const std::string& uri ) const;
		void	writeAcsLog( const struct TiedServer& server_config,
							const std::string& server_name,
							const std::string& uri,
							const std::string& msg ) const;
		void	writeErrLog( const struct TiedServer& server_config,
							const std::string& server_name,
							const std::string& uri,
							const std::string& msg ) const;
		const config::Time&	searchKeepaliveTimeout( const struct TiedServer& server_config,
												const std::string& server_name,
												const std::string& uri ) const;
		const config::Time&	searchSendTimeout( const struct TiedServer& server_config,
											const std::string& server_name,
											const std::string& uri ) const;
		const config::Time&	searchUseridExpires( const struct TiedServer& server_config,
											const std::string& server_name,
											const std::string& uri ) const;
		const struct TiedServer	retTiedServer( const std::string addr, const unsigned int port ) const;

	private:
		// data
		static std::map<int, std::string> default_page_map_; // error code と default error pageのmap

		// utils
		// 必要なメソッド追加

	public:
		int		getServPort();
		int		getListenQ();

	private:
		int		servPort_; /*port*/
		int		listenQ_; /*maximum number of client connections */
};

#endif
