#include <gtest/gtest.h>

#include <babylon/utils/base64.h>
#include <iostream>

TEST(TestBase64, Encode)
{
  const std::string testString
    = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit "
      "beschränkter Haftung";
  const std::string expectedBase64Encoded
    = "QURQIEdtYkgKQW5hbHlzZSBEZXNpZ24gJiBQcm9ncmFtbWllcnVuZwpHZXNlbGxzY2hhZnQg"
      "bWl0IGJlc2NocsOkbmt0ZXIgSGFmdHVuZw==";
  const std::string base64Encoded
    = base64_encode(reinterpret_cast<const unsigned char*>(testString.c_str()),
                    static_cast<unsigned int>(testString.length()));
  EXPECT_EQ(base64Encoded, expectedBase64Encoded);
}

TEST(TestBase64, Decode)
{
  const std::string encodedBase64String
    = "QURQIEdtYkgKQW5hbHlzZSBEZXNpZ24gJiBQcm9ncmFtbWllcnVuZwpHZXNlbGxzY2hhZnQg"
      "bWl0IGJlc2NocsOkbmt0ZXIgSGFmdHVuZw==";
  const std::string expectedBase64Decoded
    = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit "
      "beschränkter Haftung";
  const std::string decoded = base64_decode(encodedBase64String);
  EXPECT_EQ(decoded, expectedBase64Decoded);
}
