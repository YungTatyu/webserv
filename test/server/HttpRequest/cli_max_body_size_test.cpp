#include "cli_max_body_size_test.hpp"

void test::setupMaxBodySize(unsigned long size) {
  ConfigHandler& config_handler = const_cast<ConfigHandler&>(WebServer::getConfigHandler());
  config::Main* config = const_cast<config::Main*>(config_handler.config_);
  config->http_.client_max_body_size_.setSize(size);
}

void test::teardownMaxBodySize() {
  ConfigHandler& config_handler = const_cast<ConfigHandler&>(WebServer::getConfigHandler());
  config::Main* config = const_cast<config::Main*>(config_handler.config_);
  config->http_.client_max_body_size_.setSize(config::ClientMaxBodySize::kDefaultSize_);
}
