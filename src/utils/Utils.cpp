#include "Utils.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>


int	Utils::wrapperOpen( const std::string path, int flags, mode_t modes )
{
	int fd = open(path.c_str(), flags, modes);
	if (fd == -1)
	{
		std::cerr << "webserv: [emerg] open() \"" << path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	}
	return fd;
}

int	Utils::wrapperAccess( const std::string path, int modes, bool err_log )
{
	int ret = access(path.c_str(), modes);
	if (ret == -1 && err_log)
	{
		std::cerr << "webserv: [emerg] access() \"" << path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	}
	return ret;
}

bool	Utils::wrapperRealpath( const std::string path, std::string& absolute_path )
{
	char	tmp_path[MAXPATHLEN];
	if (realpath(path.c_str(), tmp_path) == NULL)
	{
		return false;
	}

	absolute_path = static_cast<std::string>(tmp_path);
	return true;
}

bool Utils::isFile( const std::string& path )
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		std::cerr << "webserv: [emerg] stat() \"" << path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}
	return S_ISREG(statbuf.st_mode);
}

bool Utils::isDirectory( const std::string& path )
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		std::cerr << "webserv: [emerg] stat() \"" << path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string Utils::readFile( const std::string& filePath )
{
	std::ifstream file(filePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::vector<std::string> Utils::createDirectoryContents( const std::string& directoryPath )
{
	std::vector<std::string> contents;
	DIR* dir = opendir(directoryPath.c_str());
	// error出力？
	if (dir == NULL)
		return contents;
	struct dirent* entry;

	contents.push_back("../");

	while ((entry = readdir(dir)) != NULL)
	{
		std::string filename = entry->d_name;
		if (filename != "." && filename != "..")
		{
			if (Utils::isDirectory(directoryPath + "/" + filename))
				filename += "/";
			contents.push_back(filename);
		}
	}
	closedir(dir);

	return contents;
}

bool Utils::isExecutable( const char* filename )
{
	struct stat sbuf;
	if ( stat(filename, &sbuf) < 0 )
		return false;
	return S_ISREG(sbuf.st_mode) && (S_IXUSR & sbuf.st_mode);
}

bool Utils::isExtensionFile( const std::string& filename, const std::string& extension )
{
	if ( filename.length() < extension.length() )
		return false;
	return std::equal( extension.begin(), extension.end(), filename.end() - extension.length() );
}

ssize_t	Utils::wrapperWrite( const int fd, const std::string& msg )
{
	size_t	msg_size = msg.size();
	size_t	written_bytes = 0;
	const size_t WriteSize = 1024;

	while ( written_bytes < msg_size )
	{
		size_t write_size = std::min(WriteSize, msg_size - written_bytes);
		std::string chunk = msg.substr(written_bytes, write_size);

		ssize_t ret = write(fd, chunk.c_str(), chunk.size());
		if (ret == -1)
			return -1;

		written_bytes += ret;
	}
	return written_bytes;
}

const std::string	Utils::ipToStr( const uint32_t ip )
{
	std::stringstream	ss;

	#if defined(__LITTLE_ENDIAN__)
	ss << ((ip >> 24) & 0xFF) << '.'  // 第1オクテット
		<< ((ip >> 16) & 0xFF) << '.' // 第2オクテット
		<< ((ip >> 8) & 0xFF) << '.'  // 第3オクテット
		<< (ip & 0xFF);               // 第4オクテット
	#else
	ss << (ip & 0xFF) << '.'  // 第1オクテット
		<< ((ip >> 8) & 0xFF) << '.' // 第2オクテット
		<< ((ip >> 16) & 0xFF) << '.'  // 第3オクテット
		<< ((ip >> 24) & 0xFF);               // 第4オクテット
	#endif
	return ss.str();
}

uint32_t	Utils::StrToIPAddress( const std::string& ip)
{
	std::istringstream iss(ip);
	std::string segment;
	std::vector<std::string> segments;

	// "." で分割
	while (std::getline(iss, segment, '.')) {
		segments.push_back(segment);
	}

	uint32_t	result = 0;

	for (int i = 0; i < 4; i++)
	{
		iss.clear();
		iss.str(segments[i]);
		int value;
		iss >> value;

		result = (result << 8) | value;
	}

	return result;
}

