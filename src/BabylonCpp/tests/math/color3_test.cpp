#include <gtest/gtest.h>

#include <babylon/math/color3.h>

TEST(TestColor3, Constructor)
{
  using namespace BABYLON;

  // Constructor
  Color3 color3;
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  color3 = Color3{1.f, 1.f, 1.f};
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);

  // Copy constructor
  color3 = Color3{color3};
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, FromHexString)
{
  using namespace BABYLON;

  // Red
  ::std::string hex{"#FF0000"};
  Color3 color3 = Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Green
  hex    = ::std::string{"#00FF00"};
  color3 = Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Blue
  hex    = ::std::string{"#0000FF"};
  color3 = Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, Lerp)
{
  using namespace BABYLON;

  Color3 c1;
  Color3 c2{1.f, 1.f, 1.f};
  Color3 color3 = Color3::Lerp(c1, c2, 0.3f);
  EXPECT_FLOAT_EQ(0.3f, color3.r);
  EXPECT_FLOAT_EQ(0.3f, color3.g);
  EXPECT_FLOAT_EQ(0.3f, color3.b);
}

TEST(TestColor3, Operators)
{
  using namespace BABYLON;

  // To array
  Color3 color3{0.1f, 0.2f, 0.3f};
  Float32Array result;
  color3.toArray(result);
  EXPECT_FLOAT_EQ(0.1f, result[0]);
  EXPECT_FLOAT_EQ(0.2f, result[1]);
  EXPECT_FLOAT_EQ(0.3f, result[2]);

  // As array
  result = color3.asArray();
  EXPECT_FLOAT_EQ(0.1f, result[0]);
  EXPECT_FLOAT_EQ(0.2f, result[1]);
  EXPECT_FLOAT_EQ(0.3f, result[2]);

  // Add
  Color3 otherColor{0.3f, 0.4f, 0.5f};
  Color3 c = color3.add(otherColor);
  EXPECT_FLOAT_EQ(0.4f, c.r);
  EXPECT_FLOAT_EQ(0.6f, c.g);
  EXPECT_FLOAT_EQ(0.8f, c.b);

  // Subtract
  otherColor = Color3{0.1f, 0.2f, 0.3f};
  c          = color3.subtract(otherColor);
  EXPECT_FLOAT_EQ(0.f, c.r);
  EXPECT_FLOAT_EQ(0.f, c.g);
  EXPECT_FLOAT_EQ(0.f, c.b);

  // Multiply
  otherColor = Color3{4.f, 3.f, 2.f};
  c          = color3.multiply(otherColor);
  EXPECT_FLOAT_EQ(0.4f, c.r);
  EXPECT_FLOAT_EQ(0.6f, c.g);
  EXPECT_FLOAT_EQ(0.6f, c.b);

  // Scale
  c = c.scale(0.5f);
  EXPECT_FLOAT_EQ(0.2f, c.r);
  EXPECT_FLOAT_EQ(0.3f, c.g);
  EXPECT_FLOAT_EQ(0.3f, c.b);
}

TEST(TestColor3, RGB)
{
  using namespace BABYLON;

  // Red
  Color3 color3 = Color3::Red();
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Green
  color3 = Color3::Green();
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Blue
  color3 = Color3::Blue();
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, ToHexString)
{
  using namespace BABYLON;

  // Red
  Color3 color3 = Color3::Red();
  EXPECT_EQ(color3.toHexString(), "#FF0000");

  // Green
  color3 = Color3::Green();
  EXPECT_EQ(color3.toHexString(), "#00FF00");

  // Blue
  color3 = Color3::Blue();
  EXPECT_EQ(color3.toHexString(), "#0000FF");
}
