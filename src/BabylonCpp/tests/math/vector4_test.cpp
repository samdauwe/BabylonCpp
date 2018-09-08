#include <gtest/gtest.h>

#include <cmath>

#include <babylon/math/vector4.h>

namespace {
float x = 1.2f;
float y = 3.4f;
float z = 5.6f;
float w = 7.8f;
} // namespace

TEST(TestVector4, Constructor)
{
  using namespace BABYLON;

  // Constructor
  Vector4 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);
  EXPECT_FLOAT_EQ(0.f, v.z);
  EXPECT_FLOAT_EQ(0.f, v.w);

  v = Vector4{x, y, z, w};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);
  EXPECT_FLOAT_EQ(z, v.z);
  EXPECT_FLOAT_EQ(w, v.w);

  // Copy constructor
  v = Vector4{v};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);
  EXPECT_FLOAT_EQ(z, v.z);
  EXPECT_FLOAT_EQ(w, v.w);
}

TEST(TestVector4, CopyFrom)
{
  using namespace BABYLON;

  Vector4 a{x, y, z, w};
  Vector4 b = Vector4().copyFrom(a);
  EXPECT_FLOAT_EQ(b.x, a.x);
  EXPECT_FLOAT_EQ(b.y, a.y);
  EXPECT_FLOAT_EQ(b.z, a.z);
  EXPECT_FLOAT_EQ(b.w, a.w);

  // Ensure that it is a true copy
  a.x = 0.f;
  a.y = -1.f;
  a.z = -2.f;
  a.w = -3.f;
  EXPECT_FLOAT_EQ(b.x, x);
  EXPECT_FLOAT_EQ(b.y, y);
  EXPECT_FLOAT_EQ(b.z, z);
  EXPECT_FLOAT_EQ(b.w, w);
}

TEST(TestVector4, CopyFromFloats)
{
  using namespace BABYLON;

  Vector4 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);
  EXPECT_FLOAT_EQ(0.f, v.z);
  EXPECT_FLOAT_EQ(0.f, v.w);

  v.copyFromFloats(x, y, z, w);
  EXPECT_FLOAT_EQ(v.x, x);
  EXPECT_FLOAT_EQ(v.y, y);
  EXPECT_FLOAT_EQ(v.z, z);
  EXPECT_FLOAT_EQ(v.w, w);
}

