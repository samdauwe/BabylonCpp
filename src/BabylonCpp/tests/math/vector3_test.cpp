#include <gtest/gtest.h>

#include <babylon/math/vector3.h>

namespace {
float x = 1.2f;
float y = 3.4f;
float z = 5.6f;
}

TEST(TestVector3, Constructor)
{
  using namespace BABYLON;

  // Constructor
  Vector3 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);
  EXPECT_FLOAT_EQ(0.f, v.z);

  v = Vector3{x, y, z};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);
  EXPECT_FLOAT_EQ(z, v.z);

  // Copy constructor
  v = Vector3{v};
  EXPECT_FLOAT_EQ(x, v.x);
  EXPECT_FLOAT_EQ(y, v.y);
  EXPECT_FLOAT_EQ(z, v.z);
}

TEST(TestVector3, CopyFrom)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b = Vector3().copyFrom(a);
  EXPECT_FLOAT_EQ(b.x, a.x);
  EXPECT_FLOAT_EQ(b.y, a.y);
  EXPECT_FLOAT_EQ(b.z, a.z);

  // Ensure that it is a true copy
  a.x = 0.f;
  a.y = -1.f;
  a.z = -2;
  EXPECT_FLOAT_EQ(b.x, x);
  EXPECT_FLOAT_EQ(b.y, y);
  EXPECT_FLOAT_EQ(b.z, z);
}

TEST(TestVector3, CopyFromFloats)
{
  using namespace BABYLON;

  Vector3 v;
  EXPECT_FLOAT_EQ(0.f, v.x);
  EXPECT_FLOAT_EQ(0.f, v.y);
  EXPECT_FLOAT_EQ(0.f, v.z);

  v.copyFromFloats(x, y, z);
  EXPECT_FLOAT_EQ(v.x, x);
  EXPECT_FLOAT_EQ(v.y, y);
  EXPECT_FLOAT_EQ(v.z, z);
}

TEST(TestVector3, GetHashCode)
{
  using namespace BABYLON;

  std::array<Vector3, 8> vects{{
    Vector3{x, y, z},   // 000
    Vector3{x, y, -z},  // 001
    Vector3{x, -y, z},  // 010
    Vector3{x, -y, -z}, // 011
    Vector3{-x, y, z},  // 100
    Vector3{-x, y, -z}, // 101
    Vector3{-x, -y, z}, // 110
    Vector3{-x, -y, -z} // 111
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

TEST(TestVector3, Add)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b{-x, -y, -z};
  Vector3 c;

  // c = a + b
  a.addToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);
  EXPECT_FLOAT_EQ(c.z, 0.f);

  // c = b + b
  c = b.add(b);
  EXPECT_FLOAT_EQ(c.x, 2 * b.x);
  EXPECT_FLOAT_EQ(c.y, 2 * b.y);
  EXPECT_FLOAT_EQ(c.z, 2 * b.z);
}

TEST(TestVector3, AttributeIterate)
{
  using namespace BABYLON;

  Vector3 pos{x, 3 * y, -2 * z};
  Vector3 boundingBoxMin{-x, -y, -z};
  Vector3 boundingBoxMax{2 * x, 2 * y, 2 * z};

  const auto attrs = {&Vector3::x, &Vector3::y, &Vector3::z};
  for (auto& attr : attrs) {
    if (pos.*attr < boundingBoxMin.*attr) {
      boundingBoxMin.*attr = pos.*attr;
    }
    if (pos.*attr > boundingBoxMax.*attr) {
      boundingBoxMax.*attr = pos.*attr;
    }
  }

  EXPECT_FLOAT_EQ(boundingBoxMin.x, -x);
  EXPECT_FLOAT_EQ(boundingBoxMin.y, -y);
  EXPECT_FLOAT_EQ(boundingBoxMin.z, -2 * z);

  EXPECT_FLOAT_EQ(boundingBoxMax.x, 2 * x);
  EXPECT_FLOAT_EQ(boundingBoxMax.y, 3 * y);
  EXPECT_FLOAT_EQ(boundingBoxMax.z, 2 * z);
}

TEST(TestVector3, Subtract)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b{-x, -y, -z};
  Vector3 c;

  // c = a - b
  c = a.subtract(b);
  EXPECT_FLOAT_EQ(c.x, 2.f * x);
  EXPECT_FLOAT_EQ(c.y, 2.f * y);
  EXPECT_FLOAT_EQ(c.z, 2.f * z);

  // c = a - a
  a.subtractToRef(a, c);
  EXPECT_FLOAT_EQ(c.x, 0.f);
  EXPECT_FLOAT_EQ(c.y, 0.f);
  EXPECT_FLOAT_EQ(c.z, 0.f);

  // a = a - a
  a.subtractInPlace(a);
  EXPECT_FLOAT_EQ(a.x, 0.f);
  EXPECT_FLOAT_EQ(a.y, 0.f);
  EXPECT_FLOAT_EQ(a.z, 0.f);
}

