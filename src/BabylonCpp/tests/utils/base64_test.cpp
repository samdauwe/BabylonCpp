#include <gtest/gtest.h>

#include <babylon/utils/base64.h>
#include <iostream>

TEST(TestBase64, btoa)
{
  using namespace BABYLON;

  const std::string testString            = "Hello World!";
  const std::string expectedBase64Encoded = "SGVsbG8gV29ybGQh";
  const std::string base64Encoded         = Base64::btoa(testString);
  EXPECT_EQ(base64Encoded, expectedBase64Encoded);
}

TEST(TestBase64, atob)
{
  using namespace BABYLON;

  const std::string testString            = "SGVsbG8gV29ybGQh";
  const std::string expectedBase64Decoded = "Hello World!";
  const std::string base64Encoded         = Base64::atob(testString);
  EXPECT_EQ(base64Encoded, expectedBase64Decoded);
}

TEST(TestBase64, Encode)
{
  using namespace BABYLON;

  const std::string testString
    = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit "
      "beschränkter Haftung";
  const std::string expectedBase64Encoded
    = "QURQIEdtYkgKQW5hbHlzZSBEZXNpZ24gJiBQcm9ncmFtbWllcnVuZwpHZXNlbGxzY2hhZnQg"
      "bWl0IGJlc2NocsOkbmt0ZXIgSGFmdHVuZw==";
  const std::string base64Encoded
    = Base64::encode(reinterpret_cast<const unsigned char*>(testString.c_str()),
                     static_cast<unsigned int>(testString.length()));
  EXPECT_EQ(base64Encoded, expectedBase64Encoded);
}

TEST(TestBase64, Decode)
{
  using namespace BABYLON;

  const std::string encodedBase64String
    = "QURQIEdtYkgKQW5hbHlzZSBEZXNpZ24gJiBQcm9ncmFtbWllcnVuZwpHZXNlbGxzY2hhZnQg"
      "bWl0IGJlc2NocsOkbmt0ZXIgSGFmdHVuZw==";
  const std::string expectedBase64Decoded
    = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit "
      "beschränkter Haftung";
  const std::string decoded = Base64::decode(encodedBase64String);
  EXPECT_EQ(decoded, expectedBase64Decoded);
}
