#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/babylon_constants.h>
#include <babylon/babylon_stl.h>

TEST(TestStdUtil, to_bitset)
{
  using namespace BABYLON;

  // 0
  float number         = 0.f;
  std::string expected = "00000000000000000000000000000000";
  EXPECT_EQ(std_util::to_bitset(number).to_string(), expected);

  // PI
  number   = Math::PI;
  expected = "01000000010010010000111111011011";
  EXPECT_EQ(std_util::to_bitset(number).to_string(), expected);

  // -PI
  number   = -Math::PI;
  expected = "11000000010010010000111111011011";
  EXPECT_EQ(std_util::to_bitset(number).to_string(), expected);

  // sqrt(2)
  number   = std::sqrt(2.f);
  expected = "00111111101101010000010011110011";
  EXPECT_EQ(std_util::to_bitset(number).to_string(), expected);
}

TEST(TestStdUtil, to_hex_string)
{
  using namespace BABYLON;

  // 0
  std::string bitString = "00000000000000000000000000000000";
  std::string expected  = "0x00000000";
  EXPECT_EQ(std_util::to_hex_string(bitString), expected);

  // PI
  bitString = "01000000010010010000111111011011";
  expected  = "0x40490fdb";
  EXPECT_EQ(std_util::to_hex_string(bitString), expected);

  // -PI
  bitString = "11000000010010010000111111011011";
  expected  = "0xc0490fdb";
  EXPECT_EQ(std_util::to_hex_string(bitString), expected);

  // sqrt(2)
  bitString = "00111111101101010000010011110011";
  expected  = "0x3fb504f3";
  EXPECT_EQ(std_util::to_hex_string(bitString), expected);
}

TEST(TestStdUtil, hex_to_float)
{
  using namespace BABYLON;

  // 0
  std::string hexString = "0x00000000";
  float expected        = 0.f;
  EXPECT_EQ(std_util::hex_to_float(hexString), expected);

  // PI
  hexString = "0x40490fdb";
  expected  = Math::PI;
  EXPECT_EQ(std_util::hex_to_float(hexString), expected);

  // -PI
  hexString = "0xc0490fdb";
  expected  = -Math::PI;
  EXPECT_EQ(std_util::hex_to_float(hexString), expected);

  // sqrt(2)
  hexString = "0x3fb504f3";
  expected  = std::sqrt(2.f);
  EXPECT_EQ(std_util::hex_to_float(hexString), expected);
}