TEST(TestVector4, GetHashCode)
{
  using namespace BABYLON;

  std::array<Vector4, 16> vects{{
    Vector4{x, y, z, w},    // 0000
    Vector4{x, y, z, -w},   // 0001
    Vector4{x, y, -z, w},   // 0010
    Vector4{x, y, -z, -w},  // 0011
    Vector4{x, -y, z, w},   // 0100
    Vector4{x, -y, z, -w},  // 0101
    Vector4{x, -y, -z, w},  // 0110
    Vector4{x, -y, -z, -w}, // 0111
    Vector4{-x, y, z, w},   // 1000
    Vector4{-x, y, z, -w},  // 1001
    Vector4{-x, y, -z, w},  // 1010
    Vector4{-x, y, -z, -w}, // 1011
    Vector4{-x, -y, z, w},  // 1100
    Vector4{-x, -y, z, -w}, // 1101
    Vector4{-x, -y, -z, w}, // 1110
    Vector4{-x, -y, -z, -w} // 1111
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

TEST(TestVector4, Add)
{
  using namespace BABYLON;

  Vector4 a{x, y, z, w};
  Vector4 b{-x, -y, -z, -w};
  Vector4 c;

  // c = a + b
  a.addToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);
  EXPECT_FLOAT_EQ(c.z, 0.f);
  EXPECT_FLOAT_EQ(c.w, 0.f);

  // c = b + b
  c = b.add(b);
  EXPECT_FLOAT_EQ(c.x, 2 * b.x);
  EXPECT_FLOAT_EQ(c.y, 2 * b.y);
  EXPECT_FLOAT_EQ(c.z, 2 * b.z);
  EXPECT_FLOAT_EQ(c.w, 2 * b.w);
}

TEST(TestVector4, Subtract)
{
  using namespace BABYLON;

  Vector4 a{x, y, z, w};
  Vector4 b{-x, -y, -z, -w};
  Vector4 c;

  // c = a - b
  c = a.subtract(b);
  EXPECT_FLOAT_EQ(c.x, 2.f * x);
  EXPECT_FLOAT_EQ(c.y, 2.f * y);
  EXPECT_FLOAT_EQ(c.z, 2.f * z);
  EXPECT_FLOAT_EQ(c.w, 2.f * w);

  // c = a - a
  a.subtractToRef(a, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);
  EXPECT_FLOAT_EQ(c.z, 0.f);
  EXPECT_FLOAT_EQ(c.w, 0.f);

  // a = a - a
  a.subtractInPlace(a);
  EXPECT_FLOAT_EQ(a.x, 0.f);
  EXPECT_FLOAT_EQ(a.y, 0.f);
  EXPECT_FLOAT_EQ(a.z, 0.f);
  EXPECT_FLOAT_EQ(a.w, 0.f);
}

TEST(TestVector4, Multiply)
{
  using namespace BABYLON;

  Vector4 a{x, y, z, w};
  Vector4 b{-x, -y, -z, -w};
  Vector4 c;

  // c = a * b
  c = a.multiply(b);
  EXPECT_FLOAT_EQ(c.x, -x * x);
  EXPECT_FLOAT_EQ(c.y, -y * y);
  EXPECT_FLOAT_EQ(c.z, -z * z);
  EXPECT_FLOAT_EQ(c.w, -w * w);

  // c = b * b
  b.multiplyToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, x * x);
  EXPECT_FLOAT_EQ(c.y, y * y);
  EXPECT_FLOAT_EQ(c.z, z * z);
  EXPECT_FLOAT_EQ(c.w, w * w);

  // a = a * a
  a.multiplyInPlace(a);
  EXPECT_FLOAT_EQ(a.x, x * x);
  EXPECT_FLOAT_EQ(a.y, y * y);
  EXPECT_FLOAT_EQ(a.z, z * z);
  EXPECT_FLOAT_EQ(a.w, w * w);

  // c = Vector2(a.x * 1, a.y * 2, a.z * 3, a.w * 4)
  c = a.multiplyByFloats(1.f, 2.f, 3.f, 4.f);
  EXPECT_FLOAT_EQ(c.x, x * x * 1.f);
  EXPECT_FLOAT_EQ(c.y, y * y * 2.f);
  EXPECT_FLOAT_EQ(c.z, z * z * 3.f);
  EXPECT_FLOAT_EQ(c.w, w * w * 4.f);
}

TEST(TestVector4, Divide)
{
  using namespace BABYLON;

  Vector4 a{x, y, z, w};
  Vector4 b{-x, -y, -z, -w};
  Vector4 c;

  // c = a * b
  c = a.divide(b);
  EXPECT_FLOAT_EQ(c.x, -1.f);
  EXPECT_FLOAT_EQ(c.y, -1.f);
  EXPECT_FLOAT_EQ(c.z, -1.f);
  EXPECT_FLOAT_EQ(c.w, -1.f);

  // c = b * b
  b.divideToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 1.f);
  EXPECT_FLOAT_EQ(c.y, 1.f);
  EXPECT_FLOAT_EQ(c.z, 1.f);
  EXPECT_FLOAT_EQ(c.w, 1.f);
}

TEST(TestVector4, Negate)
{
  using namespace BABYLON;

  Vector4 v{x, y, z, w};

  // v = -v
  v = v.negate();
  EXPECT_FLOAT_EQ(v.x, -x);
  EXPECT_FLOAT_EQ(v.y, -y);
  EXPECT_FLOAT_EQ(v.z, -z);
  EXPECT_FLOAT_EQ(v.w, -w);
}

TEST(TestVector4, Length)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, 0.f, 0.f};
  Vector4 b{0.f, -y, 0.f, 0.f};
  Vector4 c{0.f, 0.f, z, 0.f};
  Vector4 d{0.f, 0.f, 0.f, w};
  Vector4 e;

  EXPECT_FLOAT_EQ(a.length(), x);
  EXPECT_FLOAT_EQ(b.length(), y);
  EXPECT_FLOAT_EQ(c.length(), z);
  EXPECT_FLOAT_EQ(d.length(), w);
  EXPECT_FLOAT_EQ(e.length(), 0.f);

  a.copyFromFloats(x, y, z, w);
  EXPECT_FLOAT_EQ(a.length(), std::sqrt(x * x + y * y + z * z + w * w));
}

