#ifndef CONFIG_USERID_SERVICE_HPP
#define CONFIG_USERID_SERVICE_HPP

namespace config
{
class UseridService
{
	private:
		unsigned long	userid_service_;
	public:
		UseridService() {}
		~UseridService() {}
		const unsigned long	getUseridService() const;
		void	setUseridService(const unsigned long userid_service);
};
} // namespace config

#endif