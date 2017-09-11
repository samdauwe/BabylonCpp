#include <babylon/math/simd/simd_matrix.h>

#include <babylon/math/matrix.h>
#include <babylon/math/simd/float32x4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace SIMD {

SIMDMatrix::SIMDMatrix()
{
  ::std::fill(m.begin(), m.end(), 0.f);
}

SIMDMatrix::SIMDMatrix(const SIMDMatrix& otherMatrix) : m{otherMatrix.m}
{
}

SIMDMatrix::SIMDMatrix(const Matrix& otherMatrix) : m{otherMatrix.m}
{
}

SIMDMatrix::SIMDMatrix(SIMDMatrix&& otherMatrix) : m{::std::move(otherMatrix.m)}
{
}

SIMDMatrix::SIMDMatrix(Matrix&& otherMatrix) : m{::std::move(otherMatrix.m)}
{
}

SIMDMatrix& SIMDMatrix::operator=(const SIMDMatrix& otherMatrix)
{
  if (&otherMatrix != this) {
    m = otherMatrix.m;
  }

  return *this;
}

SIMDMatrix& SIMDMatrix::operator=(const Matrix& otherMatrix)
{
  m = otherMatrix.m;

  return *this;
}

SIMDMatrix& SIMDMatrix::operator=(SIMDMatrix&& otherMatrix)
{
  if (&otherMatrix != this) {
    m = ::std::move(otherMatrix.m);
  }

  return *this;
}

SIMDMatrix& SIMDMatrix::operator=(Matrix&& otherMatrix)
{
  m = ::std::move(otherMatrix.m);

  return *this;
}

SIMDMatrix::~SIMDMatrix()
{
}

SIMDMatrix& SIMDMatrix::multiplyToArraySIMD(const Matrix& other,
                                            std::array<float, 16>& result,
                                            unsigned int offset)
{
  const auto& tm = m;
  const auto& om = other.m;

  auto m0 = SIMD::Float32x4::load(om, 0);
  auto m1 = SIMD::Float32x4::load(om, 4);
  auto m2 = SIMD::Float32x4::load(om, 8);
  auto m3 = SIMD::Float32x4::load(om, 12);

  for (unsigned int i = 0; i < 16; i += 4) {
    SIMD::Float32x4::store(
      result, i + offset,
      SIMD::Float32x4::add(
        SIMD::Float32x4::mul(SIMD::Float32x4::splat(tm[i]), m0),
        SIMD::Float32x4::add(
          SIMD::Float32x4::mul(SIMD::Float32x4::splat(tm[i + 1]), m1),
          SIMD::Float32x4::add(
            SIMD::Float32x4::mul(SIMD::Float32x4::splat(tm[i + 2]), m2),
            SIMD::Float32x4::mul(SIMD::Float32x4::splat(tm[i + 3]), m3)))));
  }

  return *this;
}

