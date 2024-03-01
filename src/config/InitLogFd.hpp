#ifndef INIT_LOG_FD_HPP
#define INIT_LOG_FD_HPP

# include "Main.hpp"
# include "AccessLog.hpp"
# include "ErrorLog.hpp"

namespace config {

bool	initLogFds( Main& config );
bool	initAcsLogFds( Main& config );
bool	initErrLogFds( Main& config );
bool	addAcsFdList( std::vector<int>& fd_list, const std::vector<AccessLog>& access_log_list );
bool	addErrFdList( std::vector<int>& fd_list, const std::vector<ErrorLog>& error_log_list );

}

#endif
