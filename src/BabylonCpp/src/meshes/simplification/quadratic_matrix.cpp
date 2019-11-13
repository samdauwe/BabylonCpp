#include <babylon/meshes/simplification/quadratic_matrix.h>

namespace BABYLON {

QuadraticMatrix::QuadraticMatrix()
{
  for (unsigned int i = 0; i < 10; ++i) {
    data[i] = 0.f;
  }
}

QuadraticMatrix::QuadraticMatrix(const std::array<float, 10>& _data)
{
  for (unsigned int i = 0; i < 10; ++i) {
    data[i] = _data[i];
  }
}

QuadraticMatrix::QuadraticMatrix(const QuadraticMatrix& other) = default;
QuadraticMatrix::QuadraticMatrix(QuadraticMatrix&& other) = default;
QuadraticMatrix& QuadraticMatrix::operator=(const QuadraticMatrix& other) = default;
QuadraticMatrix& QuadraticMatrix::operator=(QuadraticMatrix&& other) = default;

QuadraticMatrix::~QuadraticMatrix() = default;

float QuadraticMatrix::det(unsigned int a11, unsigned int a12, int unsigned a13,
                           unsigned int a21, unsigned int a22, unsigned int a23,
                           unsigned int a31, unsigned int a32, unsigned int a33)
{
  return data[a11] * data[a22] * data[a33] + data[a13] * data[a21] * data[a32]
         + data[a12] * data[a23] * data[a31] - data[a13] * data[a22] * data[a31]
         - data[a11] * data[a23] * data[a32]
         - data[a12] * data[a21] * data[a33];
}

void QuadraticMatrix::addInPlace(const QuadraticMatrix& matrix)
{
  for (unsigned int i = 0; i < 10; ++i) {
    data[i] += matrix.data[i];
  }
}

void QuadraticMatrix::addArrayInPlace(const std::array<float, 10>& _data)
{
  for (unsigned int i = 0; i < 10; ++i) {
    data[i] += _data[i];
  }
}

QuadraticMatrix QuadraticMatrix::add(const QuadraticMatrix& matrix)
{
  QuadraticMatrix m;
  for (unsigned int i = 0; i < 10; ++i) {
    m.data[i] = data[i] + matrix.data[i];
  }
  return m;
}

QuadraticMatrix QuadraticMatrix::FromData(float a, float b, float c, float d)
{
  return QuadraticMatrix(QuadraticMatrix::DataFromNumbers(a, b, c, d));
}

std::array<float, 10> QuadraticMatrix::DataFromNumbers(float a, float b,
                                                       float c, float d)
{
  return {{a * a, a * b, a * c, a * d, //
           b * b, b * c, b * d,        //
           c * c, c * d,               //
           d * d}};
}

} // end of namespace BABYLON
