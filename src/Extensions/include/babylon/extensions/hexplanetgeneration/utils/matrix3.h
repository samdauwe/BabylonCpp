#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_MATRIX3_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_MATRIX3_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Vector3;

namespace Extensions {

class Matrix3 {

public:
  Matrix3();
  Matrix3(const Matrix3& other);
  ~Matrix3(); // = default

  /**
   * Matrix concatenation using '*'.
   */
  Matrix3 operator*(const Matrix3& rkMatrix) const;

  /**
   * Matrix * vector [3x3 * 3x1 = 3x1]
   */
  Vector3 operator*(const Vector3& rkVector) const;

  void FromAngleAxis(const Vector3& rkAxis, const float& fRadians);

private:
  std::array<std::array<float, 3>, 3> m;

}; // end of class Matrix3

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_MATRIX3_H
