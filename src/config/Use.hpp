#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

namespace config
{
class Use
{
	private:
		enum CONNECTION_METHOD
		{
			EPOLL,
			KQUEUE,
			POLL,
			SELECT,
		};
		CONNECTION_METHOD	connection_method;
	public:
		Use();
		~Use();
};	
} // namespace config

#endif
