#include <iostream>
#include <stdio.h>

using std::string;
using std::to_string;
using namespace rapidjson;

string sanitizedCharacters[256];
pthread_once_t sanitizedInitControl = PTHREAD_ONCE_INIT;
// Function initializeSanitizedCharacters() and sanitizeNonAsciiInput()
// these two functions work together to sanitize a string that has been
// processed
// by RapidJson. Don't use it on strings that still contains need-to-escape
// characters sucha as \b, \n, \ we did experiment and confirmed that RapidJson
// doesn't sanitized code above 127 but does that for 0x01 to 0x1F, and escape
// everything that needs escape so don't worry, there won't be anything needs
// escape
void initializeSanitizedCharacters() {
  string sanitizedSequence;
  char sanitizedSequenceBuf[16];
  for (size_t ch = 0; ch < 128; ++ch) {
    sanitizedCharacters[ch] = ch;
  }
  for (size_t ch = 128; ch < 256; ++ch) {
    snprintf(sanitizedSequenceBuf, sizeof(sanitizedSequenceBuf), "\\u%04x", ch);
    sanitizedSequence = sanitizedSequenceBuf;
    sanitizedCharacters[ch] = sanitizedSequence;
  }
}

const char *ValueTypeString[] = {"NULL", "STRING", "BOOL",   "INT",
                                 "UINT", "INT64",  "UINT64", "DOUBLE"};

string getPrintableValue(const Value &val) {
  Type type = val.GetType();
  string s("");

  switch (type) {
  case kNullType:
    s = "null";
    break;
  case kFalseType:
    s = "false";
    break;
  case kTrueType:
    s = "true";
    break;
  case kObjectType:
    s = "Object";
    break;
  case kArrayType:
    s = "Array";
    break;
  case kStringType:
    s = "String \"";
    s.append(val.GetString());
    s.append("\"");
    break;
  case kNumberType:
    s = "Number ";
    if (val.IsInt64()) {
      s.append(std::to_string((long long)(val.GetInt64())));
    } else if (val.IsDouble()) {
      s.append(std::to_string((long double)(val.GetDouble())));
    } else {
      s.append("Unknown Type");
    }
    break;
  default:
    s = "Unknown Object type";
  }
  return s;
}

// http://www.utf8-chartable.de/   UTF8  0~255 table
IpVer getCidrIpFromUtf8Ip(const std::string &utf8Ip, std::string &res) {
  int i = 0, ipIndex = 0;
  IpVer retVal = IPVER_UNKNOWN;
  unsigned char ipV6[16] = {0};
  if (utf8Ip.size() < 16) { // 192.168.232.121     only 15 chars
    return retVal;
  }
  while (i < utf8Ip.size()) {
    unsigned char val = (unsigned char)utf8Ip[i];
    if (val == 0xC3) { // e.g., rapidJson will read \u00FF  as 0xC3 0xBF
      ipV6[ipIndex] = (unsigned char)utf8Ip[i + 1];
      ipV6[ipIndex] += 0x40; // the diff between UTF8 code and the original
                             // code, e.g., 0xBF->0xFF, 0xBE->0xFE
      i += 2;
    } else if (val == 0xC2) {
      ipV6[ipIndex] = (unsigned char)utf8Ip[i + 1];
      i += 2;
    } else if (val <= 0x7F) {
      ipV6[ipIndex] = utf8Ip[i];
      ++i;
    } else {
      return retVal;
    }
    ++ipIndex;
  } // end while

  bool areFirstTenBytesZero = true;
  for (int j = 0; j < 10; ++j) {
    if (ipV6[j] != 0) {
      areFirstTenBytesZero = false;
      break;
    }
  }
  res = "";
  if (areFirstTenBytesZero && ipV6[10] == 0xFF && ipV6[11] == 0xFF) { // ipV4
    retVal = IPVER_V4;
    res = to_string((long long unsigned int)ipV6[12]) + '.' +
          to_string((long long unsigned int)ipV6[13]) + '.' +
          to_string((long long unsigned int)ipV6[14]) + '.' +
          to_string((long long unsigned int)ipV6[15]);
  } else {
    char hexRep[8];
    retVal = IPVER_V6;
    for (int j = 0; j < 16; j += 2) {
      unsigned short val = ipV6[j];
      val = (val << 8) | ipV6[j + 1];
      snprintf(hexRep, 8, "%x", val);
      res += hexRep;
      res += ':';
    } // end for
    // remove the trailing ':'
    res.erase(res.size() - 1, 1);
  }

  return retVal;
} // end getCidrIpFromUtf8Ip()

