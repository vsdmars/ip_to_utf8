#include <header.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <arpa/inet.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {

  // std::cout << *genUTF8Str("192.168.1.1") << std::endl;
  // std::cout << *genIPFromUTF8(*genUTF8Str("192.168.1.1")) << std::endl;

  auto to_lower_ip = [](string input) { return CLI::detail::to_lower(input); };
  auto validate_ip = [](const string &input) {
    in_addr ipv4;
    in6_addr ipv6;

    if (inet_pton(AF_INET6, input.c_str(), &ipv6) == 1) {
      return genUTF8Str(input).value_or("");
    } else if (inet_pton(AF_INET, input.c_str(), &ipv4) == 1) {
      return genUTF8Str(input).value_or("");
    } else {
      cout << "input: " << input << "\n" << flush;
      return genIPFromUTF8(input).value_or("");
    }
  };

  CLI::App app{"iucli converts CIDR IP string to UTF8 format and vice versa."};

  string input;
  app.add_option("conversion input", input,
                 "string to be converted either to IP or UTF8")
      ->required()
      ->allow_extra_args(false)
      ->transform(to_lower_ip)
      ->check(validate_ip);

  CLI11_PARSE(app, argc, argv);
  cout << "result: " << input << '\n' << flush;
  return 0;
}
