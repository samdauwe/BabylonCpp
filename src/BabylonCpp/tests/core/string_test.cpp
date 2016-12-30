#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/string.h>

TEST(TestString, concat)
{
  using namespace BABYLON;

  EXPECT_EQ(String::concat(1, "2", "A"), "12A");
}

TEST(TestString, contains)
{
  using namespace BABYLON;

  const std::string s("Babylon.js");
  EXPECT_TRUE(String::contains(s, "Babylon"));
  EXPECT_FALSE(String::contains(s, "babylon"));
}

TEST(TestString, startsWith)
{
  using namespace BABYLON;

  const std::string s("Babylon.js");
  EXPECT_TRUE(String::startsWith(s, "Babylon"));
  EXPECT_FALSE(String::startsWith(s, "babylon"));
  EXPECT_FALSE(String::startsWith(s, "Babylon.js_"));
}

TEST(TestString, endsWith)
{
  using namespace BABYLON;

  const std::string s("Babylon.js");
  EXPECT_TRUE(String::endsWith(s, ".js"));
  EXPECT_FALSE(String::endsWith(s, ".JS"));
  EXPECT_FALSE(String::endsWith(s, "Babylon.js_"));
}

TEST(TestString, fromCharCode)
{
  using namespace BABYLON;

  EXPECT_EQ(String::fromCharCode(65), "A");
  EXPECT_EQ(String::fromCharCode(72, 69, 76, 76, 79), "HELLO");
}

TEST(TestString, isDigit)
{
  using namespace BABYLON;

  EXPECT_TRUE(String::isDigit(5));
  EXPECT_TRUE(String::isDigit("5"));
  EXPECT_FALSE(String::isDigit("A"));
  EXPECT_FALSE(String::isDigit("*"));
}

TEST(TestString, join)
{
  using namespace BABYLON;

  std::vector<std::string> v{"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
  const std::string expected{"A,B,C,D,E,F,G,H,I,J"};
  EXPECT_EQ(String::join(v, ','), expected);
}

TEST(TestString, replace)
{
  using namespace BABYLON;

  // remove whitespace characters
  std::string source{
    "  gl_FragColor = vec4(rightFrag.rgb * leftFrag.rgb, 1.0);\n"};
  std::string expected{"gl_FragColor=vec4(rightFrag.rgb*leftFrag.rgb,1.0);\n"};
  String::replaceInPlace(source, " ", "");
  EXPECT_EQ(source, expected);
}

TEST(TestString, split)
{
  using namespace BABYLON;

  const std::string str{"A,B*CxD,E;F,G,H,I,J"};
  std::vector<std::string> expected{"A", "B*CxD", "E;F", "G", "H", "I", "J"};
  EXPECT_THAT(String::split(str, ','), ::testing::ContainerEq(expected));
}

TEST(TestString, toLowerCase)
{
  using namespace BABYLON;

  const std::string source{"**BABYLON.JS**"};
  const std::string expected{"**babylon.js**"};
  EXPECT_EQ(String::toLowerCase(source), expected);
}

TEST(TestString, toUpperCase)
{
  using namespace BABYLON;

  const std::string source{"**babyloncpp**"};
  const std::string expected{"**BABYLONCPP**"};
  EXPECT_EQ(String::toUpperCase(source), expected);
}
