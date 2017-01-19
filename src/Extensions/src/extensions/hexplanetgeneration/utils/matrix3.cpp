#include <babylon/extensions/hexplanetgeneration/utils/matrix3.h>

#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

Matrix3::Matrix3()
{
}

Matrix3::Matrix3(const Matrix3& /*other*/)
{
}

Matrix3::~Matrix3()
{
}

Matrix3 Matrix3::operator*(const Matrix3& /*rkMatrix*/) const
{
  return Matrix3();
}

Vector3 Matrix3::operator*(const Vector3& /*rkVector*/) const
{
  return Vector3();
}

void Matrix3::FromAngleAxis(const Vector3& /*rkAxis*/,
                            const float& /*fRadians*/)
{
}

} // end of namespace Extensions
} // end of namespace BABY
