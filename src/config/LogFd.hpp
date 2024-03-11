#ifndef LOGFD_HPP
#define LOGFD_HPP

# include "Main.hpp"
# include "AccessLog.hpp"
# include "ErrorLog.hpp"

namespace config {

bool	initLogFds( Main& config );
bool	initAcsLogFds( Main& config );
bool	initErrLogFds( Main& config );
int		addAcsFdList ( std::set<std::string>& directives_set, const std::vector<config::AccessLog>& access_log_list, std::vector<int>& fd_list );
int		addErrFdList ( std::set<std::string>& directives_set, const std::vector<config::ErrorLog>& error_log_list, std::vector<int>& fd_list );
void	closeLogFds( const std::vector<int> log_list );
void	terminateLogFds( const Main* config );

}

#endif