TEST(TestStdUtil, to_vector)
{
  using namespace BABYLON;

  const Int32Array test = std_util::to_vector(0, 1, 2, 3, 4, 5);
  const Int32Array expected{0, 1, 2, 3, 4, 5};
  EXPECT_THAT(test, ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, concat)
{
  using namespace BABYLON;

  Int32Array test{0, 1, 2, 3, 4, 5};
  std_util::concat(test, {6, 7, 8});
  std_util::concat(test, {9, 10});
  const Int32Array expected{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  EXPECT_THAT(test, ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, contains)
{
  using namespace BABYLON;

  const Int32Array v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  EXPECT_TRUE(std_util::contains(v, 2));
  EXPECT_TRUE(std_util::contains(v, 7));
  EXPECT_TRUE(std_util::contains(v, 10));
  EXPECT_FALSE(std_util::contains(v, -1));
  EXPECT_FALSE(std_util::contains(v, 11));
}

TEST(TestStdUtil, insert_at)
{
  using namespace BABYLON;

  std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
  const std::vector<std::string> expected{"DIFFUSE", "CLIPPLANE", "ALPHATEST",
                                          "POINTSIZE", "FOG"};
  EXPECT_THAT(std_util::insert_at(v, 2, "ALPHATEST"),
              ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, unordered_map_contains)
{
  using namespace BABYLON;

  const std::unordered_map<std::string, int> c{
    {"DIFFUSE", 0},   {"CLIPPLANE", 2}, {"VERTEXALPHA", 4},
    {"ALPHATEST", 6}, {"POINTSIZE", 8}, {"FOG", 10}};
  EXPECT_TRUE(std_util::contains(c, "DIFFUSE"));
  EXPECT_TRUE(std_util::contains(c, "CLIPPLANE"));
  EXPECT_FALSE(std_util::contains(c, "SHADOWS"));
}

TEST(TestStdUtil, extract_keys)
{
  using namespace BABYLON;

  const std::unordered_map<std::string, int> c{
    {"DIFFUSE", 0},   {"CLIPPLANE", 2}, {"VERTEXALPHA", 4},
    {"ALPHATEST", 6}, {"POINTSIZE", 8}, {"FOG", 10}};
  const std::vector<std::string> keys = std_util::extract_keys(c);
  EXPECT_TRUE(std_util::contains(keys, "DIFFUSE"));
  EXPECT_TRUE(std_util::contains(keys, "CLIPPLANE"));
  EXPECT_TRUE(std_util::contains(keys, "VERTEXALPHA"));
  EXPECT_TRUE(std_util::contains(keys, "ALPHATEST"));
  EXPECT_TRUE(std_util::contains(keys, "POINTSIZE"));
  EXPECT_TRUE(std_util::contains(keys, "FOG"));
  EXPECT_FALSE(std_util::contains(keys, "VERTEXCOLOR"));
}

TEST(TestStdUtil, extract_values)
{
  using namespace BABYLON;

  const std::unordered_map<std::string, int> c{
    {"DIFFUSE", 0},   {"CLIPPLANE", 2}, {"VERTEXALPHA", 4},
    {"ALPHATEST", 6}, {"POINTSIZE", 8}, {"FOG", 10}};
  const Int32Array values = std_util::extract_values(c);
  EXPECT_TRUE(std_util::contains(values, 0));
  EXPECT_FALSE(std_util::contains(values, 1));
  EXPECT_TRUE(std_util::contains(values, 2));
  EXPECT_TRUE(std_util::contains(values, 4));
  EXPECT_TRUE(std_util::contains(values, 6));
  EXPECT_TRUE(std_util::contains(values, 8));
  EXPECT_TRUE(std_util::contains(values, 10));
}

TEST(TestStdUtil, erase)
{
  using namespace BABYLON;

  std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
  const std::vector<std::string> expected{"DIFFUSE", "FOG"};
  EXPECT_TRUE(std_util::erase(v, "CLIPPLANE"));
  EXPECT_TRUE(std_util::erase(v, "POINTSIZE"));
  EXPECT_FALSE(std_util::erase(v, "POINTSIZE"));
  EXPECT_FALSE(std_util::erase(v, "VERTEXALPHA"));
  EXPECT_THAT(v, ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, index_of)
{
  using namespace BABYLON;

  std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
  EXPECT_EQ(std_util::index_of(v, "DIFFUSE"), 0);
  EXPECT_EQ(std_util::index_of(v, "FOG"), 3);
  EXPECT_EQ(std_util::index_of(v, "VERTEXALPHA"), -1);
}

TEST(TestStdUtil, slice)
{
  using namespace BABYLON;

  const Int32Array v{1, 2, 3, 4, 5};

  {
    const Int32Array result = std_util::slice(v, 1, 3);
    const Int32Array expected{2, 3};
    EXPECT_THAT(result, ::testing::ContainerEq(expected));
  }

  {
    const Int32Array result = std_util::slice(v, 1);
    const Int32Array expected{2, 3, 4, 5};
    EXPECT_THAT(result, ::testing::ContainerEq(expected));
  }

  {
    const Int32Array result = std_util::slice(v, 3, 1);
    const Int32Array expected{2, 3};
    EXPECT_THAT(result, ::testing::ContainerEq(expected));
  }

  {
    const Int32Array result = std_util::slice(v, -3, 2);
    const Int32Array expected{1, 2};
    EXPECT_THAT(result, ::testing::ContainerEq(expected));
  }
}

TEST(TestStdUtil, slice_in_place)
{
  using namespace BABYLON;

  {
    Int32Array v{1, 2, 3, 4, 5};
    std_util::slice_in_place(v, 1, 3);
    const Int32Array expected{2, 3};
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
  }

  {
    Int32Array v{1, 2, 3, 4, 5};
    std_util::slice_in_place(v, 1);
    const Int32Array expected{2, 3, 4, 5};
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
  }

  {
    Int32Array v{1, 2, 3, 4, 5};
    std_util::slice_in_place(v, 3, 1);
    const Int32Array expected{2, 3};
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
  }

  {
    Int32Array v{1, 2, 3, 4, 5};
    std_util::slice_in_place(v, -3, 2);
    const Int32Array expected{1, 2};
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
  }
}

TEST(TestStdUtil, splice)
{
  using namespace BABYLON;

  // At position 2, add the new items
  {
    std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
    std::vector<std::string> expected{"DIFFUSE",     "CLIPPLANE", "SHADOW",
                                      "VERTEXALPHA", "POINTSIZE", "FOG"};
    std::vector<std::string> r
      = std_util::splice(v, 2, 0, {"SHADOW", "VERTEXALPHA"});
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
    EXPECT_TRUE(r.empty());
  }

  // At position 2, add the new items, and remove 1 item
  {
    std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
    std::vector<std::string> expected{"DIFFUSE", "CLIPPLANE", "SHADOW",
                                      "VERTEXALPHA", "FOG"};
    std::vector<std::string> expectedRemovedItem{"POINTSIZE"};
    std::vector<std::string> r
      = std_util::splice(v, 2, 1, {"SHADOW", "VERTEXALPHA"});
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
    EXPECT_THAT(r, ::testing::ContainerEq(expectedRemovedItem));
  }

  // At position 2, remove 2 items
  {
    std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "SHADOW", "VERTEXALPHA",
                               "FOG"};
    std::vector<std::string> expected{"DIFFUSE", "CLIPPLANE", "FOG"};
    std::vector<std::string> expectedRemovedItems{"SHADOW", "VERTEXALPHA"};
    std::vector<std::string> r = std_util::splice(v, 2, 2);
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
    EXPECT_THAT(r, ::testing::ContainerEq(expectedRemovedItems));
  }

  // Remove last item
  {
    std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "SHADOW", "FOG"};
    std::vector<std::string> expected{"DIFFUSE", "CLIPPLANE", "SHADOW"};
    std::vector<std::string> expectedRemovedItem{"FOG"};
    std::vector<std::string> r = std_util::splice(v, -1, 1);
    EXPECT_THAT(v, ::testing::ContainerEq(expected));
    EXPECT_THAT(r, ::testing::ContainerEq(expectedRemovedItem));
  }
}

TEST(TestStdUtil, remove)
{
  using namespace BABYLON;

  std::vector<std::string> v{"DIFFUSE", "CLIPPLANE", "POINTSIZE", "FOG"};
  const std::vector<std::string> expected{"CLIPPLANE", "POINTSIZE"};
  std_util::remove(v, 0);
  std_util::remove(v, v.size() - 1);
  EXPECT_THAT(v, ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, push_unique)
{
  using namespace BABYLON;

  Int32Array result;
  const Int32Array v{1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5};
  const Int32Array expected{1, 2, 3, 4, 5};
  for (auto& iv : v) {
    std_util::push_unique(result, iv);
  }
  EXPECT_THAT(result, ::testing::ContainerEq(expected));
}

TEST(TestStdUtil, range)
{
  using namespace BABYLON;

  const size_t start = 10;
  const size_t end   = 20;
  std::vector<size_t> result;
  const std::vector<size_t> expected{10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  for (auto i : std_util::range(start, end)) {
    result.emplace_back(i);
  }
  EXPECT_THAT(result, ::testing::ContainerEq(expected));
}
