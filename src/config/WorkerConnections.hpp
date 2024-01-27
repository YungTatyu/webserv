#ifndef CONFIG_WORKER_CONNECTIONS_HPP
#define CONFIG_WORKER_CONNECTIONS_HPP

#include "conf.hpp"

namespace config
{
class WorkerConnections
{
	private:
		long long	worker_connections_;
	public:
		WorkerConnections() : worker_connections_(this->kDefaultWorkerConnections_) {}
		~WorkerConnections() {}
		const static unsigned long	kDefaultWorkerConnections_ = 512;
		const long long	&getWorkerConnections() const { return this->worker_connections_; }
		void	setWorkerConnections(const long long &worker_connections) { this->worker_connections_ = worker_connections; }
		const static unsigned int	kType_ = CONF_EVENTS|CONF_TAKE1|CONF_UNIQUE;
};	
} // namespace config

#endif
