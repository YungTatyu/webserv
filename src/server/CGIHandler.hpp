#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <unistd.h>
# include <sys/stat.h>
# include <string>
# include <iostream>
# include <fstream>
# include <cstdlib>
# include <sstream>

class CGIHandler
{
	public:
		static bool isCGI( std::string& requestURI );
		static std::string executeCGI( std::string& uri, std::string& query );
		static std::string getCommandPath(const std::string& command);
		static std::vector<std::string> split(const std::string& s, char delimiter);

	private:
		CGIHandler();
		enum PipeEnds
		{
			READ = 0,
			WRITE = 1
		};
};

#endif