SIMDMatrix& SIMDMatrix::invertToRefSIMD(Matrix& other)
{
  using ShuffleType = SIMD::Float32x4::ShuffleType;

  const auto& src = m;
  auto& dest      = other.m;

  // Load the 4 rows
  auto src0 = SIMD::Float32x4::load(src, 0);
  auto src1 = SIMD::Float32x4::load(src, 4);
  auto src2 = SIMD::Float32x4::load(src, 8);
  auto src3 = SIMD::Float32x4::load(src, 12);

  // Transpose the source matrix.  Sort of.  Not a true transpose operation
  auto tmp1 = SIMD::Float32x4::shuffle(src0, src1, ShuffleType::LANE_0145);
  auto row1 = SIMD::Float32x4::shuffle(src2, src3, ShuffleType::LANE_0145);
  auto row0 = SIMD::Float32x4::shuffle(tmp1, row1, ShuffleType::LANE_0246);
  row1      = SIMD::Float32x4::shuffle(row1, tmp1, ShuffleType::LANE_1357);

  tmp1      = SIMD::Float32x4::shuffle(src0, src1, ShuffleType::LANE_2367);
  auto row3 = SIMD::Float32x4::shuffle(src2, src3, ShuffleType::LANE_2367);
  auto row2 = SIMD::Float32x4::shuffle(tmp1, row3, ShuffleType::LANE_0246);
  row3      = SIMD::Float32x4::shuffle(row3, tmp1, ShuffleType::LANE_1357);

  tmp1        = SIMD::Float32x4::mul(row2, row3);
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  auto minor0 = SIMD::Float32x4::mul(row1, tmp1);
  auto minor1 = SIMD::Float32x4::mul(row0, tmp1);
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor0      = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row1, tmp1), minor0);
  minor1      = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row0, tmp1), minor1);
  minor1      = SIMD::Float32x4::swizzle(minor1, 2, 3, 0, 1); // 0x4E = 01001110

  // ----
  tmp1        = SIMD::Float32x4::mul(row1, row2);
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  minor0      = SIMD::Float32x4::add(SIMD::Float32x4::mul(row3, tmp1), minor0);
  auto minor3 = SIMD::Float32x4::mul(row0, tmp1);
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor0      = SIMD::Float32x4::sub(minor0, SIMD::Float32x4::mul(row3, tmp1));
  minor3      = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row0, tmp1), minor3);
  minor3      = SIMD::Float32x4::swizzle(minor3, 2, 3, 0, 1); // 0x4E = 01001110

  // ----
  tmp1        = SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(row1, 2, 3, 0, 1),
                              row3);                 // 0x4E = 01001110
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  row2        = SIMD::Float32x4::swizzle(row2, 2, 3, 0, 1); // 0x4E = 01001110
  minor0      = SIMD::Float32x4::add(SIMD::Float32x4::mul(row2, tmp1), minor0);
  auto minor2 = SIMD::Float32x4::mul(row0, tmp1);
  tmp1        = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor0      = SIMD::Float32x4::sub(minor0, SIMD::Float32x4::mul(row2, tmp1));
  minor2      = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row0, tmp1), minor2);
  minor2      = SIMD::Float32x4::swizzle(minor2, 2, 3, 0, 1); // 0x4E = 01001110

  // ----
  tmp1   = SIMD::Float32x4::mul(row0, row1);
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  minor2 = SIMD::Float32x4::add(SIMD::Float32x4::mul(row3, tmp1), minor2);
  minor3 = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row2, tmp1), minor3);
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor2 = SIMD::Float32x4::sub(SIMD::Float32x4::mul(row3, tmp1), minor2);
  minor3 = SIMD::Float32x4::sub(minor3, SIMD::Float32x4::mul(row2, tmp1));

  // ----
  tmp1   = SIMD::Float32x4::mul(row0, row3);
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  minor1 = SIMD::Float32x4::sub(minor1, SIMD::Float32x4::mul(row2, tmp1));
  minor2 = SIMD::Float32x4::add(SIMD::Float32x4::mul(row1, tmp1), minor2);
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor1 = SIMD::Float32x4::add(SIMD::Float32x4::mul(row2, tmp1), minor1);
  minor2 = SIMD::Float32x4::sub(minor2, SIMD::Float32x4::mul(row1, tmp1));

  // ----
  tmp1   = SIMD::Float32x4::mul(row0, row2);
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 1, 0, 3, 2); // 0xB1 = 10110001
  minor1 = SIMD::Float32x4::add(SIMD::Float32x4::mul(row3, tmp1), minor1);
  minor3 = SIMD::Float32x4::sub(minor3, SIMD::Float32x4::mul(row1, tmp1));
  tmp1   = SIMD::Float32x4::swizzle(tmp1, 2, 3, 0, 1); // 0x4E = 01001110
  minor1 = SIMD::Float32x4::sub(minor1, SIMD::Float32x4::mul(row3, tmp1));
  minor3 = SIMD::Float32x4::add(SIMD::Float32x4::mul(row1, tmp1), minor3);

  // Compute determinant
  auto det = SIMD::Float32x4::mul(row0, minor0);
  det      = SIMD::Float32x4::add(SIMD::Float32x4::swizzle(det, 2, 3, 0, 1),
                             det); // 0x4E = 01001110
  det      = SIMD::Float32x4::add(SIMD::Float32x4::swizzle(det, 1, 0, 3, 2),
                             det); // 0xB1 = 10110001
  tmp1     = SIMD::Float32x4::reciprocalApproximation(det);
  det      = SIMD::Float32x4::sub(
    SIMD::Float32x4::add(tmp1, tmp1),
    SIMD::Float32x4::mul(det, SIMD::Float32x4::mul(tmp1, tmp1)));
  det = SIMD::Float32x4::swizzle(det, 0, 0, 0, 0);

  // Compute final values by multiplying with 1/det
  SIMD::Float32x4::store(dest, 0, SIMD::Float32x4::mul(det, minor0));
  SIMD::Float32x4::store(dest, 4, SIMD::Float32x4::mul(det, minor1));
  SIMD::Float32x4::store(dest, 8, minor2 = SIMD::Float32x4::mul(det, minor2));
  SIMD::Float32x4::store(dest, 12, SIMD::Float32x4::mul(det, minor3));

  return *this;
}

