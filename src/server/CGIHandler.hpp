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
		static std::string getQueryString( std::string& uri );
		static std::string getScriptPath( std::string& uri );
		static bool isExecutable( const char* filename );
		static bool isPHPExtension(const std::string& filename);
		static std::string get_command_path(const std::string& command);
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

