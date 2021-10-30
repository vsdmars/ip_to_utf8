#include <header.h>

// http://www.utf8-chartable.de/   UTF8  0~255 table
optional<string> getCidrIpFromUtf8Ip(const std::string &utf8Ip) {

  unsigned char ipV6[16] = {0};

  if (utf8Ip.size() < 16) { // 192.168.232.121     only 15 chars
    return {};
  }

  auto ipIdx = 0;
  auto idx = 0;
  while (ipIdx < utf8Ip.size()) {
    unsigned char val = (unsigned char)utf8Ip[ipIdx];

    if (val == 0xC3) { // e.g., rapidJson will read \u00FF  as 0xC3 0xBF
      ipV6[idx] = (unsigned char)utf8Ip[ipIdx + 1];
      ipV6[idx] += 0x40; // the diff between UTF8 code and the original
                         // code, e.g., 0xBF->0xFF, 0xBE->0xFE
      ipIdx += 2;
    } else if (val == 0xC2) {
      ipV6[idx] = (unsigned char)utf8Ip[ipIdx + 1];
      ipIdx += 2;
    } else if (val <= 0x7F) {
      ipV6[idx] = utf8Ip[ipIdx];
      ++ipIdx;
    } else {
      return {};
    }

    ++idx;
  } // end while

  bool areFirstTenBytesZero = true;
  for (auto j = 0; j < 10; ++j) {
    if (ipV6[j] != 0) {
      areFirstTenBytesZero = false;
      break;
    }
  }

  string res = "";
  if (areFirstTenBytesZero && ipV6[10] == 0xFF && ipV6[11] == 0xFF) { // ipV4
    res = to_string((long long unsigned int)ipV6[12]) + '.' +
          to_string((long long unsigned int)ipV6[13]) + '.' +
          to_string((long long unsigned int)ipV6[14]) + '.' +
          to_string((long long unsigned int)ipV6[15]);
  } else {
    char hexRep[8];

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

  return res;
} // end getCidrIpFromUtf8Ip()

// tool to get JSON encode IP from human readable IP. For IPv6 input, we don't
// accept abbreviated like ::12:3:4:5, and only accept a-f, not A-F
optional<string> getJsonIpFromCidrIp(const string &cidrIp) {
  unsigned char bytes[16] = {0};

  int i = 0;
  int n = cidrIp.size();
  int index = 0;

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

  string data{""};
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
