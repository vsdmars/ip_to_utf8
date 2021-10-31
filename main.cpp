#include <header.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv) {

  CLI::App app{"App description"};
  std::string filename = "TESTO";
  app.add_option("-f,--file", filename, "A help string");

  CLI11_PARSE(app, argc, argv);

  // std::cout << *getJsonIpFromCidrIp("192.168.1.1") << std::endl;
  // std::cout << *getCidrIpFromUtf8Ip(*getJsonIpFromCidrIp("192.168.1.1"))
  // << std::endl;
  return 0;
}
