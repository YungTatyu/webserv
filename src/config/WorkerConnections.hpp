#ifndef CONFIG_WORKER_CONNECTIONS_HPP
#define CONFIG_WORKER_CONNECTIONS_HPP

namespace config
{
class WorkerConnections
{
	private:
		unsigned long	worker_connections_;
	public:
		WorkerConnections();
		~WorkerConnections();
		const static unsigned long	kDefaultWorkerConnections = 512;
		const unsigned long	&getWorkerConnections() const { return this->worker_connections_; }
		void	setWorkerConnections(const unsigned long &worker_connections) { this->worker_connections_ = worker_connections; }
};	
} // namespace config

#endif