#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/math/simd/float32x4.h>

TEST(TestSIMDFloat32x4, add)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(-1, -2, 3, 4);
  auto b   = SIMD::Float32x4(3, 3, 3, 3);
  auto res = a + b;

  EXPECT_EQ(res.x(), 2);
  EXPECT_EQ(res.y(), 1);
  EXPECT_EQ(res.z(), 6);
  EXPECT_EQ(res.w(), 7);
}

TEST(TestSIMDFloat32x4, div)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(2, 2, 2, 2);
  auto b   = SIMD::Float32x4(4, 4, 4, 4);
  auto res = a / b;

  EXPECT_EQ(res.x(), 0.5f);
  EXPECT_EQ(res.y(), 0.5f);
  EXPECT_EQ(res.z(), 0.5f);
  EXPECT_EQ(res.w(), 0.5f);
}

TEST(TestSIMDFloat32x4, load)
{
  using namespace BABYLON;

  std::array<float, 4> src{{1, 2, 3, 4}};
  auto res = SIMD::Float32x4(SIMD::Float32x4::load(src));

  EXPECT_EQ(res.x(), 1);
  EXPECT_EQ(res.y(), 2);
  EXPECT_EQ(res.z(), 3);
  EXPECT_EQ(res.w(), 4);
}

TEST(TestSIMDFloat32x4, extractLane)
{
  using namespace BABYLON;

  auto t = SIMD::Float32x4(1, 2, 3, 4);

  EXPECT_EQ(SIMD::Float32x4::extractLane(t.xmm, 2), 3);
}

TEST(TestSIMDFloat32x4, mul)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(-1, -2, 3, 4);
  auto b   = SIMD::Float32x4(3, 3, 3, 3);
  auto res = a * b;

  EXPECT_EQ(res.x(), -3);
  EXPECT_EQ(res.y(), -6);
  EXPECT_EQ(res.z(), 9);
  EXPECT_EQ(res.w(), 12);
}

TEST(TestSIMDFloat32x4, neg)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(-1, -2, 3, 0);
  auto res = SIMD::Float32x4(SIMD::Float32x4::neg(a.xmm));

  EXPECT_EQ(res.x(), 1);
  EXPECT_EQ(res.y(), 2);
  EXPECT_EQ(res.z(), -3);
  EXPECT_EQ(res.w(), -0);
}

TEST(TestSIMDFloat32x4, reciprocalApproximation)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(1, 2, 2, 4).xmm;
  auto res = SIMD::Float32x4(SIMD::Float32x4::reciprocalApproximation(a));

  EXPECT_TRUE(std::abs(res.x() - 1.f) < 1e-3f);
  EXPECT_TRUE(std::abs(res.y() - 0.5f) < 1e-3f);
  EXPECT_TRUE(std::abs(res.z() - 0.5f) < 1e-3f);
  EXPECT_TRUE(std::abs(res.w() - 0.25f) < 1e-3f);
}

TEST(TestSIMDFloat32x4, reciprocalSqrtApproximation)
{
  using namespace BABYLON;
  std::array<float, 4> input{{1.f, 2.f, 3.f, 4.f}};
  auto a   = SIMD::Float32x4::load(input);
  auto res = SIMD::Float32x4::reciprocalSqrtApproximation(a);
  std::array<float, 4> dest;
  SIMD::Float32x4::store(dest, res);

  EXPECT_TRUE(std::abs(dest[0] - (1.f / std::sqrt(input[0]))) < 1e-3f);
  EXPECT_TRUE(std::abs(dest[1] - (1.f / std::sqrt(input[1]))) < 1e-3f);
  EXPECT_TRUE(std::abs(dest[2] - (1.f / std::sqrt(input[2]))) < 1e-3f);
  EXPECT_TRUE(std::abs(dest[3] - (1.f / std::sqrt(input[3]))) < 1e-3f);
}

TEST(TestSIMDFloat32x4, DISABLED_replaceLane)
{
  using namespace BABYLON;

  auto t   = SIMD::Float32x4(1, 2, 3, 4);
  auto res = SIMD::Float32x4(SIMD::Float32x4::replaceLane(t.xmm, 2, 42));

  EXPECT_EQ(res.x(), 1.f);
  EXPECT_EQ(res.y(), 2.f);
  EXPECT_EQ(res.z(), 42.f);
  EXPECT_EQ(res.w(), 4.f);
}

TEST(TestSIMDFloat32x4, shuffle)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(1, 2, 3, 4);
  auto b   = SIMD::Float32x4(5, 6, 7, 8);
  auto t   = SIMD::Float32x4::ShuffleType::LANE_1357;
  auto res = SIMD::Float32x4(SIMD::Float32x4::shuffle(a.xmm, b.xmm, t));

  EXPECT_EQ(res.x(), 2);
  EXPECT_EQ(res.y(), 4);
  EXPECT_EQ(res.z(), 6);
  EXPECT_EQ(res.w(), 8);
}

TEST(TestSIMDFloat32x4, splat)
{
  using namespace BABYLON;

  auto a = SIMD::Float32x4(SIMD::Float32x4::splat(3));

  EXPECT_EQ(a.x(), 3);
  EXPECT_EQ(a.y(), 3);
  EXPECT_EQ(a.z(), 3);
  EXPECT_EQ(a.w(), 3);
}

TEST(TestSIMDFloat32x4, store)
{
  using namespace BABYLON;

  auto t = SIMD::Float32x4(1, 2, 3, 4);
  std::array<float, 4> dest;
  SIMD::Float32x4::store(dest, t.xmm);

  EXPECT_EQ(dest[0], 1);
  EXPECT_EQ(dest[1], 2);
  EXPECT_EQ(dest[2], 3);
  EXPECT_EQ(dest[3], 4);
}

TEST(TestSIMDFloat32x4, sub)
{
  using namespace BABYLON;

  auto a   = SIMD::Float32x4(-1, -2, 3, 4);
  auto b   = SIMD::Float32x4(3, 3, 3, 3);
  auto res = a - b;

  EXPECT_EQ(res.x(), -4);
  EXPECT_EQ(res.y(), -5);
  EXPECT_EQ(res.z(), 0);
  EXPECT_EQ(res.w(), 1);
}

TEST(TestSIMDFloat32x4, swizzle)
{
  using namespace BABYLON;

  auto t   = SIMD::Float32x4(1, 2, 3, 4);
  auto res = SIMD::Float32x4(SIMD::Float32x4::swizzle(t, 1, 2, 0, 3));

  EXPECT_EQ(res.x(), 2);
  EXPECT_EQ(res.y(), 3);
  EXPECT_EQ(res.z(), 1);
  EXPECT_EQ(res.w(), 4);
}
