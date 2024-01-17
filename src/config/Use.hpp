#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

namespace config
{
class Use
{
	public:
		enum CONNECTION_METHOD
		{
			SELECT,
			POLL,
			KQUEUE,
			EPOLL,
		};
		Use();
		~Use();
		const CONNECTION_METHOD	getConnectionMethod() const;
		void	setConnectionMethod();
	private:
		CONNECTION_METHOD	connection_method_;
};	
} // namespace config

#endif
