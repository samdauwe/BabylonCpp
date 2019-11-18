#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/babylon_constants.h>
#include <babylon/core/string.h>

TEST(TestString, toCharCodes)
{
  using namespace BABYLON;

  const std::string s("Babylon.js");
  const std::vector<uint8_t> expected{66,  97,  98, 121, 108,
                                      111, 110, 46, 106, 115};
  EXPECT_THAT(String::toCharCodes(s), ::testing::ContainerEq(expected));
}

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

TEST(TestString, escape)
{
  using namespace BABYLON;

  EXPECT_THAT(String::escape("*"), "\\*");
  EXPECT_THAT(String::escape("\\"), "\\\\");
  EXPECT_THAT(String::escape("||"), "\\|\\|");
  EXPECT_THAT(String::escape(","), ",");
}

TEST(TestString, escapeStrings)
{
  using namespace BABYLON;

  const std::vector<std::string> input{",", "*", "||", ";", "\\"};
  const std::vector<std::string> expected{",", "\\*", "\\|\\|", ";",
                                              "\\\\"};

  EXPECT_THAT(String::escape(input), ::testing::ContainerEq(expected));
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

  std::vector<std::string> v{"A", "B", "C", "D", "E",
                                 "F", "G", "H", "I", "J"};
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

TEST(TestString, randomString)
{
  using namespace BABYLON;

  const std::string r1 = String::randomString(10);
  EXPECT_EQ(r1.size(), 10);

  const std::string r2 = String::randomString(5, "a");
  EXPECT_EQ(r2, "aaaaa");
}

std::string my_callback(const std::smatch& m)
{
  int int_m = atoi(m.str(0).c_str());
  return std::to_string(int_m + 1);
}

TEST(TestString, regexMatch)
{
  using namespace BABYLON;

  const std::regex re(R"(#include<(.+)>(\((.*)\))*(\[(.*)\])*)", std::regex::optimize);

  {
    const std::string s{"#include<helperFunctions>"};
    const auto r = String::regexMatch(s, re);
    const std::vector<std::string> e{s, "helperFunctions", "", "", "", ""};
    EXPECT_THAT(r, ::testing::ContainerEq(e));
  }

  {
    const std::string s{"#include<__decl__defaultFragment>"};
    const auto r = String::regexMatch(s, re);
    const std::vector<std::string> e{
      s, "__decl__defaultFragment", "", "", "", ""};
    EXPECT_THAT(r, ::testing::ContainerEq(e));
  }

  {
    const std::string s{
      "#include<__decl__lightFragment>[0..maxSimultaneousLights]"};
    const auto r = String::regexMatch(s, re);
    const std::vector<std::string> e{
      s,  "__decl__lightFragment",      "",
      "", "[0..maxSimultaneousLights]", "0..maxSimultaneousLights"};
    EXPECT_THAT(r, ::testing::ContainerEq(e));
  }
}

TEST(TestString, regexReplace)
{
  using namespace BABYLON;

  {
    const std::string s{
      "#define CUSTOM_VERTEX_BEGIN\n\nattribute vec3 position;"};
    const std::string regex{"attribute[ \t]"};
    const std::string r{String::regexReplace(s, regex, "in ")};
    const std::string e{"#define CUSTOM_VERTEX_BEGIN\n\nin vec3 position;"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"#extension GL_OES_standard_derivatives : enable"};
    const std::string regex{
      "#extension.+(GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_"
      "EXT_frag_depth).+enable"};
    const std::string r{String::regexReplace(s, regex, "")};
    const std::string e;
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{";#extension GL_EXT_frag_depth : enable;"};
    const std::string regex{
      "#extension.+(GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_"
      "EXT_frag_depth).+enable"};
    const std::string r{String::regexReplace(s, regex, "")};
    const std::string e{";;"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"varying vec2 vUV;varying vec4 vColor;"};
    const std::string r{String::regexReplace(s, "varying\\s", "in ")};
    const std::string e{"in vec2 vUV;in vec4 vColor;"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"attribute\tvec4 matricesIndices;"};
    const std::string r{String::regexReplace(s, "attribute[ \\t]", "in ")};
    const std::string e{"in vec4 matricesIndices;"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"\tattribute vec4 matricesIndicesExtra;"};
    const std::string r{String::regexReplace(s, "[ \\t]attribute", " in")};
    const std::string e{" in vec4 matricesIndicesExtra;"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"vec4 color = texture2D(textureSampler, vUV);"};
    const std::string r{String::regexReplace(s, "texture2D\\(", "texture(")};
    const std::string e{"vec4 color = texture(textureSampler, vUV);"};
    EXPECT_EQ(r, e);
  }

  {
    const std::string s{"void main(void)"};
    const std::string r{String::regexReplace(
      s, "void\\s+?main\\(", "out vec4 glFragColor;\nvoid main(")};
    const std::string e{"out vec4 glFragColor;\nvoid main(void)"};
    EXPECT_EQ(r, e);
  }

  {
    const auto _glslFloat = [](float x, unsigned int decimalFigures) {
      std::ostringstream oss;
      oss.precision(decimalFigures);
      oss << std::fixed << x;
      return String::regexReplace(oss.str(), "0+", "");
    };
    const std::string r1{_glslFloat(Math::PI, 6)};
    const std::string e1{"3.141593"};
    EXPECT_EQ(r1, e1);
    const std::string r2{_glslFloat(3.1400000f, 6)};
    const std::string e2{"3.14"};
    EXPECT_EQ(r2, e2);
  }
}

TEST(TestString, regexReplaceWithCallback)
{
  using namespace BABYLON;

  // Using free function
  {
    const std::string s{"1, 9, 19"};
    const std::string r{String::regexReplace(
      s, std::regex("\\d+", std::regex::optimize), my_callback)};
    const std::string e{"2, 10, 20"};
    EXPECT_EQ(r, e);
  }

  // Using lambda function
  {
    const auto callback = [](const std::smatch& m) {
      int int_m = std::atoi(m.str(0).c_str());
      return std::to_string(int_m + 1);
    };
    const std::string s{"1, 9, 19"};
    const std::string r{String::regexReplace(
      s, std::regex(R"(\d+)", std::regex::optimize), callback)};
    const std::string e{"2, 10, 20"};
    EXPECT_EQ(r, e);
  }

  {
    const auto callback = [](const std::smatch& m) {
      if (m.size() == 2) {
        return m.str(1) + "{X}";
      }
      return m.str(0);
    };
    const std::string s{
      "notShadowLevel = computeShadowWithESMCube(light{X}.vLightData.xyz, "
      "shadowSampler{X}, light{X}.shadowsInfo.x, light{X}.shadowsInfo.z);"};
    const std::regex regex{R"(light\{X\}.(\w*))", std::regex::optimize};
    const std::string r{String::regexReplace(s, regex, callback)};
    const std::string e{
      "notShadowLevel = computeShadowWithESMCube(vLightData{X}.xyz, "
      "shadowSampler{X}, shadowsInfo{X}.x, shadowsInfo{X}.z);"};
    EXPECT_EQ(r, e);
  }

  // Example: Replacing a Fahrenheit degree with its Celsius equivalent
  // Note: only integer based
  {
    const auto f2c = [](const std::string& x) {
      const auto convert = [](const std::smatch& m) {
        const int fval = std::atoi(m.str(0).c_str());
        const int cval = static_cast<int>((fval - 32) * 5.f / 9.f);
        return std::to_string(cval) + 'C';
      };
      return String::regexReplace(
        x, std::regex("(\\d+)F", std::regex::optimize), convert);
    };
    EXPECT_EQ(f2c("212F"), "100C");
  }

  // Example: number lines
  {
    const std::string s{"line1\nline2\nline3"};
    int i               = 2;
    const auto callback = [&i](const std::smatch& /*m*/) {
      return "\n" + std::to_string(i++) + "\t";
    };
    const std::string r{
      "1\t"
      + String::regexReplace(s, std::regex("\n", std::regex::optimize),
                             callback)};
    const std::string e{"1\tline1\n2\tline2\n3\tline3"};
    EXPECT_EQ(r, e);
  }
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
  std::string expected{
    "gl_FragColor=vec4(rightFrag.rgb*leftFrag.rgb,1.0);\n"};
  String::replaceInPlace(source, " ", "");
  EXPECT_EQ(source, expected);
}

TEST(TestString, split)
{
  using namespace BABYLON;

  const std::string str{"A,B*CxD,E;F,G,H,I,J"};
  std::vector<std::string> expected{"A", "B*CxD", "E;F", "G",
                                        "H", "I",     "J"};
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
