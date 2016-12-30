#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/math/color3.h>
#include <babylon/math/color4.h>

TEST(TestColor4, CheckColors4)
{
  using namespace BABYLON;
  // Create test data
  Float32Array colors{
    0.1f, 0.2f, 0.3f, // First Color3
    0.4f, 0.5f, 0.6f, // Second Color3
  };
  Float32Array result = Color4::CheckColors4(colors, colors.size() / 3);
  // Set expected results
  Float32Array expectedResult{
    0.1f, 0.2f, 0.3f, 1.f, // First Color4
    0.4f, 0.5f, 0.6f, 1.f, // Second Color4
  };
  // Perform comparison
  EXPECT_THAT(result, ::testing::ContainerEq(expectedResult));
}

TEST(TestColor4, Constructor)
{
  // Constructor
  BABYLON::Color4 color4;
  EXPECT_FLOAT_EQ(0.f, color4.r);
  EXPECT_FLOAT_EQ(0.f, color4.g);
  EXPECT_FLOAT_EQ(0.f, color4.b);
  EXPECT_FLOAT_EQ(0.f, color4.a);

  color4 = BABYLON::Color4{1.f, 1.f, 1.f, 1.f};
  EXPECT_FLOAT_EQ(1.f, color4.r);
  EXPECT_FLOAT_EQ(1.f, color4.g);
  EXPECT_FLOAT_EQ(1.f, color4.b);
  EXPECT_FLOAT_EQ(1.f, color4.a);

  // Copy constructor
  color4 = BABYLON::Color4{color4};
  EXPECT_FLOAT_EQ(1.f, color4.r);
  EXPECT_FLOAT_EQ(1.f, color4.g);
  EXPECT_FLOAT_EQ(1.f, color4.b);
  EXPECT_FLOAT_EQ(1.f, color4.a);
}

TEST(TestColor4, FromHexString)
{
  // Red
  std::string hex{"#FF000000"};
  BABYLON::Color4 color4 = BABYLON::Color4::FromHexString(hex);
  EXPECT_FLOAT_EQ(1.f, color4.r);
  EXPECT_FLOAT_EQ(0.f, color4.g);
  EXPECT_FLOAT_EQ(0.f, color4.b);
  EXPECT_FLOAT_EQ(0.f, color4.a);

  // Green
  hex    = std::string{"#00FF0000"};
  color4 = BABYLON::Color4::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color4.r);
  EXPECT_FLOAT_EQ(1.f, color4.g);
  EXPECT_FLOAT_EQ(0.f, color4.b);
  EXPECT_FLOAT_EQ(0.f, color4.a);

  // Blue
  hex    = std::string{"#0000FF00"};
  color4 = BABYLON::Color4::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color4.r);
  EXPECT_FLOAT_EQ(0.f, color4.g);
  EXPECT_FLOAT_EQ(1.f, color4.b);
  EXPECT_FLOAT_EQ(0.f, color4.a);

  hex    = std::string{"#000000FF"};
  color4 = BABYLON::Color4::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color4.r);
  EXPECT_FLOAT_EQ(0.f, color4.g);
  EXPECT_FLOAT_EQ(0.f, color4.b);
  EXPECT_FLOAT_EQ(1.f, color4.a);
}

TEST(TestColor4, Lerp)
{
  BABYLON::Color4 c1;
  BABYLON::Color4 c2{1.f, 1.f, 1.f, 1.f};
  BABYLON::Color4 color4 = BABYLON::Color4::Lerp(c1, c2, 0.3f);
  EXPECT_FLOAT_EQ(0.3f, color4.r);
  EXPECT_FLOAT_EQ(0.3f, color4.g);
  EXPECT_FLOAT_EQ(0.3f, color4.b);
  EXPECT_FLOAT_EQ(0.3f, color4.a);
}

TEST(TestColor4, Operators)
{
  // To array
  BABYLON::Color4 color4{0.1f, 0.2f, 0.3f, 0.1f};
  BABYLON::Float32Array result;
  color4.toArray(result);
  EXPECT_FLOAT_EQ(0.1f, result[0]);
  EXPECT_FLOAT_EQ(0.2f, result[1]);
  EXPECT_FLOAT_EQ(0.3f, result[2]);
  EXPECT_FLOAT_EQ(0.1f, result[3]);

  // As array
  result = color4.asArray();
  EXPECT_FLOAT_EQ(0.1f, result[0]);
  EXPECT_FLOAT_EQ(0.2f, result[1]);
  EXPECT_FLOAT_EQ(0.3f, result[2]);
  EXPECT_FLOAT_EQ(0.1f, result[3]);

  // Add
  BABYLON::Color4 otherColor{0.3f, 0.4f, 0.5f, 0.6f};
  BABYLON::Color4 c = color4.add(otherColor);
  EXPECT_FLOAT_EQ(0.4f, c.r);
  EXPECT_FLOAT_EQ(0.6f, c.g);
  EXPECT_FLOAT_EQ(0.8f, c.b);
  EXPECT_FLOAT_EQ(0.7f, c.a);

  // Subtract
  otherColor = BABYLON::Color4{0.1f, 0.2f, 0.3f, 0.1f};
  c          = color4.subtract(otherColor);
  EXPECT_FLOAT_EQ(0.f, c.r);
  EXPECT_FLOAT_EQ(0.f, c.g);
  EXPECT_FLOAT_EQ(0.f, c.b);
  EXPECT_FLOAT_EQ(0.f, c.a);

  // Scale
  c = BABYLON::Color4{0.4f, 0.6f, 0.7f, 0.5f};
  c = c.scale(0.5f);
  EXPECT_FLOAT_EQ(0.20f, c.r);
  EXPECT_FLOAT_EQ(0.30f, c.g);
  EXPECT_FLOAT_EQ(0.35f, c.b);
  EXPECT_FLOAT_EQ(0.25f, c.a);
}

TEST(TestColor4, ToHexString)
{
  // Red
  BABYLON::Color3 color3 = BABYLON::Color3::Red();
  BABYLON::Color4 color4 = color3.toColor4();
  EXPECT_EQ(color4.toHexString(), "#FF0000FF");

  // Green
  color3 = BABYLON::Color3::Green();
  color4 = color3.toColor4();
  EXPECT_EQ(color4.toHexString(), "#00FF00FF");

  // Blue
  color3 = BABYLON::Color3::Blue();
  color4 = color3.toColor4();
  EXPECT_EQ(color4.toHexString(), "#0000FFFF");
}
