#pragma once

#include <iostream>
#include <optional>
#include <stdio.h>

using std::optional;
using std::string;
using std::to_string;

optional<string> genIPFromUTF8(string ip);
optional<string> genUTF8Str(const string &cidrIp);
