#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

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
#endif
