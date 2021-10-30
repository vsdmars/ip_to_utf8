#pragma once

#include <iostream>
#include <optional>
#include <stdio.h>

using std::optional;
using std::string;
using std::to_string;

optional<string> getCidrIpFromUtf8Ip(const std::string &utf8Ip);
optional<string> getJsonIpFromCidrIp(const string &cidrIp);