// tool to get JSON encode IP from human readable IP. For IPv6 input, we don't
// accept abbreviated like ::12:3:4:5, and only accept a-f, not A-F
string getJsonIpFromCidrIp(const string &cidrIp) {
  unsigned char bytes[16] = {0};
  int i = 0, n = cidrIp.size(), index;
  if (cidrIp.find(':') == string::npos) { // IPv4
    index = 12;
    while (i < n) {
      while (i < n && cidrIp[i] != '.') {
        bytes[index] *= 10; // convert number string to int. Move onto next
                            // digit
        bytes[index] += cidrIp[i] - '0';
        ++i;
      }
      if (i < n) { // cidrIp[i] == '.'
        ++i;
        ++index;
      }
    } // end while (i < n)
    bytes[10] = bytes[11] = 0xFF;
  } else if (cidrIp.find(':') != string::npos) { // IPv6
    index = 0;
    while (i < n) {
      unsigned short val = 0;
      while (i < n && cidrIp[i] != ':') {
        val <<= 4;
        if ('0' <= cidrIp[i] && cidrIp[i] <= '9') {
          val += cidrIp[i] - '0';
        } else if ('a' <= cidrIp[i] && cidrIp[i] <= 'f') {
          val += cidrIp[i] - 'a' + 10;
        } else {
          return string("");
        }
        ++i;
      }
      bytes[index] = val >> 8;
      bytes[index + 1] = val & 0xff;
      index += 2;
      if (i < n) { // cidrIp[i] == ':'
        ++i;
      }
    } // end while (i < n)
  }   // end else if  IPv6

  string data("");
  for (i = 0; i < 16; ++i) {
    if (bytes[i] == 8) {
      data.append("\\b");
    } else if (bytes[i] == 9) {
      data.append("\\t");
    } else if (bytes[i] == 10) {
      data.append("\\n");
    } else if (bytes[i] == 12) {
      data.append("\\f");
    } else if (bytes[i] == 13) {
      data.append("\\r");
    } else if (bytes[i] == 34) {
      data.append("\\\"");
    } else if (bytes[i] == 92) {
      data.append("\\\\");
    } else if (0x20 <= bytes[i] && bytes[i] <= 0x7e) {
      data += bytes[i];
    } else {
      char buf[8];
      snprintf(buf, 8, "\\u%04x", bytes[i]);
      data.append(buf);
    }
  } // end for
  return data;
} // end getJsonIpFromCidrIp

// Function and sanitizeNonAsciiInput()
// this function only works on a string that has been processed by RapidJson.
// Don't use it on strings that still contains need-to-escape characters sucha
// as '\b', '\n', '\\'
void sanitizeNonAsciiInput(const string &input, string &output) {
  pthread_once(&sanitizedInitControl, initializeSanitizedCharacters);
  size_t inputSize = input.size();

  bool needSanitize = false;
  size_t i = 0;
  for (i = 0; i < inputSize; ++i) {
    if (input[i] < 0) { // unsigned char 0x80 is -128 and 0xFF is -1 in char
      needSanitize = true;
      break;
    }
  }

  if (!needSanitize) {
    output = input;
  } else {
    output.clear();
    output.reserve(inputSize + 300); // according to our experience, there won't
                                     // be more than 60 non-ascii code
    // usually much less, e.g.,  8 . Each one will need 5 more bytes
    output = input.substr(0, i);
    for (i; i < inputSize; ++i) {
      output += sanitizedCharacters[static_cast<unsigned char>(input[i])];
    }
  }
}
