#ifndef IOUTILS_HPP
# define IOUTILS_HPP

# include <sys/types.h>
# include <string>

namespace IOUtils
{
	ssize_t wrapperWrite( const int fd, const std::string& msg );
}

#endif
