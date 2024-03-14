#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>
# include <vector>

namespace FileUtils
{
	bool isDirectory(const std::string& path);
	std::string readFile(const std::string& filePath);
	std::vector<std::string> getDirectoryContents(const std::string& directoryPath);
	bool isExecutable( const char* filename );
	bool isExtensionFile(const std::string& filename, const std::string& extension);
}

#endif