void SIMDMatrix::LookAtLHToRefSIMD(const Vector3& eyeRef,
                                   const Vector3& targetRef,
                                   const Vector3& upRef, Matrix& result)
{
  using ShuffleType = SIMD::Float32x4::ShuffleType;

  auto& out = result.m;
  SIMD::Float32x4 center(targetRef.x, targetRef.y, targetRef.z, 0.f);
  SIMD::Float32x4 eye(eyeRef.x, eyeRef.y, eyeRef.z, 0.f);
  SIMD::Float32x4 up(upRef.x, upRef.y, upRef.z, 0.f);

  auto f   = SIMD::Float32x4::sub(center, eye);
  auto tmp = SIMD::Float32x4::mul(f, f);
  tmp      = SIMD::Float32x4::add(
    tmp, SIMD::Float32x4::add(SIMD::Float32x4::swizzle(tmp, 1, 2, 0, 3),
                              SIMD::Float32x4::swizzle(tmp, 2, 0, 1, 3)));
  f = SIMD::Float32x4::mul(f,
                           SIMD::Float32x4::reciprocalSqrtApproximation(tmp));

  tmp = SIMD::Float32x4::mul(up, up);
  tmp = SIMD::Float32x4::add(
    tmp, SIMD::Float32x4::add(SIMD::Float32x4::swizzle(tmp, 1, 2, 0, 3),
                              SIMD::Float32x4::swizzle(tmp, 2, 0, 1, 3)));
  up     = SIMD::Float32x4::mul(up,
                            SIMD::Float32x4::reciprocalSqrtApproximation(tmp));
  auto s = SIMD::Float32x4::sub(
    SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(f, 1, 2, 0, 3),
                         SIMD::Float32x4::swizzle(up, 2, 0, 1, 3)),
    SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(f, 2, 0, 1, 3),
                         SIMD::Float32x4::swizzle(up, 1, 2, 0, 3)));
  tmp = SIMD::Float32x4::mul(s, s);
  tmp = SIMD::Float32x4::add(
    tmp, SIMD::Float32x4::add(SIMD::Float32x4::swizzle(tmp, 1, 2, 0, 3),
                              SIMD::Float32x4::swizzle(tmp, 2, 0, 1, 3)));
  s      = SIMD::Float32x4::mul(s,
                           SIMD::Float32x4::reciprocalSqrtApproximation(tmp));
  auto u = SIMD::Float32x4::sub(
    SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(s, 1, 2, 0, 3),
                         SIMD::Float32x4::swizzle(f, 2, 0, 1, 3)),
    SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(s, 2, 0, 1, 3),
                         SIMD::Float32x4::swizzle(f, 1, 2, 0, 3)));
  tmp = SIMD::Float32x4::mul(s, s);
  tmp = SIMD::Float32x4::add(
    tmp, SIMD::Float32x4::add(SIMD::Float32x4::swizzle(tmp, 1, 2, 0, 3),
                              SIMD::Float32x4::swizzle(tmp, 2, 0, 1, 3)));
  s = SIMD::Float32x4::mul(s,
                           SIMD::Float32x4::reciprocalSqrtApproximation(tmp));

  auto zero  = SIMD::Float32x4::splat(0.f);
  s          = SIMD::Float32x4::neg(s);
  auto tmp01 = SIMD::Float32x4::shuffle(s, u, ShuffleType::LANE_0145);
  auto tmp23 = SIMD::Float32x4::shuffle(f, zero, ShuffleType::LANE_0145);
  auto a0    = SIMD::Float32x4::shuffle(tmp01, tmp23, ShuffleType::LANE_0246);
  auto a1    = SIMD::Float32x4::shuffle(tmp01, tmp23, ShuffleType::LANE_1357);
  auto a2    = SIMD::Float32x4::shuffle(
    SIMD::Float32x4::shuffle(s, u, ShuffleType::LANE_2367),
    SIMD::Float32x4::shuffle(f, zero, ShuffleType::LANE_2367),
    ShuffleType::LANE_0246);
  SIMD::Float32x4 a3(0.f, 0.f, 0.f, 1.f);

  SIMD::Float32x4 b(1.f, 0.f, 0.f, 0.f);
  SIMD::Float32x4::store(
    out, 0,
    SIMD::Float32x4::add(
      SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 0, 0, 0, 0), a0),
      SIMD::Float32x4::add(
        SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 1, 1, 1, 1), a1),
        SIMD::Float32x4::add(
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 2, 2, 2, 2), a2),
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 3, 3, 3, 3), a3)))));

  b = SIMD::Float32x4(0.0, 1.0, 0.0, 0.0);
  SIMD::Float32x4::store(
    out, 4,
    SIMD::Float32x4::add(
      SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 0, 0, 0, 0), a0),
      SIMD::Float32x4::add(
        SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 1, 1, 1, 1), a1),
        SIMD::Float32x4::add(
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 2, 2, 2, 2), a2),
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 3, 3, 3, 3), a3)))));

  b = SIMD::Float32x4(0.f, 0.f, 1.f, 0.f);
  SIMD::Float32x4::store(
    out, 8,
    SIMD::Float32x4::add(
      SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 0, 0, 0, 0), a0),
      SIMD::Float32x4::add(
        SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 1, 1, 1, 1), a1),
        SIMD::Float32x4::add(
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 2, 2, 2, 2), a2),
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 3, 3, 3, 3), a3)))));

  b = SIMD::Float32x4::replaceLane(SIMD::Float32x4::neg(eye), 3, 1.0);
  SIMD::Float32x4::store(
    out, 12,
    SIMD::Float32x4::add(
      SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 0, 0, 0, 0), a0),
      SIMD::Float32x4::add(
        SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 1, 1, 1, 1), a1),
        SIMD::Float32x4::add(
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 2, 2, 2, 2), a2),
          SIMD::Float32x4::mul(SIMD::Float32x4::swizzle(b, 3, 3, 3, 3), a3)))));
}

} // end of namespace SIMD
} // namespace BABYLON
