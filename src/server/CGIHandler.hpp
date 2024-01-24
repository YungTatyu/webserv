#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <unistd.h>
# include <string>

class CGIHandler
{
	public:
		static bool isCGI( std::string& requestURI);
		static std::string executeCGI();

	private:
		enum PipeEnds
		{
			READ = 0,
			WRITE = 1
		};
};

#endif

