#include <babylon/math/simd/simd_vector3.h>

#include <babylon/math/matrix.h>
#include <babylon/math/simd/float32x4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace SIMD {

void SIMDVector3::TransformCoordinatesToRefSIMD(const Vector3& vector,
                                                const Matrix& transformation,
                                                Vector3& result)
{
  SIMDVector3::TransformCoordinatesFromFloatsToRefSIMD(
    vector.x, vector.y, vector.z, transformation, result);
}

void SIMDVector3::TransformCoordinatesFromFloatsToRefSIMD(
  float x, float y, float z, const Matrix& transformation, Vector3& result)
{
  const auto& m = transformation.m;

  auto m0 = SIMD::Float32x4::load(m, 0);
  auto m1 = SIMD::Float32x4::load(m, 4);
  auto m2 = SIMD::Float32x4::load(m, 8);
  auto m3 = SIMD::Float32x4::load(m, 12);
  auto r  = SIMD::Float32x4::add(
    SIMD::Float32x4::add(SIMD::Float32x4::mul(SIMD::Float32x4::splat(x), m0),
                         SIMD::Float32x4::mul(SIMD::Float32x4::splat(y), m1)),
    SIMD::Float32x4::add(SIMD::Float32x4::mul(SIMD::Float32x4::splat(z), m2),
                         m3));
  r        = SIMD::Float32x4::div(r, SIMD::Float32x4::swizzle(r, 3, 3, 3, 3));
  result.x = SIMD::Float32x4::extractLane(r, 0);
  result.y = SIMD::Float32x4::extractLane(r, 1);
  result.z = SIMD::Float32x4::extractLane(r, 2);
}

} // end of namespace SIMD
} // end of namespace BABY