TEST(TestVector3, Multiply)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b{-x, -y, -z};
  Vector3 c;

  // c = a * b
  c = a.multiply(b);
  EXPECT_FLOAT_EQ(c.x, -x * x);
  EXPECT_FLOAT_EQ(c.y, -y * y);
  EXPECT_FLOAT_EQ(c.z, -z * z);

  // c = b * b
  b.multiplyToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, x * x);
  EXPECT_FLOAT_EQ(c.y, y * y);
  EXPECT_FLOAT_EQ(c.z, z * z);

  // a = a * a
  a.multiplyInPlace(a);
  EXPECT_FLOAT_EQ(a.x, x * x);
  EXPECT_FLOAT_EQ(a.y, y * y);
  EXPECT_FLOAT_EQ(a.z, z * z);

  // c = Vector2(a.x * 1, a.y * 2, a.z * 3)
  c = a.multiplyByFloats(1.f, 2.f, 3.f);
  EXPECT_FLOAT_EQ(c.x, x * x * 1.f);
  EXPECT_FLOAT_EQ(c.y, y * y * 2.f);
  EXPECT_FLOAT_EQ(c.z, z * z * 3.f);
}

TEST(TestVector3, Divide)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b{-x, -y, -z};
  Vector3 c;

  // c = a / b
  c = a.divide(b);
  EXPECT_FLOAT_EQ(c.x, -1.f);
  EXPECT_FLOAT_EQ(c.y, -1.f);
  EXPECT_FLOAT_EQ(c.z, -1.f);

  // c = b / b
  b.divideToRef(b, c);
  EXPECT_FLOAT_EQ(c.x, 1.f);
  EXPECT_FLOAT_EQ(c.y, 1.f);
  EXPECT_FLOAT_EQ(c.z, 1.f);

  // c = c / c
  c.divideInPlace(c);
  EXPECT_FLOAT_EQ(c.x, 1.f);
  EXPECT_FLOAT_EQ(c.y, 1.f);
  EXPECT_FLOAT_EQ(c.z, 1.f);
}

TEST(TestVector3, Negate)
{
  using namespace BABYLON;

  Vector3 v{x, y, z};

  // v = -v
  v = v.negate();
  EXPECT_FLOAT_EQ(v.x, -x);
  EXPECT_FLOAT_EQ(v.y, -y);
  EXPECT_FLOAT_EQ(v.z, -z);
}

TEST(TestVector3, Dot)
{
  using namespace BABYLON;

  Vector3 a{x, y, z};
  Vector3 b{-x, -y, -z};
  Vector3 c;

  float result = Vector3::Dot(a, b);
  EXPECT_FLOAT_EQ(result, -x * x - y * y - z * z);

  result = Vector3::Dot(a, c);
  EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(TestVector3, Length)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, 0.f};
  Vector3 b{0.f, -y, 0.f};
  Vector3 c{0.f, 0.f, z};
  Vector3 d;

  EXPECT_FLOAT_EQ(a.length(), x);
  EXPECT_FLOAT_EQ(b.length(), y);
  EXPECT_FLOAT_EQ(c.length(), z);
  EXPECT_FLOAT_EQ(d.length(), 0.f);

  a.copyFromFloats(x, y, z);
  EXPECT_FLOAT_EQ(a.length(), std::sqrt(x * x + y * y + z * z));
}