TEST(TestVector4, LengthSquared)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, 0.f, 0.f};
  Vector4 b{0.f, -y, 0.f, 0.f};
  Vector4 c{0.f, 0.f, z, 0.f};
  Vector4 d{0.f, 0.f, 0.f, w};
  Vector4 e;

  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x);
  EXPECT_FLOAT_EQ(b.lengthSquared(), y * y);
  EXPECT_FLOAT_EQ(c.lengthSquared(), z * z);
  EXPECT_FLOAT_EQ(d.lengthSquared(), w * w);
  EXPECT_FLOAT_EQ(e.lengthSquared(), 0.f);

  a.copyFromFloats(x, y, z, w);
  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x + y * y + z * z + w * w);
}

TEST(TestVector4, Normalize)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, 0.f, 0.f};
  Vector4 b{0.f, -y, 0.f, 0.f};
  Vector4 c{0.f, 0.f, z, 0.f};
  Vector4 d{0.f, 0.f, 0.f, -w};

  a.normalize();
  EXPECT_FLOAT_EQ(a.length(), 1.f);
  EXPECT_FLOAT_EQ(a.x, 1.f);

  b.normalize();
  EXPECT_FLOAT_EQ(b.length(), 1.f);
  EXPECT_FLOAT_EQ(b.y, -1.f);

  c.normalize();
  EXPECT_FLOAT_EQ(c.length(), 1.f);
  EXPECT_FLOAT_EQ(c.z, 1.f);

  d.normalize();
  EXPECT_FLOAT_EQ(d.length(), 1.f);
  EXPECT_FLOAT_EQ(d.w, -1.f);
}

TEST(TestVector4, Distance)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, 0.f, 0.f};
  Vector4 b{0.f, -y, 0.f, 0.f};
  Vector4 c{0.f, 0.f, z, 0.f};
  Vector4 d{0.f, 0.f, 0.f, w};
  Vector4 e;

  EXPECT_FLOAT_EQ(Vector4::Distance(a, e), x);
  EXPECT_FLOAT_EQ(Vector4::Distance(b, e), y);
  EXPECT_FLOAT_EQ(Vector4::Distance(c, e), z);
  EXPECT_FLOAT_EQ(Vector4::Distance(d, e), w);
}

TEST(TestVector4, DistanceSquared)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, 0.f, 0.f};
  Vector4 b{0.f, -y, 0.f, 0.f};
  Vector4 c{0.f, 0.f, z, 0.f};
  Vector4 d{0.f, 0.f, 0.f, w};
  Vector4 e;

  EXPECT_FLOAT_EQ(Vector4::DistanceSquared(a, e), x * x);
  EXPECT_FLOAT_EQ(Vector4::DistanceSquared(b, e), y * y);
  EXPECT_FLOAT_EQ(Vector4::DistanceSquared(c, e), z * z);
  EXPECT_FLOAT_EQ(Vector4::DistanceSquared(d, e), w * w);
}

TEST(TestVector4, Equals)
{
  using namespace BABYLON;

  Vector4 a{x, 0.f, z, 0.f};
  Vector4 b{0.f, -y, 0.f, -w};

  EXPECT_NE(a.x, b.x);
  EXPECT_NE(a.y, b.y);
  EXPECT_NE(a.z, b.z);
  EXPECT_NE(a.w, b.w);

  EXPECT_TRUE(!a.equals(b));
  EXPECT_TRUE(!b.equals(a));

  a = a.copyFrom(b);
  EXPECT_FLOAT_EQ(a.x, b.x);
  EXPECT_FLOAT_EQ(a.y, b.y);
  EXPECT_FLOAT_EQ(a.z, b.z);
  EXPECT_FLOAT_EQ(a.w, b.w);

  EXPECT_TRUE(a.equals(b));
  EXPECT_TRUE(b.equals(a));
}
