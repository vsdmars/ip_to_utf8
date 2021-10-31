#include <header.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <arpa/inet.h>

#include <cstdio>
#include <iostream>
#include <string>

using std::cout;
using std::flush;
using std::string;

int main(int argc, char **argv) {

  auto to_lower_ip = [](string input) { return CLI::detail::to_lower(input); };

  auto convert = [](const string &input) {
    in_addr ipv4;
    in6_addr ipv6;

    if (inet_pton(AF_INET6, input.c_str(), &ipv6) == 1) {
      return genUTF8Str(input).value_or("");
    } else if (inet_pton(AF_INET, input.c_str(), &ipv4) == 1) {
      return genUTF8Str(input).value_or("");
    } else {
      return genIPFromUTF8(input).value_or("");
    }
  };

  auto validate_ip = [](const string &input) {
    in_addr ipv4;
    in6_addr ipv6;

    if (inet_pton(AF_INET6, input.c_str(), &ipv6) == 1) {
      return string{};
    } else if (inet_pton(AF_INET, input.c_str(), &ipv4) == 1) {
      return string{};
    } else {

      if (input.find(R"(\)") == std::string::npos) {
        throw CLI::ValidationError{
            "input validate failed",
            "please quote input string as argument for iuctl."};
      }

      return string{};
    }
  };

  CLI::App app{"iuctl converts CIDR IP string to UTF8 format and vice versa."};

  string input;
  app.add_option("input", input, "string to be converted either to IP or UTF8")
      ->required()
      ->allow_extra_args(false)
      ->transform(to_lower_ip)
      ->check(validate_ip);

  CLI11_PARSE(app, argc, argv);

  cout << convert(input) << "\n" << flush;

  return 0;
}
