#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/math/simd/float32x4.h>

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
