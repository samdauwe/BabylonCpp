#ifndef BABYLON_MESH_SIMPLIFICATION_QUADRATIC_MATRIX_H
#define BABYLON_MESH_SIMPLIFICATION_QUADRATIC_MATRIX_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT QuadraticMatrix {

public:
  QuadraticMatrix();
  QuadraticMatrix(const array_t<float, 10>& data);
  ~QuadraticMatrix();

  float det(int a11, int a12, int a13, int a21, int a22, int a23, int a31,
            int a32, int a33);
  void addInPlace(const QuadraticMatrix& matrix);
  void addArrayInPlace(const array_t<float, 10>& data);
  QuadraticMatrix add(const QuadraticMatrix& matrix);

  static QuadraticMatrix FromData(float a, float b, float c, float d);
  static array_t<float, 10> DataFromNumbers(float a, float b, float c, float d);

private:
  array_t<float, 10> data;

}; // end of class QuadraticMatrix

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_QUADRATIC_MATRIX_H
