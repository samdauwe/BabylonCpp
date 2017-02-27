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

TEST(TestString, count)
{
  using namespace BABYLON;

  const std::string s("-- Babylon.js --");
  EXPECT_EQ(String::count(s, "B"), 1);
  EXPECT_EQ(String::count(s, "."), 1);
  EXPECT_EQ(String::count(s, "z"), 0);
  EXPECT_EQ(String::count(s, "-"), 4);
  EXPECT_EQ(String::count(s, "--"), 2);
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

TEST(TestString, indexOf)
{
  using namespace BABYLON;

  std::string str = "Hello world, welcome to the universe.";
  EXPECT_EQ(String::indexOf(str, "welcome"), 13);
  EXPECT_EQ(String::indexOf(str, "e"), 1);
  EXPECT_EQ(String::indexOf(str, "e", 5), 14);
  EXPECT_EQ(String::indexOf(str, "abc"), -1);
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

TEST(TestString, lastIndexOf)
{
  using namespace BABYLON;

  std::string str = "Hello planet earth, you are a great planet.";
  EXPECT_EQ(String::lastIndexOf(str, "planet"), 36);
  EXPECT_EQ(String::lastIndexOf(str, "planet", 20), 6);
  EXPECT_EQ(String::lastIndexOf(str, "abc"), -1);
}

TEST(TestString, nthChar)
{
  using namespace BABYLON;

  EXPECT_EQ(String::nthChar("abc", 0), 'a');
  EXPECT_EQ(String::nthChar("abc", 1), 'b');
  EXPECT_EQ(String::nthChar("abc", 2), 'c');
}

TEST(TestString, pad)
{
  using namespace BABYLON;

  std::string s{"Babylon.js"};
  String::pad(s, 15, '.');
  EXPECT_EQ(s, "Babylon.js.....");
  String::pad(s, 10, '.');
  EXPECT_EQ(s, "Babylon.js.....");
}

TEST(TestString, printf)
{
  using namespace BABYLON;

  EXPECT_EQ(String::printf("%s", "Hello"), "Hello");
  EXPECT_EQ(String::printf("Pi = %.5lf", 3.14159), "Pi = 3.14159");

  auto test = String::printf("Hello %s, did you know that pi equals %.2lf?\n",
                             "User", 3.14159);
  std::string expected{"Hello User, did you know that pi equals 3.14?\n"};
  EXPECT_EQ(test, expected);
}

TEST(TestString, pushFront)
{
  using namespace BABYLON;

  std::string s = "World";
  String::pushFront(s, "Hello ");
  EXPECT_EQ(s, "Hello World");
}

TEST(TestString, removeSubstring)
{
  using namespace BABYLON;

  std::string s{"Line 1, Line 2, Line 3, Line 4, Line 5"};
  const std::string subStr{"Line "};
  const std::string expected{"1, 2, 3, 4, 5"};
  EXPECT_EQ(String::removeSubstring(s, subStr), expected);
}

TEST(TestString, repeat)
{
  using namespace BABYLON;

  // remove whitespace characters
  EXPECT_EQ(String::repeat("Hello world!", 0), "");
  EXPECT_EQ(String::repeat("Hello world!", 2), "Hello world!Hello world!");
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

TEST(TestString, slice)
{
  using namespace BABYLON;

  const std::string str{"Hello world!"};
  EXPECT_EQ(String::slice(str), "Hello world!");
  EXPECT_EQ(String::slice(str, 0), "Hello world!");
  EXPECT_EQ(String::slice(str, 3), "lo world!");
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

TEST(TestString, toNumber)
{
  using namespace BABYLON;

  EXPECT_EQ(String::toNumber<int>("3"), 3);
  EXPECT_EQ(String::toNumber<size_t>("3"), 3);
  EXPECT_EQ(String::toNumber<float>("4.5"), 4.5f);
  EXPECT_EQ(String::toNumber<double>("4.5"), 4.5);
}

TEST(TestString, toString)
{
  using namespace BABYLON;

  EXPECT_EQ(String::toString<int>(3), "3");
  EXPECT_EQ(String::toString<size_t>(3), "3");
  EXPECT_EQ(String::toString<float>(4.5f), "4.5");
  EXPECT_EQ(String::toString<double>(4.5), "4.5");
}

TEST(TestString, toTitleCase)
{
  using namespace BABYLON;

  std::string str{"hello world"};
  EXPECT_EQ(String::toTitleCase(str), "Hello World");
}

TEST(TestString, trim)
{
  using namespace BABYLON;

  std::string str{"  abc  "};
  EXPECT_EQ(String::trim(str), "abc");
}

TEST(TestString, trimCopy)
{
  using namespace BABYLON;

  std::string str{"  abc "};
  std::string strCopy{String::trimCopy(str)};
  EXPECT_EQ(str, "  abc ");
  EXPECT_EQ(strCopy, "abc");
}

TEST(TestString, trimLeft)
{
  using namespace BABYLON;

  std::string str{"  abc "};
  EXPECT_EQ(String::trimLeft(str), "abc ");
}

TEST(TestString, trimRight)
{
  using namespace BABYLON;

  std::string str{" abc  "};
  EXPECT_EQ(String::trimRight(str), " abc");
}
