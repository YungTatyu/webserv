#ifndef CONFIG_WORKER_CONNECTIONS_HPP
#define CONFIG_WORKER_CONNECTIONS_HPP

class WorkerConnections
{
	private:
		unsigned int	connections;
	public:
		WorkerConnections();
		~WorkerConnections();
		const static unsigned int	kDefaultConnections = 512;
};

#endif