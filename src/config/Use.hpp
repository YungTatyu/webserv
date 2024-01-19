#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

namespace config
{
class Use
{
	private:
		enum CONNECTION_METHOD
		{
			SELECT,
			POLL,
			KQUEUE,
			EPOLL,
		};
		CONNECTION_METHOD	connection_method_;
	public:
		Use();
		~Use();
		const CONNECTION_METHOD	getConnectionMethod() const;
		void	setConnectionMethod();
};	
} // namespace config

#endif
