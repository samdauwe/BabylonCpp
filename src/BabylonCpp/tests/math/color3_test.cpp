#include <gtest/gtest.h>

#include <babylon/math/color3.h>

TEST(TestColor3, Constructor)
{
  // Constructor
  BABYLON::Color3 color3;
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  color3 = BABYLON::Color3{1.f, 1.f, 1.f};
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);

  // Copy constructor
  color3 = BABYLON::Color3{color3};
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, FromHexString)
{
  // Red
  std::string hex{"#FF0000"};
  BABYLON::Color3 color3 = BABYLON::Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Green
  hex    = std::string{"#00FF00"};
  color3 = BABYLON::Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Blue
  hex    = std::string{"#0000FF"};
  color3 = BABYLON::Color3::FromHexString(hex);
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, Lerp)
{
  BABYLON::Color3 c1;
  BABYLON::Color3 c2{1.f, 1.f, 1.f};
  BABYLON::Color3 color3 = BABYLON::Color3::Lerp(c1, c2, 0.3f);
  EXPECT_FLOAT_EQ(0.3f, color3.r);
  EXPECT_FLOAT_EQ(0.3f, color3.g);
  EXPECT_FLOAT_EQ(0.3f, color3.b);
}

TEST(TestColor3, Operators)
{
  // To array
  BABYLON::Color3 color3{0.1f, 0.2f, 0.3f};
  BABYLON::Float32Array result;
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
  BABYLON::Color3 otherColor{0.3f, 0.4f, 0.5f};
  BABYLON::Color3 c = color3.add(otherColor);
  EXPECT_FLOAT_EQ(0.4f, c.r);
  EXPECT_FLOAT_EQ(0.6f, c.g);
  EXPECT_FLOAT_EQ(0.8f, c.b);

  // Subtract
  otherColor = BABYLON::Color3{0.1f, 0.2f, 0.3f};
  c          = color3.subtract(otherColor);
  EXPECT_FLOAT_EQ(0.f, c.r);
  EXPECT_FLOAT_EQ(0.f, c.g);
  EXPECT_FLOAT_EQ(0.f, c.b);

  // Multiply
  otherColor = BABYLON::Color3{4.f, 3.f, 2.f};
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
  // Red
  BABYLON::Color3 color3 = BABYLON::Color3::Red();
  EXPECT_FLOAT_EQ(1.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Green
  color3 = BABYLON::Color3::Green();
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(1.f, color3.g);
  EXPECT_FLOAT_EQ(0.f, color3.b);

  // Blue
  color3 = BABYLON::Color3::Blue();
  EXPECT_FLOAT_EQ(0.f, color3.r);
  EXPECT_FLOAT_EQ(0.f, color3.g);
  EXPECT_FLOAT_EQ(1.f, color3.b);
}

TEST(TestColor3, ToHexString)
{
  // Red
  BABYLON::Color3 color3 = BABYLON::Color3::Red();
  EXPECT_EQ(color3.toHexString(), "#FF0000");

  // Green
  color3 = BABYLON::Color3::Green();
  EXPECT_EQ(color3.toHexString(), "#00FF00");

  // Blue
  color3 = BABYLON::Color3::Blue();
  EXPECT_EQ(color3.toHexString(), "#0000FF");
}
