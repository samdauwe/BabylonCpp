#include <babylon/extensions/hexplanetgeneration/utils/matrix3.h>

#include <cmath>

#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

Matrix3::Matrix3()
    : m{{{{0.f, 0.f, 0.f}}, {{0.f, 0.f, 0.f}}, {{0.f, 0.f, 0.f}}}}
{
}

Matrix3::Matrix3(const Matrix3& other) : m{other.m}
{
}

Matrix3::~Matrix3()
{
}

Matrix3 Matrix3::operator*(const Matrix3& rkMatrix) const
{
  Matrix3 kProd;
  for (size_t iRow = 0; iRow < 3; ++iRow) {
    for (size_t iCol = 0; iCol < 3; ++iCol) {
      kProd.m[iRow][iCol] = m[iRow][0] * rkMatrix.m[0][iCol]
                            + m[iRow][1] * rkMatrix.m[1][iCol]
                            + m[iRow][2] * rkMatrix.m[2][iCol];
    }
  }
  return kProd;
}

Vector3 Matrix3::operator*(const Vector3& rkVector) const
{
  std::array<float, 3> kProd;
  for (unsigned int iRow = 0; iRow < 3; ++iRow) {
    kProd[iRow] = m[iRow][0] * rkVector[0] + //
                  m[iRow][1] * rkVector[1] + //
                  m[iRow][2] * rkVector[2];  //
  }
  return Vector3(kProd[0], kProd[1], kProd[2]);
}

void Matrix3::FromAngleAxis(const Vector3& rkAxis, const float& fRadians)
{
  float fCos         = std::cos(fRadians);
  float fSin         = std::cos(fRadians);
  float fOneMinusCos = 1.f - fCos;
  float fX2          = rkAxis.x * rkAxis.x;
  float fY2          = rkAxis.y * rkAxis.y;
  float fZ2          = rkAxis.z * rkAxis.z;
  float fXYM         = rkAxis.x * rkAxis.y * fOneMinusCos;
  float fXZM         = rkAxis.x * rkAxis.z * fOneMinusCos;
  float fYZM         = rkAxis.y * rkAxis.z * fOneMinusCos;
  float fXSin        = rkAxis.x * fSin;
  float fYSin        = rkAxis.y * fSin;
  float fZSin        = rkAxis.z * fSin;

  m[0][0] = fX2 * fOneMinusCos + fCos;
  m[0][1] = fXYM - fZSin;
  m[0][2] = fXZM + fYSin;
  m[1][0] = fXYM + fZSin;
  m[1][1] = fY2 * fOneMinusCos + fCos;
  m[1][2] = fYZM - fXSin;
  m[2][0] = fXZM - fYSin;
  m[2][1] = fYZM + fXSin;
  m[2][2] = fZ2 * fOneMinusCos + fCos;
}

} // end of namespace Extensions
} // namespace BABYLON
