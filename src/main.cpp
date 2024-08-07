#include "Main.hpp"

#include <string.h>

#include <iostream>

#include "ConfigHandler.hpp"
#include "WebServer.hpp"
#include "conf.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " [config_file_name]" << std::endl;
    return 1;
  }

  const config::Main *config = config::initConfig(argv[1]);
  if (config == NULL) return 1;

  try {
    WebServer server(config);
    server.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  delete config;
  return 1;
}
