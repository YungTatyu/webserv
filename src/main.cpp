#include "Main.hpp"

#include <string.h>

#include <iostream>

#include "ConfigHandler.hpp"
#include "WebServer.hpp"
#include "conf.hpp"

int main(int ac, char *av[]) {
  if (ac != 2) {
    std::cerr << "Usage: " << av[0] << " [config_file_name]" << std::endl;
    return 1;
  }

  const config::Main *config = config::initConfig(av[1]);
  if (config == NULL) return 1;

  try {
    WebServer server = WebServer(config);
    server.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 1;
}
