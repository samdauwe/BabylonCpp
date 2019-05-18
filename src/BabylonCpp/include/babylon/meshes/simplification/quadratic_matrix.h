#ifndef BABYLON_MESHES_SIMPLIFICATION_QUADRATIC_MATRIX_H
#define BABYLON_MESHES_SIMPLIFICATION_QUADRATIC_MATRIX_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT QuadraticMatrix {

public:
  QuadraticMatrix();
  QuadraticMatrix(const std::array<float, 10>& data);
  QuadraticMatrix(const QuadraticMatrix& other);
  QuadraticMatrix(QuadraticMatrix&& other);
  QuadraticMatrix& operator=(const QuadraticMatrix& other);
  QuadraticMatrix& operator=(QuadraticMatrix&& other);
  ~QuadraticMatrix();

  float det(unsigned int a11, unsigned int a12, unsigned int a13, //
            unsigned int a21, unsigned int a22, int unsigned a23, //
            int unsigned a31, int unsigned a32, int unsigned a33  //
  );
  void addInPlace(const QuadraticMatrix& matrix);
  void addArrayInPlace(const std::array<float, 10>& data);
  QuadraticMatrix add(const QuadraticMatrix& matrix);

  static QuadraticMatrix FromData(float a, float b, float c, float d);
  static std::array<float, 10> DataFromNumbers(float a, float b, float c,
                                               float d);

private:
  std::array<float, 10> data;

}; // end of class QuadraticMatrix

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_SIMPLIFICATION_QUADRATIC_MATRIX_H
