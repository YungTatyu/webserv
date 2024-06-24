#ifndef CONFIG_HANDLER_TEST_HPP
#define CONFIG_HANDLER_TEST_HPP

#include <arpa/inet.h>
#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/param.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ConfigHandler.hpp"
#include "Http.hpp"
#include "HttpRequest.hpp"
#include "Lexer.hpp"
#include "LimitExcept.hpp"
#include "Location.hpp"
#include "LogFd.hpp"
#include "NetworkIOHandler.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Utils.hpp"

namespace test {

const static std::string kConfigHandlerTestPath = "test/server/ConfigHandler/";

class ConfigHandlerTest {
 public:
  // テストに使うオブジェクト
  ConfigHandler config_handler_;
  HttpRequest request_;
  struct TiedServer tied_server_;
  std::string conf_path_;

  // 各TESTの前に呼び出されるセットアップメソッド
  ConfigHandlerTest(std::string conf_name)
      : tied_server_(TiedServer("127.0.0.1", 8001)), conf_path_(kConfigHandlerTestPath + conf_name) {
    this->setup();
  }

  ConfigHandlerTest(std::string conf_name, std::string ip, unsigned int port)
      : tied_server_(TiedServer(ip, port)), conf_path_(kConfigHandlerTestPath + conf_name) {
    this->setup();
  }

  void setup() {
    // config作成
    config::Main* config = new config::Main();
    config::Lexer lexer(conf_path_);
    lexer.tokenize();

    config::Parser parser(*config, lexer.getTokens(), conf_path_);
    if (!parser.parse()) {
      FAIL() << "Parse failed" << std::endl;
    }

    // log出力テストはlogファイル作成
    if (this->conf_path_.find("writeAccessLog") != std::string::npos ||
        this->conf_path_.find("writeErrorLog") != std::string::npos) {
      if (!initLogFds(*config)) {
        config_handler_.loadConfiguration(config);
        FAIL() << "initLogFds failed" << std::endl;
      }
    }

    config_handler_.loadConfiguration(config);
  }

  ~ConfigHandlerTest() {
    // テストケースのクリーンアップ処理
    // log出力テストはlogファイル削除
    if (this->conf_path_.find("writeAccessLog") != std::string::npos) {
      config::terminateLogFds(config_handler_.config_);
      unlink("logs/http_access.log");
      unlink("logs/server_access.log");
      unlink("logs/location_access.log");
      unlink("logs/multiple1.log");
      unlink("logs/multiple2.log");
      unlink("logs/multiple3.log");
    } else if (this->conf_path_.find("writeErrorLog") != std::string::npos) {
      config::terminateLogFds(config_handler_.config_);
      unlink("logs/server_error.log");
      unlink("logs/location_error.log");
      unlink("logs/multiple1.log");
      unlink("logs/multiple2.log");
      unlink("logs/multiple3.log");
    }
    delete config_handler_.config_;
  }

  void initRequest(const config::REQUEST_METHOD& method, const std::string& uri,
                   const std::map<std::string, std::string, Utils::CaseInsensitiveCompare>& headers,
                   const std::string& body, const HttpRequest::ParseState parseState) {
    this->request_.method = method;
    this->request_.uri = uri;
    this->request_.headers = headers;
    this->request_.body = body;
    this->request_.parseState = parseState;
  }

  void initTiedServer(std::vector<const config::Server*> server_list) {
    for (auto server : server_list) this->tied_server_.servers_.push_back(server);
  }

  std::string getAbsolutePath(std::string file_path) {
    char absolute_tmp_path[MAXPATHLEN];

    // 絶対pathを取得
    if (realpath(file_path.c_str(), absolute_tmp_path) == NULL) {
      std::cerr << file_path << " is not found." << std::endl;
      exit(EXIT_FAILURE);
    }

    // absolutepath = ~/webserv
    return static_cast<std::string>(absolute_tmp_path);
  }

  static bool getWriteResult(std::string file_path, const std::string& msg) {
    std::ifstream logFile(file_path.c_str());
    if (!logFile.is_open()) {
      return false;
    }

    std::vector<std::string> logContent;
    std::string line;
    while (std::getline(logFile, line, '\0')) {
      logContent.push_back(line);
    }
    for (size_t i = 0; i < logContent.size(); i++) {
      // std::cout << logContent[i] << std::endl;
      if (logContent[i].find(msg) != std::string::npos) return true;
    }
    return false;
  }

  void WRITE_ACCURATE(std::string file_path, const std::string& msg) {
    EXPECT_TRUE(getWriteResult(file_path, msg));
  }

  void WRITE_NONE(std::string file_path, const std::string& msg) {
    EXPECT_FALSE(getWriteResult(file_path, msg));
  }

  void EXPECT_NO_FILE(std::string file_path) {
    std::ifstream logFile(file_path.c_str());
    EXPECT_FALSE(logFile.is_open());
  }

  void sameTime(const config::Time& expect, const config::Time& actual) {
    EXPECT_EQ(expect.time_in_ms_, actual.time_in_ms_);
  }

  struct sockaddr_in createClient(std::string ip) {
    struct sockaddr_in cli_addr;
    std::memset(&cli_addr, 0, sizeof(cli_addr));  // ゼロで初期化
    cli_addr.sin_family = AF_INET;                // IPv4
    cli_addr.sin_addr.s_addr = Utils::StrToIPAddress(ip);
    return cli_addr;
  }

  void sameLocation(const config::Location& expect, const config::Location* actual) {
    if (actual) {
      EXPECT_EQ(expect.uri_, actual->uri_);
      return;
    }
    FAIL() << "can't find location" << std::endl;
  }
};

};  // namespace test
#endif
