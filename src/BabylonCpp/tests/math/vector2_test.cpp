#include <gtest/gtest.h>

#include <array>
#include <cmath>

#include <babylon/math/vector2.h>

namespace TestVector2 {
float x = 1.2f;
float y = 3.4f;
} // end of namespace TestVector2

TEST(TestVector2, Constructor)
{
  using namespace BABYLON;
  using namespace TestVector2;

  // Constructor
  Vector2 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);

  v = Vector2{x, y};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);

  // Copy constructor
  v = Vector2{v};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);
}

TEST(TestVector2, CopyFrom)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b = Vector2().copyFrom(a);
  EXPECT_FLOAT_EQ(b.x, a.x);
  EXPECT_FLOAT_EQ(b.y, a.y);

  // Ensure that it is a true copy
  a.x = 0.f;
  a.y = -1.f;
  EXPECT_FLOAT_EQ(b.x, x);
  EXPECT_FLOAT_EQ(b.y, y);
}

TEST(TestVector2, CopyFromFloats)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);

  v.copyFromFloats(x, y);
  EXPECT_FLOAT_EQ(v.x, x);
  EXPECT_FLOAT_EQ(v.y, y);
}

TEST(TestVector2, GetHashCode)
{
  using namespace BABYLON;
  using namespace TestVector2;

  std::array<Vector2, 4> vects{{
    Vector2{x, y},  // 00
    Vector2{x, -y}, // 01
    Vector2{-x, y}, // 10
    Vector2{-x, -y} // 11
  }};

  for (size_t i = 0; i < vects.size(); ++i) {
    for (size_t j = 0; j < vects.size(); ++j) {
      if (i == j) {
        EXPECT_EQ(vects[i].getHashCode(), vects[j].getHashCode());
      }
      else {
        EXPECT_NE(vects[i].getHashCode(), vects[j].getHashCode());
      }
    }
  }
}

TEST(TestVector2, Add)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b{-x, -y};
  Vector2 c;

  // c = a + b
  a.addToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);

  // c = b + b
  c = b.add(b);
  EXPECT_FLOAT_EQ(c.x, 2.f * b.x);
  EXPECT_FLOAT_EQ(c.y, 2.f * b.y);
}

TEST(TestVector2, Subtract)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b{-x, -y};
  Vector2 c;

  // c = a - b
  c = a.subtract(b);
  EXPECT_FLOAT_EQ(c.x, 2.f * x);
  EXPECT_FLOAT_EQ(c.y, 2.f * y);

  // c = a - a
  a.subtractToRef(a, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);

  // a = a - a
  a.subtractInPlace(a);
  EXPECT_FLOAT_EQ(a.x, 0.f);
  EXPECT_FLOAT_EQ(a.y, 0.f);
}

TEST(TestVector2, Multiply)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b{-x, -y};
  Vector2 c;

  // c = a * b
  c = a.multiply(b);
  EXPECT_FLOAT_EQ(c.x, -x * x);
  EXPECT_FLOAT_EQ(c.y, -y * y);

  // c = b * b
  b.multiplyToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, x * x);
  EXPECT_FLOAT_EQ(c.y, y * y);

  // a = a * a
  a.multiplyInPlace(a);
  EXPECT_FLOAT_EQ(a.x, x * x);
  EXPECT_FLOAT_EQ(a.y, y * y);

  // c = Vector2(a.x * 1, a.y * 2)
  c = a.multiplyByFloats(1.f, 2.f);
  EXPECT_FLOAT_EQ(c.x, x * x * 1.f);
  EXPECT_FLOAT_EQ(c.y, y * y * 2.f);
}

TEST(TestVector2, Divide)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b{-x, -y};
  Vector2 c;

  // c = a * b
  c = a.divide(b);
  EXPECT_FLOAT_EQ(c.x, -1.f);
  EXPECT_FLOAT_EQ(c.y, -1.f);

  // c = b * b
  b.divideToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 1.f);
  EXPECT_FLOAT_EQ(c.y, 1.f);
}

TEST(TestVector2, Negate)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 v{x, y};

  // v = -v
  v = v.negate();
  EXPECT_FLOAT_EQ(v.x, -x);
  EXPECT_FLOAT_EQ(v.y, -y);
}

TEST(TestVector2, Dot)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, y};
  Vector2 b{-x, -y};
  Vector2 c;

  float result = Vector2::Dot(a, b);
  EXPECT_FLOAT_EQ(result, -x * x - y * y);

  result = Vector2::Dot(a, c);
  EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(TestVector2, Length)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};
  Vector2 c;

  EXPECT_FLOAT_EQ(a.length(), x);
  EXPECT_FLOAT_EQ(b.length(), y);
  EXPECT_FLOAT_EQ(c.length(), 0.f);

  a.copyFromFloats(x, y);
  EXPECT_FLOAT_EQ(a.length(), std::sqrt(x * x + y * y));
}

TEST(TestVector2, LengthSquared)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};
  Vector2 c;

  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x);
  EXPECT_FLOAT_EQ(b.lengthSquared(), y * y);
  EXPECT_FLOAT_EQ(c.lengthSquared(), 0.f);

  a.copyFromFloats(x, y);
  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x + y * y);
}

TEST(TestVector2, Normalize)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};
  Vector2 c;

  a.normalize();
  EXPECT_FLOAT_EQ(a.length(), 1.f);
  EXPECT_FLOAT_EQ(a.x, 1.f);

  b.normalize();
  EXPECT_FLOAT_EQ(b.length(), 1.f);
  EXPECT_FLOAT_EQ(b.y, -1.f);
}

TEST(TestVector2, Distance)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};
  Vector2 c;

  EXPECT_FLOAT_EQ(Vector2::Distance(a, c), x);
  EXPECT_FLOAT_EQ(Vector2::Distance(b, c), y);
}

TEST(TestVector2, DistanceSquared)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};
  Vector2 c;

  EXPECT_FLOAT_EQ(Vector2::DistanceSquared(a, c), x * x);
  EXPECT_FLOAT_EQ(Vector2::DistanceSquared(b, c), y * y);
}

TEST(TestVector2, Lerp)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};

  EXPECT_TRUE(Vector2::Lerp(a, a, 0.f).equals(Vector2::Lerp(a, a, 0.5f)));
  EXPECT_TRUE(Vector2::Lerp(a, a, 0.f).equals(Vector2::Lerp(a, a, 1.f)));

  EXPECT_TRUE(Vector2::Lerp(a, b, 0.f).equals(a));

  EXPECT_FLOAT_EQ(Vector2::Lerp(a, b, 0.5f).x, x * 0.5f);
  EXPECT_FLOAT_EQ(Vector2::Lerp(a, b, 0.5f).y, -y * 0.5f);

  EXPECT_TRUE(Vector2::Lerp(a, b, 1).equals(b));
}

TEST(TestVector2, Equals)
{
  using namespace BABYLON;
  using namespace TestVector2;

  Vector2 a{x, 0.f};
  Vector2 b{0.f, -y};

  EXPECT_NE(a.x, b.x);
  EXPECT_NE(a.y, b.y);

  EXPECT_TRUE(!a.equals(b));
  EXPECT_TRUE(!b.equals(a));

  a = a.copyFrom(b);
  EXPECT_FLOAT_EQ(a.x, b.x);
  EXPECT_FLOAT_EQ(a.y, b.y);

  EXPECT_TRUE(a.equals(b));
  EXPECT_TRUE(b.equals(a));
}