TEST(TestVector3, LengthSquared)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, 0.f};
  Vector3 b{0.f, -y, 0.f};
  Vector3 c{0.f, 0.f, z};
  Vector3 d;

  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x);
  EXPECT_FLOAT_EQ(b.lengthSquared(), y * y);
  EXPECT_FLOAT_EQ(c.lengthSquared(), z * z);
  EXPECT_FLOAT_EQ(d.lengthSquared(), 0.f);

  a.copyFromFloats(x, y, z);
  EXPECT_FLOAT_EQ(a.lengthSquared(), x * x + y * y + z * z);
}

TEST(TestVector3, Normalize)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, 0.f};
  Vector3 b{0.f, -y, 0.f};
  Vector3 c{0.f, 0.f, z};

  a.normalize();
  EXPECT_FLOAT_EQ(a.length(), 1.f);
  EXPECT_FLOAT_EQ(a.x, 1.f);

  b.normalize();
  EXPECT_FLOAT_EQ(b.length(), 1.f);
  EXPECT_FLOAT_EQ(b.y, -1.f);

  c.normalize();
  EXPECT_FLOAT_EQ(c.length(), 1.f);
  EXPECT_FLOAT_EQ(c.z, 1.f);
}

TEST(TestVector3, Distance)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, 0.f};
  Vector3 b{0.f, -y, 0.f};
  Vector3 c{0.f, 0.f, z};
  Vector3 d;

  EXPECT_FLOAT_EQ(Vector3::Distance(a, d), x);
  EXPECT_FLOAT_EQ(Vector3::Distance(b, d), y);
  EXPECT_FLOAT_EQ(Vector3::Distance(c, d), z);
}

TEST(TestVector3, DistanceSquared)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, 0.f};
  Vector3 b{0.f, -y, 0.f};
  Vector3 c{0.f, 0.f, z};
  Vector3 d;

  EXPECT_FLOAT_EQ(Vector3::DistanceSquared(a, d), x * x);
  EXPECT_FLOAT_EQ(Vector3::DistanceSquared(b, d), y * y);
  EXPECT_FLOAT_EQ(Vector3::DistanceSquared(c, d), z * z);
}

TEST(TestVector3, Lerp)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, z};
  Vector3 b{0.f, -y, 0.f};

  EXPECT_TRUE(Vector3::Lerp(a, a, 0.f).equals(Vector3::Lerp(a, a, 0.5f)));
  EXPECT_TRUE(Vector3::Lerp(a, a, 0.f).equals(Vector3::Lerp(a, a, 1.f)));

  EXPECT_TRUE(Vector3::Lerp(a, b, 0.f).equals(a));

  EXPECT_FLOAT_EQ(Vector3::Lerp(a, b, 0.5f).x, x * 0.5f);
  EXPECT_FLOAT_EQ(Vector3::Lerp(a, b, 0.5f).y, -y * 0.5f);
  EXPECT_FLOAT_EQ(Vector3::Lerp(a, b, 0.5f).z, z * 0.5f);

  EXPECT_TRUE(Vector3::Lerp(a, b, 1).equals(b));
}

TEST(TestVector3, Equals)
{
  using namespace BABYLON;

  Vector3 a{x, 0.f, z};
  Vector3 b{0.f, -y, 0.f};

  EXPECT_NE(a.x, b.x);
  EXPECT_NE(a.y, b.y);
  EXPECT_NE(a.z, b.z);

  EXPECT_TRUE(!a.equals(b));
  EXPECT_TRUE(!b.equals(a));

  a = a.copyFrom(b);
  EXPECT_FLOAT_EQ(a.x, b.x);
  EXPECT_FLOAT_EQ(a.y, b.y);
  EXPECT_FLOAT_EQ(a.z, b.z);

  EXPECT_TRUE(a.equals(b));
  EXPECT_TRUE(b.equals(a));
}
