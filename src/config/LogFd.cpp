#include "LogFd.hpp"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

#include "syscall_wrapper.hpp"
#include "utils.hpp"

static const std::string kAccessLog = "access_log";
static const std::string kAccessFd = "access_fd";
static const std::string kErrorLog = "error_log";
static const std::string kErrorFd = "error_fd";

/*
 * AddAcsFdList/AddErrFdListの返り値
 *
 * 1:  fdを追加する
 * 0:  log directiveがないので、fdを追加しない
 * -1: error
 */
int config::addAcsFdList(std::set<std::string>& directives_set,
                         const std::vector<config::AccessLog>& access_log_list, std::vector<int>& fd_list) {
  // access_log directiveがなければ飛ばす。
  if (directives_set.find(kAccessLog) == directives_set.end()) return 0;

  std::string tmp_path;
  int tmp_fd;

  for (size_t i = 0; i < access_log_list.size(); i++) {
    // access_logはそのコンテキスト中にoffがあればすべて無効
    if (!access_log_list[i].getIsAccesslogOn()) {
      fd_list.clear();
      return 1;
    }
    tmp_path = access_log_list[i].getFile();
    // ファイルはあるが、write権限がない時ときは飛ばす
    // ここのエラー出力任意にできるようにする。でないと、ファイルがない時は毎回accessエラーでる
    if (syscall_wrapper::Access(tmp_path, F_OK, false) == 0 &&
        syscall_wrapper::Access(tmp_path, W_OK, false) == -1)
      continue;
    // openするのはそのディレクティブにエラーやoffがないことがわかってからの方が無駄なファイルつくらなくて済む
    tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return -1;
    }
    fd_list.push_back(tmp_fd);
  }
  directives_set.insert(kAccessFd);
  return 1;
}

int config::addErrFdList(std::set<std::string>& directives_set,
                         const std::vector<config::ErrorLog>& error_log_list, std::vector<int>& fd_list) {
  // error_log directiveがなければ飛ばす。
  if (directives_set.find(kErrorLog) == directives_set.end()) return 0;

  std::string tmp_path;
  int tmp_fd;

  for (size_t i = 0; i < error_log_list.size(); i++) {
    tmp_path = error_log_list[i].getFile();
    // ファイルはあるが、write権限がない時ときは飛ばす
    // ここのエラー出力任意にできるようにする。でないと、ファイルがない時は毎回accessエラーでる
    if (syscall_wrapper::Access(tmp_path, F_OK, false) == 0 &&
        syscall_wrapper::Access(tmp_path, W_OK, false) == -1)
      continue;
    tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return -1;
    }
    fd_list.push_back(tmp_fd);
  }

  directives_set.insert(kErrorFd);
  return 1;
}

/* 各コンテキストのaccess_logディレクティブで指定されたファイルのfdを取得する
 */
bool config::initAcsLogFds(config::Main& config) {
  // http context
  int ret =
      addAcsFdList(config.http_.directives_set_, config.http_.access_log_list_, config.http_.access_fd_list_);
  if (ret == -1)
    return false;
  else if (ret == 0) {
    std::string absolute_path;
    if (!utils::resolvePath(".", absolute_path)) {
      std::cerr << "webserv: [emerg] realpath() \""
                << "."
                << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
      return false;
    }
    std::string tmp_path = absolute_path + "/" + static_cast<std::string>(config::AccessLog::kDefaultFile_);
    int tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return false;
    }
    config.http_.access_fd_list_.push_back(tmp_fd);
    config.http_.directives_set_.insert(kAccessFd);
  }

  // server context
  for (size_t si = 0; si < config.http_.server_list_.size(); si++) {
    if (addAcsFdList(config.http_.server_list_[si].directives_set_,
                     config.http_.server_list_[si].access_log_list_,
                     config.http_.server_list_[si].access_fd_list_) == -1)
      return false;

    // location context
    for (size_t li = 0; li < config.http_.server_list_[si].location_list_.size(); li++) {
      if (addAcsFdList(config.http_.server_list_[si].location_list_[li].directives_set_,
                       config.http_.server_list_[si].location_list_[li].access_log_list_,
                       config.http_.server_list_[si].location_list_[li].access_fd_list_) == -1)
        return false;
    }
  }

  return true;
}

/* 各コンテキストのerror_logディレクティブで指定されたファイルのfdを取得する
 */
bool config::initErrLogFds(config::Main& config) {
  // main context
  int ret = addErrFdList(config.directives_set_, config.error_log_list_, config.error_fd_list_);
  if (ret == -1)
    return false;
  else if (ret == 0) {
    std::string absolute_path;
    if (!utils::resolvePath(".", absolute_path)) {
      std::cerr << "webserv: [emerg] realpath() \""
                << "."
                << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
      return false;
    }
    std::string tmp_path = absolute_path + "/" + static_cast<std::string>(config::ErrorLog::kDefaultFile_);
    int tmp_fd = openLogFd(tmp_path);
    if (tmp_fd == -1) {
      return false;
    }
    config.error_fd_list_.push_back(tmp_fd);
    config.directives_set_.insert(kErrorFd);
  }

  // http context
  if (addErrFdList(config.http_.directives_set_, config.http_.error_log_list_, config.http_.error_fd_list_) ==
      -1)
    return false;

  // server context
  for (size_t si = 0; si < config.http_.server_list_.size(); si++) {
    if (addErrFdList(config.http_.server_list_[si].directives_set_,
                     config.http_.server_list_[si].error_log_list_,
                     config.http_.server_list_[si].error_fd_list_) == -1)
      return false;

    // location context
    for (size_t li = 0; li < config.http_.server_list_[si].location_list_.size(); li++) {
      if (addErrFdList(config.http_.server_list_[si].location_list_[li].directives_set_,
                       config.http_.server_list_[si].location_list_[li].error_log_list_,
                       config.http_.server_list_[si].location_list_[li].error_fd_list_) == -1)
        return false;
    }
  }

  return true;
}

bool config::initLogFds(config::Main& config) {
  if (!initAcsLogFds(config) || !initErrLogFds(config)) return false;
  return true;
}

void config::closeLogFds(const std::vector<int>& log_list) {
  for (size_t i = 0; i < log_list.size(); i++) close(log_list[i]);
}

void config::terminateLogFds(const config::Main* config) {
  // main context
  closeLogFds(config->error_fd_list_);

  // http context
  closeLogFds(config->http_.access_fd_list_);
  closeLogFds(config->http_.error_fd_list_);

  // server context
  for (size_t i = 0; i < config->http_.server_list_.size(); i++) {
    closeLogFds(config->http_.server_list_[i].access_fd_list_);
    closeLogFds(config->http_.server_list_[i].error_fd_list_);

    for (size_t j = 0; j < config->http_.server_list_[i].location_list_.size(); j++) {
      closeLogFds(config->http_.server_list_[i].location_list_[j].access_fd_list_);
      closeLogFds(config->http_.server_list_[i].location_list_[j].error_fd_list_);
    }
  }
}

int config::openLogFd(const std::string& log_path) {
  return syscall_wrapper::Open(log_path, O_WRONLY | O_APPEND | O_CREAT | O_NONBLOCK | O_CLOEXEC,
                               S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
}
