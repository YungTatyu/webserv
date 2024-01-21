#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

namespace config
{
enum CONNECTION_METHOD
{
	SELECT,
	POLL,
	KQUEUE,
	EPOLL,
};

class Use
{
	private:
		CONNECTION_METHOD	connection_method_;
	public:
		Use() {}
		~Use() {}
		const CONNECTION_METHOD	getConnectionMethod() const { return this->connection_method_; }
		void	setConnectionMethod(const CONNECTION_METHOD connection_method) { this->connection_method_ = connection_method; }
};	
} // namespace config

#endif
