#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "NetworkIOHandler.hpp"
#include "utils.hpp"

class HttpResponse {
 public:
  enum RES_STATE {
    RES_CREATING_STATIC = 0,
    RES_EXECUTE_CGI,
    RES_PARSED_CGI,
    RES_CGI_ERROR,
    RES_CGI_TIMEOUT,
    RES_CGI_EXIT_FAILURE,
    RES_COMPLETE
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

  HttpResponse();
  HttpResponse(const HttpResponse&);
  ~HttpResponse();
  HttpResponse& operator=(const HttpResponse&);
  static std::string generateResponse(HttpRequest& request, HttpResponse& response,
                                      const struct TiedServer& tied_servers, int socket,
                                      const ConfigHandler& config_handler);
  static bool isKeepaliveConnection(const HttpResponse& response);
  static bool isErrorResponse(const HttpResponse& response);
  static void setup();
  void clear();

  std::string root_path_;
  std::string res_file_path_;
  std::string path_info_;
  RES_STATE state_;
  std::string status_code_line_;
  long status_code_;  // response生成するときにstatus_line_map_参照する
  std::map<std::string, std::string, utils::CaseInsensitiveCompare> headers_;
  std::string body_;
  static std::map<int, const char*> status_line_map_;  // status codeに基づくstatus lineを格納するmap
  static std::map<int, const char*> default_error_page_map_;  // defaultのerror pageを格納するmap

 private:
  std::string createResponse(config::REQUEST_METHOD method) const;
  static ResponsePhase handlePreSearchLocationPhase(HttpRequest::ParseState parse_state,
                                                    HttpResponse& response, int socket,
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
  static ResponsePhase handleUriCheckPhase(HttpResponse& response, HttpRequest& request,
                                           const config::Location* location, unsigned int request_port);
  static ResponsePhase handleSearchResFilePhase(HttpResponse& response, HttpRequest& request,
                                                const config::Server& server,
                                                const config::Location* location,
                                                const ConfigHandler& config_handler);
  static ResponsePhase handleContentPhase(HttpResponse& response, HttpRequest& request);
  static ResponsePhase handleErrorPagePhase(HttpResponse& response, HttpRequest& request,
                                            const config::Server& server, const config::Location* location,
                                            const ConfigHandler& config_handler);
  static std::string autoIndex(const std::string& directory_path, const std::string& index_dir);
  static std::string createCurrentGmtTime();
  static ResponsePhase returnPhase(HttpResponse& response, const config::Location* location);
  static void prepareReturn(HttpResponse& response, const config::Return& return_directive);
  static ResponsePhase searchResPath(HttpResponse& response, HttpRequest& request,
                                     const config::Server& server, const config::Location* location,
                                     const ConfigHandler& config_handler);
  static ResponsePhase Index(HttpResponse& response, std::string& request,
                             const std::vector<config::Index>& index_list, bool is_alias,
                             bool is_autoindex_on);
  static ResponsePhase TryFiles(HttpResponse& response, HttpRequest& request,
                                const config::TryFiles& try_files);
  static void headerFilterPhase(HttpResponse& response, const config::Time& time);
  static std::string detectContentType(const std::string& res_file_path);
  static std::string transformLetter(const std::string& key_str);
  static char lastChar(const std::string& str);
  int getStatusCode() const;
  void setStatusCode(int code);
  static bool isAccessibleFile(const std::string& file_path);
  static bool isExecutable(const std::string& file_path);
  static bool setPathinfoIfValidCgi(HttpResponse& response, HttpRequest& request);
  void separatePathinfo(const std::string& uri, size_t pos);

  size_t internal_redirect_cnt_;
  static const size_t kMaxInternalRedirect = 10;
};

#endif
