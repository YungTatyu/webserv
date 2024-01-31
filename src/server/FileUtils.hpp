#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>

namespace FileUtils
{
	bool isDirectory(const std::string& path);
	std::string readFile(const std::string& filePath);
	std::string listDirectory(const std::string& directoryPath);
	bool isExecutable( const char* filename );
	bool isPHPExtension(const std::string& filename);
}

#endif
