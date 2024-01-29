#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <unistd.h>
# include <sys/stat.h>
# include <string>
# include <iostream>
# include <fstream>

class CGIHandler
{
	public:
		static bool isCGI( std::string& requestURI);
		static std::string executeCGI( std::string& uri);
		static std::string getQueryString( std::string& uri );
		static std::string getScriptPath( std::string& uri );
		static bool isFileExistAndExecutable( const char* filename );
		static bool isPHPExtension(const std::string& filename);

	private:
		CGIHandler();
		enum PipeEnds
		{
			READ = 0,
			WRITE = 1
		};
};

#endif

