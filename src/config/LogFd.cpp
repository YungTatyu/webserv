#include "LogFd.hpp"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>

#include "error.hpp"
#include "syscall_wrapper.hpp"
#include "utils.hpp"

static const std::string kAccessLog = "access_log";
static const std::string kErrorLog = "error_log";
static const mode_t kLogFileMode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;
const int kLogFileFlags = O_WRONLY | O_APPEND | O_CREAT | O_NONBLOCK | O_CLOEXEC;

/*
 * AddAcsFdList/AddErrFdListの返り値
 *
 * 1:  fdを追加する
 * 0:  log directiveがないので、fdを追加しない
 * -1: error
 */
int config::addAcsFdList(std::set<std::string>& directives_set,
                         std::vector<config::AccessLog>& access_log_list) {
  // access_log directiveがなければ飛ばす。
  if (directives_set.find(kAccessLog) == directives_set.end()) return 0;

  std::string tmp_path;
  int tmp_fd;

  // access_logはそのコンテキスト中にoffがあればすべて無効
  for (size_t i = 0; i < access_log_list.size(); i++) {
    if (!access_log_list[i].getIsAccesslogOn()) {
      // directive_setからaccess_log削除して次のコンテキストに進む
      directives_set.erase(directives_set.find(kAccessLog));
      return 1;
    }
  }

  // offがなければfd作る
  for (size_t i = 0; i < access_log_list.size(); i++) {
    tmp_path = access_log_list[i].getFile();
    if (!checkFileAccess(tmp_path)) return -1;
    tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) return -1;
    access_log_list[i].setFd(tmp_fd);
  }
  return 1;
}

int config::addErrFdList(std::set<std::string>& directives_set,
                         std::vector<config::ErrorLog>& error_log_list) {
  // error_log directiveがなければ飛ばす。
  if (directives_set.find(kErrorLog) == directives_set.end()) return 0;

  std::string tmp_path;
  int tmp_fd;

  for (size_t i = 0; i < error_log_list.size(); i++) {
    tmp_path = error_log_list[i].getFile();
    if (!checkFileAccess(tmp_path)) return -1;
    tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) return -1;
    error_log_list[i].setFd(tmp_fd);
  }

  return 1;
}

/* 各コンテキストのaccess_logディレクティブで指定されたファイルのfdを取得する
 */
bool config::initAcsLogFds(config::Main& config) {
  // http context
  int ret = addAcsFdList(config.http_.directives_set_, config.http_.access_log_list_);
  if (ret == -1)
    return false;
  else if (ret == 0) {
    std::string absolute_path;
    if (!utils::resolvePath(".", absolute_path)) {
      std::cerr << error::strSysCallError("realpath") << std::endl;
      return false;
    }
    std::string tmp_path = absolute_path + "/" + static_cast<std::string>(config::AccessLog::kDefaultFile_);
    int tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return false;
    }
    config::AccessLog tmp_acs_log;
    tmp_acs_log.setFd(tmp_fd);
    config.http_.access_log_list_.push_back(tmp_acs_log);
  }

  // server context
  for (size_t si = 0; si < config.http_.server_list_.size(); si++) {
    if (addAcsFdList(config.http_.server_list_[si].directives_set_,
                     config.http_.server_list_[si].access_log_list_) == -1)
      return false;

    // location context
    for (size_t li = 0; li < config.http_.server_list_[si].location_list_.size(); li++) {
      if (addAcsFdList(config.http_.server_list_[si].location_list_[li].directives_set_,
                       config.http_.server_list_[si].location_list_[li].access_log_list_) == -1)
        return false;
    }
  }

  return true;
}

/* 各コンテキストのerror_logディレクティブで指定されたファイルのfdを取得する
 */
bool config::initErrLogFds(config::Main& config) {
  // main context
  int ret = addErrFdList(config.directives_set_, config.error_log_list_);
  if (ret == -1)
    return false;
  else if (ret == 0) {
    std::string absolute_path;
    if (!utils::resolvePath(".", absolute_path)) {
      std::cerr << error::strSysCallError("realpath") << std::endl;
      return false;
    }
    std::string tmp_path = absolute_path + "/" + static_cast<std::string>(config::ErrorLog::kDefaultFile_);
    int tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return false;
    }
    config::ErrorLog tmp_err_log;
    tmp_err_log.setFd(tmp_fd);
    config.error_log_list_.push_back(tmp_err_log);
  }

  // http context
  if (addErrFdList(config.http_.directives_set_, config.http_.error_log_list_) == -1) return false;

  // server context
  for (size_t si = 0; si < config.http_.server_list_.size(); si++) {
    if (addErrFdList(config.http_.server_list_[si].directives_set_,
                     config.http_.server_list_[si].error_log_list_) == -1)
      return false;

    // location context
    for (size_t li = 0; li < config.http_.server_list_[si].location_list_.size(); li++) {
      if (addErrFdList(config.http_.server_list_[si].location_list_[li].directives_set_,
                       config.http_.server_list_[si].location_list_[li].error_log_list_) == -1)
        return false;
    }
  }

  return true;
}

bool config::initLogFds(config::Main& config) {
  if (!initAcsLogFds(config) || !initErrLogFds(config)) return false;
  return true;
}

void config::terminateLogFds(const config::Main* config) {
  // main context
  closeLogFds(config->error_log_list_);

  // http context
  closeLogFds(config->http_.access_log_list_);
  closeLogFds(config->http_.error_log_list_);

  // server context
  for (size_t i = 0; i < config->http_.server_list_.size(); i++) {
    closeLogFds(config->http_.server_list_[i].access_log_list_);
    closeLogFds(config->http_.server_list_[i].error_log_list_);

    for (size_t j = 0; j < config->http_.server_list_[i].location_list_.size(); j++) {
      closeLogFds(config->http_.server_list_[i].location_list_[j].access_log_list_);
      closeLogFds(config->http_.server_list_[i].location_list_[j].error_log_list_);
    }
  }
}

int config::openLogFd(const std::string& log_path) {
  return syscall_wrapper::Open(log_path, kLogFileFlags, kLogFileMode);
}

bool config::checkFileAccess(const std::string& path) {
  // ファイルはあるが、write権限がない時ときはerror
  if (syscall_wrapper::Access(path, F_OK, false) == 0 && syscall_wrapper::Access(path, W_OK, true) == -1) {
    std::cerr << error::strSysCallError("access", path) << std::endl;
    return false;
  }
  return true;
}
