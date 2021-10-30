#ifndef RapidjsonUtil_H
#define RapidjsonUtil_H
#include <string>
#include <rapidjson/document.h>

namespace SentinelRapidjson {
enum IpVer { IPVER_UNKNOWN, IPVER_V4, IPVER_V6};
enum ValueType { VT_NULL, VT_STRING, VT_BOOL, VT_INT, VT_UINT, VT_INT64, VT_UINT64, VT_DOUBLE};
extern const char* ValueTypeString[];
std::string getPrintableValue(const rapidjson::Value& val);
IpVer getCidrIpFromUtf8Ip(const std::string& utf8Ip, std::string& res);
std::string getJsonIpFromCidrIp(const std::string& cidrIp);

// Function and sanitizeNonAsciiInput()
// this function only works on a string that has been processed by RapidJson. Don't use it on strings that still
// contains need-to-escape characters sucha as '\b', '\n', '\\'
void sanitizeNonAsciiInput(const std::string& input, std::string& output);
void sanitizeNonAsciiInputOpt(const std::string& input, std::string& output);
}

#endif
