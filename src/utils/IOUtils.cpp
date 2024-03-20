#include "IOUtils.hpp"
#include <unistd.h>

const static size_t WriteSize = 1024;

ssize_t	IOUtils::wrapperWrite( const int fd, const std::string& msg )
{
	size_t	msg_size = msg.size();
	size_t	written_bytes = 0;

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

