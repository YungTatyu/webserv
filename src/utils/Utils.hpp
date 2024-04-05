#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <vector>
# include <algorithm>
# include <stdint.h>
#include <netinet/in.h>

namespace Utils
{
// 大文字小文字を区別しない文字列比較関数
struct CaseInsensitiveCompare
{
	bool	operator()(std::string lhs, std::string rhs) const
	{
		std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
		std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::tolower);
		return lhs < rhs;
	}
};

int	wrapperOpen(const std::string path, int flags, mode_t modes );
int	wrapperAccess( const std::string path, int modes, bool err_log );
bool	wrapperRealpath( const std::string path, std::string& absolute_path );
bool	isFile( const std::string& path );
bool	isDirectory( const std::string& path );
std::string	readFile( const std::string& filePath );
std::vector<std::string>	createDirectoryContents( const std::string& directoryPath );
bool	isExecutable( const char* filename );
bool	isExtensionFile( const std::string& filename, const std::string& extension );
ssize_t	wrapperWrite( const int fd, const std::string& msg );
bool	wrapperGetsockname(struct sockaddr_in& addr, const int sock);
std::string	socketToStrIPAddress( const int sock );
std::string	ipToStr( const uint32_t ip );
uint32_t	StrToIPAddress( const std::string& ip);
int	resolveConnectedPort(const int sock);
std::string	toLower(std::string str);
bool	isSpace(const unsigned char ch);
template<typename T>std::string	toStr(const T value);
}

template<typename T>
std::string	Utils::toStr(const T value)
{
	std::stringstream	converter;
	converter << value;
	return converter.str();
}

#endif
