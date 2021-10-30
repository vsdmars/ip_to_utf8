

#include "../include/RapidjsonUtil.h"
#include <string>
#include "gtest/gtest.h"

using std::string;
using SentinelRapidjson::getCidrIpFromUtf8Ip;
using SentinelRapidjson::getJsonIpFromCidrIp;

// use this command to get sample response  curli http://yahao-ld1.linkedin.biz:16826/fuse/ucsWhitelistedIps
#include <rapidjson/document.h>
using namespace rapidjson;

TEST(getCidrIpFromUtf8IpTest, UndecodableInput) {
  //use this service to create new conversion examples: curli http://bfeng-ld1.linkedin.biz:1338/simpleRestli/2620:9:c00e:213:2e27:d7ff:feef:6347
  string res;
  string ip("192.241.193.157");
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res.empty());
}

TEST(getCidrIpFromUtf8IpTest, InputQuoteTest) {
  // 192.168.39.2        '  Ascii code is 39
  string response =
    "{\"ip\":\"\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8'\\u0002\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "192.168.39.2");
}

TEST(getCidrIpFromUtf8IpTest, InputDoubleQuoteTest) {
  string response =
    "{\"ip\":\"\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8\\\"\\u0002\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "192.168.34.2");
}

TEST(getCidrIpFromUtf8IpTest, InputBackSlashTest) {
  string response =
    "{\"ip\":\"\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8\\\\\\u0002\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "192.168.92.2");
}

TEST(getCidrIpFromUtf8IpTest, InputDigitalOceanTest) {
  string response =
    "{\"ip\":\"\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00f1\\u00c1\\u009d\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "192.241.193.157");

}

TEST(getCidrIpFromUtf8IpTest, InputIPv61DigitTest) {
  string response =
    "{\"ip\":\"\\u0000\\u0002\\u0000\\u0001\\u0000\\f\\u0000\\u0002\\u0000\\u0002\\u0000\\r\\u0000\\u000f\\u0000\\u0007\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "2:1:c:2:2:d:f:7");

}

TEST(getCidrIpFromUtf8IpTest, InputIPv6MixDigitsTest) {
  string response =
    "{\"ip\":\"\\u00124\\u0005g\\u0000\\u0089\\u0000\\u0000\\u00ab\\u00cd\\u000e\\u00fe\\u0000\\u00dc\\u0000\\u000b\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "1234:567:89:0:abcd:efe:dc:b");

}

TEST(getCidrIpFromUtf8IpTest, InputIPv6Heading0Test) {
  string response =
    "{\"ip\":\"\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0001#\\u0000E\\u0000g\\u0000\\u0089\\u0000\\u0010\"}";
  Document parsedDocument;
  parsedDocument.Parse(response.c_str());
  ASSERT_FALSE(parsedDocument.HasParseError());
  string ip(parsedDocument["ip"].GetString(), parsedDocument["ip"].GetStringLength());
  string res;
  SentinelRapidjson::getCidrIpFromUtf8Ip(ip, res);
  ASSERT_TRUE(res == "0:0:0:123:45:67:89:10");

}

TEST(getCidrIpFromUtf8IpTest, GetJsonIpTest) {
  string cidrIp("192.168.1.2");
  string jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8\\u0001\\u0002");

  cidrIp = "54.176.122.246";  //used by orgBlock and IpBlock test blockedip2
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff6\\u00b0z\\u00f6");

  cidrIp = "0:0:0:123:45:67:89:10";
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp == "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0001#\\u0000E\\u0000g\\u0000\\u0089\\u0000\\u0010");

  cidrIp = "2620:109:c008:200:0:0:0:0";
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp == "& \\u0001\\t\\u00c0\\b\\u0002\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000");

  cidrIp = "192.168.39.2";
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8'\\u0002");

  cidrIp = "192.168.34.2";
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c0\\u00a8\\\"\\u0002");

  cidrIp = "89.145.12.189";  //used by IpBlock test blockedip2
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ffY\\u0091\\f\\u00bd");

  cidrIp = "2a01:4f8:130:7301:0:0:0:2";  //used by IpBlock test blockedipv6
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp == "*\\u0001\\u0004\\u00f8\\u00010s\\u0001\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0002");

  cidrIp = "54.176.122.236";  //used by orgBlock and IpBlock test softblockedip
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff6\\u00b0z\\u00ec");

  cidrIp = "192.55.79.0";  //used by orgBlock and IpBlock test softblockedip
  jsonIp = getJsonIpFromCidrIp(cidrIp);
  ASSERT_TRUE(jsonIp ==
              "\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u0000\\u00ff\\u00ff\\u00c07O\\u0000");
}

// This test deliberately tries to assign ints greater than 127 to char variables to test sanitization of input
// Skipping the -Wnarrowing flag for compilation.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
TEST(SanitizingUtilTest, SanitizeNonAsciiInput) {
  const char handMadeUserAgentArray[12] = { 7, 15, 18, 127, 0xf0, 0xfd, 0xfe, 0xff, '\\', '/', '=', 0};
  string handMadeUserAgent(handMadeUserAgentArray);

  string expectedResult("abc\\u0080\\u00f0\\u00fd\\u00fe\\u00ff\\/=");
  expectedResult[0] = 7;   //test that we don't touch anything that is less than 128
  expectedResult[1] = 15;
  expectedResult[2] = 18;

  string asciiJsonString;
  SentinelRapidjson::sanitizeNonAsciiInput(handMadeUserAgent, asciiJsonString);

  ASSERT_TRUE(asciiJsonString.substr(0, 3) == expectedResult.substr(0, 3));
}
#pragma GCC diagnostic pop

TEST(SanitizingUtilTest, SanitizeAsciiInput) {
  string  handMadeUserAgent = "This is a user agent with only ascii codes \\u0023 \\ \b";
  string asciiJsonString;
  SentinelRapidjson::sanitizeNonAsciiInput(handMadeUserAgent, asciiJsonString);
  ASSERT_TRUE(asciiJsonString == handMadeUserAgent);
}