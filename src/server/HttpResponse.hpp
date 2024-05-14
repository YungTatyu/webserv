#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "NetworkIOHandler.hpp"
#include "Utils.hpp"

class HttpResponse {
 public:
  // initializer
  HttpResponse();

  // enum
  enum RES_STATE {
    RES_CREATING_STATIC,
    RES_EXECUTE_CGI,
    RES_PARSED_CGI,
    RES_CGI_ERROR
  };

  enum ResponsePhase {
    sw_start_phase = 0,
    sw_pre_search_location_phase,
    sw_search_location_phase,
    sw_post_search_location_phase,
    sw_return_phase,
    sw_allow_phase,
    sw_uri_check_phase,
    sw_search_res_file_phase,
    sw_content_phase,
    sw_error_page_phase,
    sw_log_phase,
    sw_end_phase
  };

  // member methods
  static std::string generateResponse(HttpRequest& request, HttpResponse& response,
                                      const struct TiedServer& tied_servers, const int client_sock,
                                      const ConfigHandler& config_handler);

  // public variables
  std::string root_path_;
  std::string res_file_path_;
  RES_STATE state_;
  std::string status_code_line_;
  long status_code_;  // response生成するときにstatus_line_map_参照する
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers_;
  std::string body_;
  static std::map<int, std::string> status_line_map_;  // status codeに基づくstatus lineを格納するmap
  static std::map<int, const std::string*> default_error_page_map_;  // defaultのerror pageを格納するmap

 private:
  // private member
  size_t internal_redirect_cnt_;
  static const size_t kMaxInternalRedirect = 10;

  static std::string createResponse(const HttpResponse& response);
  // handle phase methods
  static ResponsePhase handlePreSearchLocationPhase(const HttpRequest::ParseState parse_state,
                                                    HttpResponse& response, const int client_sock,
                                                    struct sockaddr_in& client_addr);
  static ResponsePhase handleSearchLocationPhase(HttpResponse& response, const HttpRequest& request,
                                                 const config::Server& server,
                                                 const config::Location** location,
                                                 const ConfigHandler& config_handler);
  static ResponsePhase handleAllowPhase(HttpResponse& response, const HttpRequest& request,
                                        const config::Server& server, const config::Location* location,
                                        struct sockaddr_in client_addr, const ConfigHandler& config_handler);
  static ResponsePhase handleReturnPhase(HttpResponse& response, const config::Location* location,
                                         const ConfigHandler& config_handler);
  static ResponsePhase handleUriCheckPhase(HttpResponse& response, const HttpRequest& request,
                                           const config::Server& server, const config::Location* location);
  static ResponsePhase handleSearchResFilePhase(HttpResponse& response, HttpRequest& request,
                                                const config::Server& server,
                                                const config::Location* location,
                                                const ConfigHandler& config_handler);
  static ResponsePhase handleContentPhase(HttpResponse& response, HttpRequest& request);
  static ResponsePhase handleErrorPagePhase(HttpResponse& response, HttpRequest& request,
                                            const config::Server& server, const config::Location* location,
                                            const ConfigHandler& config_handler);

  // 名前微妙
  static std::string autoIndex(const std::string& directory_path, const std::string& index_dir);
  static std::string createCurrentGmtTime();
  static ResponsePhase returnPhase(HttpResponse& response, const config::Location* location);
  static void prepareReturn(HttpResponse& response, const config::Return& return_directive);
  static ResponsePhase searchResPath(HttpResponse& response, HttpRequest& request,
                                     const config::Server& server, const config::Location* location,
                                     const ConfigHandler& config_handler);
  static ResponsePhase Index(HttpResponse& response, HttpRequest& request,
                             const std::vector<config::Index>& index_list, bool is_autoindex_on);
  static ResponsePhase TryFiles(HttpResponse& response, HttpRequest& request,
                                const config::TryFiles& try_files);
  static void headerFilterPhase(HttpResponse& response, const config::Time& time);
  static std::string detectContentType(const std::string& res_file_path);
  static std::string transformLetter(const std::string& key_str);
  static char lastChar(const std::string& str);
};

#endif
