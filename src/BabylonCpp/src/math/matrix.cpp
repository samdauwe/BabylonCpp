#include <babylon/math/matrix.h>

#include <babylon/cameras/camera.h>
#include <babylon/cameras/vr/vr_fov.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/plane.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/tmp.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/math/viewport.h>

// SIMD
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
#include <babylon/math/simd/simd_matrix.h>
#endif

namespace BABYLON {

Matrix::Matrix()
{
  std::fill(m.begin(), m.end(), 0.f);
}

Matrix::Matrix(const Matrix& otherMatrix) : m{otherMatrix.m}
{
}

Matrix::Matrix(Matrix&& otherMatrix) : m{std::move(otherMatrix.m)}
{
}

Matrix& Matrix::operator=(const Matrix& otherMatrix)
{
  if (&otherMatrix != this) {
    m = otherMatrix.m;
  }

  return *this;
}

Matrix& Matrix::operator=(Matrix&& otherMatrix)
{
  if (&otherMatrix != this) {
    std::swap(m, otherMatrix.m);
  }

  return *this;
}

Matrix::~Matrix()
{
}

Matrix Matrix::copy() const
{
  return Matrix(*this);
}

std::unique_ptr<Matrix> Matrix::clone() const
{
  return std_util::make_unique<Matrix>(*this);
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix)
{
  os << "{\"M\":[";
  for (unsigned int i = 0; i < 15; ++i) {
    os << matrix.m[i] << ",";
  }
  os << matrix.m[15] << "}";
  return os;
}

const char* Matrix::getClassName() const
{
  return "Matrix";
}

int Matrix::getHashCode() const
{
  float hash = m[0];
  for (unsigned int i = 1; i < 16; ++i) {
    hash = std::pow((hash * 397), m[i]);
  }
  return static_cast<int>(hash);
}

/** Properties **/
bool Matrix::isIdentity() const
{
  if (!std_util::almost_equal(m[0], 1.f) || !std_util::almost_equal(m[5], 1.f)
      || !std_util::almost_equal(m[10], 1.f)
      || !std_util::almost_equal(m[15], 1.f))
    return false;

  if (!std_util::almost_equal(m[1], 0.f) || !std_util::almost_equal(m[2], 0.f)
      || !std_util::almost_equal(m[3], 0.f)
      || !std_util::almost_equal(m[4], 0.f)
      || !std_util::almost_equal(m[6], 0.f)
      || !std_util::almost_equal(m[7], 0.f)
      || !std_util::almost_equal(m[8], 0.f)
      || !std_util::almost_equal(m[9], 0.f)
      || !std_util::almost_equal(m[11], 0.f)
      || !std_util::almost_equal(m[12], 0.f)
      || !std_util::almost_equal(m[13], 0.f)
      || !std_util::almost_equal(m[14], 0.f))
    return false;

  return true;
}

float Matrix::determinant() const
{
  float temp1 = (m[10] * m[15]) - (m[11] * m[14]);
  float temp2 = (m[9] * m[15]) - (m[11] * m[13]);
  float temp3 = (m[9] * m[14]) - (m[10] * m[13]);
  float temp4 = (m[8] * m[15]) - (m[11] * m[12]);
  float temp5 = (m[8] * m[14]) - (m[10] * m[12]);
  float temp6 = (m[8] * m[13]) - (m[9] * m[12]);

  return ((((m[0] * (((m[5] * temp1) - (m[6] * temp2)) + (m[7] * temp3)))
            - (m[1] * (((m[4] * temp1) - (m[6] * temp4)) + (m[7] * temp5))))
           + (m[2] * (((m[4] * temp2) - (m[5] * temp4)) + (m[7] * temp6))))
          - (m[3] * (((m[4] * temp3) - (m[5] * temp5)) + (m[6] * temp6))));
}

/** Methods **/
Float32Array Matrix::toArray() const
{
  Float32Array v(m.size());
  std::copy(m.begin(), m.end(), v.begin());

  return v;
}

Float32Array Matrix::asArray() const
{
  return toArray();
}

Matrix& Matrix::invert()
{
  invertToRef(*this);

  return *this;
}

Matrix& Matrix::reset()
{
  std::fill(m.begin(), m.end(), 0.f);

  return *this;
}

Matrix Matrix::subtract(const Matrix& other)
{
  Matrix result;

  subtractFromRef(other, result);

  return result;
}

Matrix& Matrix::subtractFromRef(const Matrix& other, Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index] = m[index] - other.m[index];
  }

  return *this;
}

Matrix Matrix::add(const Matrix& other)
{
  Matrix result;

  addToRef(other, result);

  return result;
}

Matrix& Matrix::addToRef(const Matrix& other, Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index] = m[index] + other.m[index];
  }

  return *this;
}

Matrix& Matrix::addToSelf(const Matrix& other)
{
  for (unsigned int index = 0; index < 16; ++index) {
    m[index] += other.m[index];
  }

  return *this;
}

Matrix& Matrix::invertToRef(Matrix& other)
{
  float l1  = m[0];
  float l2  = m[1];
  float l3  = m[2];
  float l4  = m[3];
  float l5  = m[4];
  float l6  = m[5];
  float l7  = m[6];
  float l8  = m[7];
  float l9  = m[8];
  float l10 = m[9];
  float l11 = m[10];
  float l12 = m[11];
  float l13 = m[12];
  float l14 = m[13];
  float l15 = m[14];
  float l16 = m[15];
  float l17 = (l11 * l16) - (l12 * l15);
  float l18 = (l10 * l16) - (l12 * l14);
  float l19 = (l10 * l15) - (l11 * l14);
  float l20 = (l9 * l16) - (l12 * l13);
  float l21 = (l9 * l15) - (l11 * l13);
  float l22 = (l9 * l14) - (l10 * l13);
  float l23 = ((l6 * l17) - (l7 * l18)) + (l8 * l19);
  float l24 = -(((l5 * l17) - (l7 * l20)) + (l8 * l21));
  float l25 = ((l5 * l18) - (l6 * l20)) + (l8 * l22);
  float l26 = -(((l5 * l19) - (l6 * l21)) + (l7 * l22));
  float l27 = 1.f / ((((l1 * l23) + (l2 * l24)) + (l3 * l25)) + (l4 * l26));
  float l28 = (l7 * l16) - (l8 * l15);
  float l29 = (l6 * l16) - (l8 * l14);
  float l30 = (l6 * l15) - (l7 * l14);
  float l31 = (l5 * l16) - (l8 * l13);
  float l32 = (l5 * l15) - (l7 * l13);
  float l33 = (l5 * l14) - (l6 * l13);
  float l34 = (l7 * l12) - (l8 * l11);
  float l35 = (l6 * l12) - (l8 * l10);
  float l36 = (l6 * l11) - (l7 * l10);
  float l37 = (l5 * l12) - (l8 * l9);
  float l38 = (l5 * l11) - (l7 * l9);
  float l39 = (l5 * l10) - (l6 * l9);

  other.m[0]  = l23 * l27;
  other.m[4]  = l24 * l27;
  other.m[8]  = l25 * l27;
  other.m[12] = l26 * l27;
  other.m[1]  = -(((l2 * l17) - (l3 * l18)) + (l4 * l19)) * l27;
  other.m[5]  = (((l1 * l17) - (l3 * l20)) + (l4 * l21)) * l27;
  other.m[9]  = -(((l1 * l18) - (l2 * l20)) + (l4 * l22)) * l27;
  other.m[13] = (((l1 * l19) - (l2 * l21)) + (l3 * l22)) * l27;
  other.m[2]  = (((l2 * l28) - (l3 * l29)) + (l4 * l30)) * l27;
  other.m[6]  = -(((l1 * l28) - (l3 * l31)) + (l4 * l32)) * l27;
  other.m[10] = (((l1 * l29) - (l2 * l31)) + (l4 * l33)) * l27;
  other.m[14] = -(((l1 * l30) - (l2 * l32)) + (l3 * l33)) * l27;
  other.m[3]  = -(((l2 * l34) - (l3 * l35)) + (l4 * l36)) * l27;
  other.m[7]  = (((l1 * l34) - (l3 * l37)) + (l4 * l38)) * l27;
  other.m[11] = -(((l1 * l35) - (l2 * l37)) + (l4 * l39)) * l27;
  other.m[15] = (((l1 * l36) - (l2 * l38)) + (l3 * l39)) * l27;

  return *this;
}

Matrix& Matrix::setTranslation(const Vector3& vector3)
{
  m[12] = vector3.x;
  m[13] = vector3.y;
  m[14] = vector3.z;

  return *this;
}

Vector3 Matrix::getTranslation() const
{
  return Vector3(m[12], m[13], m[14]);
}

Matrix Matrix::multiply(Matrix& other)
{
  Matrix result;

  multiplyToRef(other, result);

  return result;
}

Matrix Matrix::multiply(Matrix&& other)
{
  Matrix result;

  multiplyToRef(other, result);

  return result;
}

Matrix& Matrix::copyFrom(const Matrix& other)
{
  for (unsigned int index = 0; index < 16; ++index) {
    m[index] = other.m[index];
  }

  return *this;
}

Matrix& Matrix::copyToArray(std::array<float, 16>& array, unsigned int offset)
{
  if (offset != 0)
    return *this;

  for (unsigned int index = 0; index != 16; ++index) {
    array[offset + index] = m[index];
  }

  return *this;
}

Matrix& Matrix::copyToArray(Float32Array& array, unsigned int offset)
{
  if (array.size() < 16 + offset)
    return *this;

  for (unsigned int index = 0; index != 16; ++index) {
    array[offset + index] = m[index];
  }

  return *this;
}

Matrix& Matrix::multiplyToRef(const Matrix& other, Matrix& result)
{
  multiplyToArray(other, result.m, 0);

  return *this;
}

Matrix& Matrix::multiplyToArray(const Matrix& other,
                                std::array<float, 16>& result,
                                unsigned int offset)
{
  float tm0  = m[0];
  float tm1  = m[1];
  float tm2  = m[2];
  float tm3  = m[3];
  float tm4  = m[4];
  float tm5  = m[5];
  float tm6  = m[6];
  float tm7  = m[7];
  float tm8  = m[8];
  float tm9  = m[9];
  float tm10 = m[10];
  float tm11 = m[11];
  float tm12 = m[12];
  float tm13 = m[13];
  float tm14 = m[14];
  float tm15 = m[15];

  float om0  = other.m[0];
  float om1  = other.m[1];
  float om2  = other.m[2];
  float om3  = other.m[3];
  float om4  = other.m[4];
  float om5  = other.m[5];
  float om6  = other.m[6];
  float om7  = other.m[7];
  float om8  = other.m[8];
  float om9  = other.m[9];
  float om10 = other.m[10];
  float om11 = other.m[11];
  float om12 = other.m[12];
  float om13 = other.m[13];
  float om14 = other.m[14];
  float om15 = other.m[15];

  result[offset]     = tm0 * om0 + tm1 * om4 + tm2 * om8 + tm3 * om12;
  result[offset + 1] = tm0 * om1 + tm1 * om5 + tm2 * om9 + tm3 * om13;
  result[offset + 2] = tm0 * om2 + tm1 * om6 + tm2 * om10 + tm3 * om14;
  result[offset + 3] = tm0 * om3 + tm1 * om7 + tm2 * om11 + tm3 * om15;

  result[offset + 4] = tm4 * om0 + tm5 * om4 + tm6 * om8 + tm7 * om12;
  result[offset + 5] = tm4 * om1 + tm5 * om5 + tm6 * om9 + tm7 * om13;
  result[offset + 6] = tm4 * om2 + tm5 * om6 + tm6 * om10 + tm7 * om14;
  result[offset + 7] = tm4 * om3 + tm5 * om7 + tm6 * om11 + tm7 * om15;

  result[offset + 8]  = tm8 * om0 + tm9 * om4 + tm10 * om8 + tm11 * om12;
  result[offset + 9]  = tm8 * om1 + tm9 * om5 + tm10 * om9 + tm11 * om13;
  result[offset + 10] = tm8 * om2 + tm9 * om6 + tm10 * om10 + tm11 * om14;
  result[offset + 11] = tm8 * om3 + tm9 * om7 + tm10 * om11 + tm11 * om15;

  result[offset + 12] = tm12 * om0 + tm13 * om4 + tm14 * om8 + tm15 * om12;
  result[offset + 13] = tm12 * om1 + tm13 * om5 + tm14 * om9 + tm15 * om13;
  result[offset + 14] = tm12 * om2 + tm13 * om6 + tm14 * om10 + tm15 * om14;
  result[offset + 15] = tm12 * om3 + tm13 * om7 + tm14 * om11 + tm15 * om15;

  return *this;
}

Matrix& Matrix::multiplyToArray(const Matrix& other, Float32Array& result,
                                unsigned int offset)
{
  if (result.size() < 16 + offset)
    return *this;

  std::array<float, 16> array;
  multiplyToArray(other, array, offset);
  for (unsigned int i = 0; i != 16; ++i) {
    result[i] = array[i];
  }

  return *this;
}

bool Matrix::equals(const Matrix& other) const
{
  return (std_util::almost_equal(m[0], other.m[0])
          && std_util::almost_equal(m[1], other.m[1])
          && std_util::almost_equal(m[2], other.m[2])
          && std_util::almost_equal(m[3], other.m[3])
          && std_util::almost_equal(m[4], other.m[4])
          && std_util::almost_equal(m[5], other.m[5])
          && std_util::almost_equal(m[6], other.m[6])
          && std_util::almost_equal(m[7], other.m[7])
          && std_util::almost_equal(m[8], other.m[8])
          && std_util::almost_equal(m[9], other.m[9])
          && std_util::almost_equal(m[10], other.m[10])
          && std_util::almost_equal(m[11], other.m[11])
          && std_util::almost_equal(m[12], other.m[12])
          && std_util::almost_equal(m[13], other.m[13])
          && std_util::almost_equal(m[14], other.m[14])
          && std_util::almost_equal(m[15], other.m[15]));
}

bool Matrix::decompose(Vector3& scale, Quaternion& rotation,
                       Vector3& translation) const
{
  translation.x = m[12];
  translation.y = m[13];
  translation.z = m[14];

  float xs = MathTools::Sign(m[0] * m[1] * m[2] * m[3]) < 0 ? -1.f : 1.f;
  float ys = MathTools::Sign(m[4] * m[5] * m[6] * m[7]) < 0 ? -1.f : 1.f;
  float zs = MathTools::Sign(m[8] * m[9] * m[10] * m[11]) < 0 ? -1.f : 1.f;

  scale.x = xs * std::sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
  scale.y = ys * std::sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
  scale.z = zs * std::sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

  if (std_util::almost_equal(scale.x, 0.f)
      || std_util::almost_equal(scale.y, 0.f)
      || std_util::almost_equal(scale.z, 0.f)) {
    rotation.x = 0.f;
    rotation.y = 0.f;
    rotation.z = 0.f;
    rotation.w = 1.f;
    return false;
  }

  Matrix::FromValuesToRef(
    m[0] / scale.x, m[1] / scale.x, m[2] / scale.x, 0.f,  //
    m[4] / scale.y, m[5] / scale.y, m[6] / scale.y, 0.f,  //
    m[8] / scale.z, m[9] / scale.z, m[10] / scale.z, 0.f, //
    0.f, 0.f, 0.f, 1.f, Tmp::MatrixArray[0]);

  Quaternion::FromRotationMatrixToRef(Tmp::MatrixArray[0], rotation);

  return true;
}

Matrix Matrix::getRotationMatrix() const
{
  Matrix result = Matrix::Identity();

  getRotationMatrixToRef(result);

  return result;
}

void Matrix::getRotationMatrixToRef(Matrix& result) const
{
  float xs = m[0] * m[1] * m[2] * m[3] < 0 ? -1 : 1;
  float ys = m[4] * m[5] * m[6] * m[7] < 0 ? -1 : 1;
  float zs = m[8] * m[9] * m[10] * m[11] < 0 ? -1 : 1;

  float sx = xs * std::sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
  float sy = ys * std::sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
  float sz = zs * std::sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

  Matrix::FromValuesToRef(m[0] / sx, m[1] / sx, m[2] / sx, 0.f,  //
                          m[4] / sy, m[5] / sy, m[6] / sy, 0.f,  //
                          m[8] / sz, m[9] / sz, m[10] / sz, 0.f, //
                          0.f, 0.f, 0.f, 1.f, result);
}

/** Operator overloading **/
Matrix Matrix::operator+(const Matrix& other)
{
  return add(other);
}

Matrix& Matrix::operator+=(const Matrix& other)
{
  return addToSelf(other);
}

Matrix Matrix::operator-(const Matrix& other)
{
  return subtract(other);
}

Matrix Matrix::operator*(Matrix& other)
{
  return multiply(other);
}

bool Matrix::operator==(const Matrix& other) const
{
  return equals(other);
}

bool Matrix::operator!=(const Matrix& other) const
{
  return !equals(other);
}

float& Matrix::operator[](unsigned int index)
{
  return m[index];
}

const float& Matrix::operator[](unsigned int index) const
{
  return m[index];
}

/** Statics **/
Matrix Matrix::FromArray(const Float32Array& array, unsigned int offset)
{
  Matrix result;

  Matrix::FromArrayToRef(array, offset, result);

  return result;
}

void Matrix::FromArrayToRef(const Float32Array& array, unsigned int offset,
                            Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index] = array[index + offset];
  }
}

void Matrix::FromFloat32ArrayToRefScaled(const Float32Array& array,
                                         unsigned int offset, float scale,
                                         Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index] = array[index + offset] * scale;
  }
}

void Matrix::FromValuesToRef(float initialM11, float initialM12,
                             float initialM13, float initialM14,
                             float initialM21, float initialM22,
                             float initialM23, float initialM24,
                             float initialM31, float initialM32,
                             float initialM33, float initialM34,
                             float initialM41, float initialM42,
                             float initialM43, float initialM44, Matrix& result)
{
  result.m[0]  = initialM11;
  result.m[1]  = initialM12;
  result.m[2]  = initialM13;
  result.m[3]  = initialM14;
  result.m[4]  = initialM21;
  result.m[5]  = initialM22;
  result.m[6]  = initialM23;
  result.m[7]  = initialM24;
  result.m[8]  = initialM31;
  result.m[9]  = initialM32;
  result.m[10] = initialM33;
  result.m[11] = initialM34;
  result.m[12] = initialM41;
  result.m[13] = initialM42;
  result.m[14] = initialM43;
  result.m[15] = initialM44;
}

Vector4 Matrix::getRow(unsigned int index) const
{
  if (index > 3) {
    return Vector4(-1.f, -1.f, -1.f, -1.f);
  }

  unsigned int i = index * 4;
  return Vector4(m[i + 0], m[i + 1], m[i + 2], m[i + 3]);
}

Matrix& Matrix::setRow(unsigned int index, const Vector4& row)
{
  if (index > 3) {
    return *this;
  }

  unsigned int i = index * 4;
  m[i + 0]       = row.x;
  m[i + 1]       = row.y;
  m[i + 2]       = row.z;
  m[i + 3]       = row.w;

  return *this;
}

Matrix Matrix::FromValues(float initialM11, float initialM12, float initialM13,
                          float initialM14, float initialM21, float initialM22,
                          float initialM23, float initialM24, float initialM31,
                          float initialM32, float initialM33, float initialM34,
                          float initialM41, float initialM42, float initialM43,
                          float initialM44)
{
  Matrix result;

  result.m[0]  = initialM11;
  result.m[1]  = initialM12;
  result.m[2]  = initialM13;
  result.m[3]  = initialM14;
  result.m[4]  = initialM21;
  result.m[5]  = initialM22;
  result.m[6]  = initialM23;
  result.m[7]  = initialM24;
  result.m[8]  = initialM31;
  result.m[9]  = initialM32;
  result.m[10] = initialM33;
  result.m[11] = initialM34;
  result.m[12] = initialM41;
  result.m[13] = initialM42;
  result.m[14] = initialM43;
  result.m[15] = initialM44;

  return result;
}

Matrix Matrix::Compose(const Vector3& scale, Quaternion& rotation,
                       const Vector3& translation)
{
  Matrix result = Matrix::FromValues(scale.x, 0.f, 0.f, 0.f, //
                                     0.f, scale.y, 0.f, 0.f, //
                                     0.f, 0.f, scale.z, 0.f, //
                                     0.f, 0.f, 0.f, 1.f);

  Matrix rotationMatrix = Matrix::Identity();
  rotation.toRotationMatrix(rotationMatrix);
  result = result.multiply(rotationMatrix);

  result.setTranslation(translation);

  return result;
}

Matrix Matrix::Identity()
{
  return Matrix::FromValues(1.f, 0.f, 0.f, 0.f, //
                            0.f, 1.f, 0.f, 0.f, //
                            0.f, 0.f, 1.f, 0.f, //
                            0.f, 0.f, 0.f, 1.f);
}

void Matrix::IdentityToRef(Matrix& result)
{
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, 1.f, 0.f, 0.f, //
                          0.f, 0.f, 1.f, 0.f, //
                          0.f, 0.f, 0.f, 1.f, result);
}

Matrix Matrix::Zero()
{
  return Matrix::FromValues(0.f, 0.f, 0.f, 0.f, //
                            0.f, 0.f, 0.f, 0.f, //
                            0.f, 0.f, 0.f, 0.f, //
                            0.f, 0.f, 0.f, 0.f);
}

Matrix Matrix::RotationX(float angle)
{
  Matrix result;

  Matrix::RotationXToRef(angle, result);

  return result;
}

Matrix Matrix::Invert(Matrix& source)
{
  Matrix result;

  source.invertToRef(result);

  return result;
}

void Matrix::RotationXToRef(float angle, Matrix& result)
{
  float s = std::sin(angle);
  float c = std::cos(angle);

  result.m[0]  = 1.f;
  result.m[15] = 1.f;

  result.m[5]  = c;
  result.m[10] = c;
  result.m[9]  = -s;
  result.m[6]  = s;

  result.m[1]  = 0.f;
  result.m[2]  = 0.f;
  result.m[3]  = 0.f;
  result.m[4]  = 0.f;
  result.m[7]  = 0.f;
  result.m[8]  = 0.f;
  result.m[11] = 0.f;
  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;
}

Matrix Matrix::RotationY(float angle)
{
  Matrix result;

  Matrix::RotationYToRef(angle, result);

  return result;
}

void Matrix::RotationYToRef(float angle, Matrix& result)
{
  float s = std::sin(angle);
  float c = std::cos(angle);

  result.m[5]  = 1.f;
  result.m[15] = 1.f;

  result.m[0]  = c;
  result.m[2]  = -s;
  result.m[8]  = s;
  result.m[10] = c;

  result.m[1]  = 0.f;
  result.m[3]  = 0.f;
  result.m[4]  = 0.f;
  result.m[6]  = 0.f;
  result.m[7]  = 0.f;
  result.m[9]  = 0.f;
  result.m[11] = 0;
  result.m[12] = 0;
  result.m[13] = 0;
  result.m[14] = 0;
}

Matrix Matrix::RotationZ(float angle)
{
  Matrix result;

  Matrix::RotationZToRef(angle, result);

  return result;
}

void Matrix::RotationZToRef(float angle, Matrix& result)
{
  float s = std::sin(angle);
  float c = std::cos(angle);

  result.m[10] = 1.f;
  result.m[15] = 1.f;

  result.m[0] = c;
  result.m[1] = s;
  result.m[4] = -s;
  result.m[5] = c;

  result.m[2]  = 0.f;
  result.m[3]  = 0.f;
  result.m[6]  = 0.f;
  result.m[7]  = 0.f;
  result.m[8]  = 0.f;
  result.m[9]  = 0.f;
  result.m[11] = 0.f;
  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;
}

Matrix Matrix::RotationAxis(Vector3& axis, float angle)
{
  Matrix result = Matrix::Zero();
  Matrix::RotationAxisToRef(axis, angle, result);
  return result;
}

void Matrix::RotationAxisToRef(Vector3& axis, float angle, Matrix& result)
{
  float s  = std::sin(-angle);
  float c  = std::cos(-angle);
  float c1 = 1.f - c;

  axis.normalize();

  result.m[0] = (axis.x * axis.x) * c1 + c;
  result.m[1] = (axis.x * axis.y) * c1 - (axis.z * s);
  result.m[2] = (axis.x * axis.z) * c1 + (axis.y * s);
  result.m[3] = 0.f;

  result.m[4] = (axis.y * axis.x) * c1 + (axis.z * s);
  result.m[5] = (axis.y * axis.y) * c1 + c;
  result.m[6] = (axis.y * axis.z) * c1 - (axis.x * s);
  result.m[7] = 0.f;

  result.m[8]  = (axis.z * axis.x) * c1 - (axis.y * s);
  result.m[9]  = (axis.z * axis.y) * c1 + (axis.x * s);
  result.m[10] = (axis.z * axis.z) * c1 + c;
  result.m[11] = 0.f;

  result.m[15] = 1.f;
}

Matrix Matrix::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
  Matrix result;

  Matrix::RotationYawPitchRollToRef(yaw, pitch, roll, result);

  return result;
}

void Matrix::RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                       Matrix& result)
{
  Quaternion m_tempQuaternion;
  Quaternion::RotationYawPitchRollToRef(yaw, pitch, roll, m_tempQuaternion);

  m_tempQuaternion.toRotationMatrix(result);
}

Matrix Matrix::Scaling(float x, float y, float z)
{
  Matrix result = Matrix::Zero();

  Matrix::ScalingToRef(x, y, z, result);

  return result;
}

void Matrix::ScalingToRef(float x, float y, float z, Matrix& result)
{
  result.m[0]  = x;
  result.m[1]  = 0.f;
  result.m[2]  = 0.f;
  result.m[3]  = 0.f;
  result.m[4]  = 0.f;
  result.m[5]  = y;
  result.m[6]  = 0.f;
  result.m[7]  = 0.f;
  result.m[8]  = 0.f;
  result.m[9]  = 0.f;
  result.m[10] = z;
  result.m[11] = 0.f;
  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;
  result.m[15] = 1.f;
}

Matrix Matrix::Translation(float x, float y, float z)
{
  Matrix result = Matrix::Identity();

  Matrix::TranslationToRef(x, y, z, result);

  return result;
}

void Matrix::TranslationToRef(float x, float y, float z, Matrix& result)
{
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, 1.f, 0.f, 0.f, //
                          0.f, 0.f, 1.f, 0.f, //
                          x, y, z, 1.f, result);
}

Matrix Matrix::Lerp(const Matrix& startValue, const Matrix& endValue,
                    float gradient)
{
  Matrix result = Matrix::Zero();

  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index]
      = startValue.m[index] * (1.f - gradient) + endValue.m[index] * gradient;
  }

  return result;
}

Matrix Matrix::DecomposeLerp(Matrix& startValue, Matrix& endValue,
                             float gradient)
{
  Vector3 startScale(0.f, 0.f, 0.f);
  Quaternion startRotation;
  Vector3 startTranslation(0.f, 0.f, 0.f);
  startValue.decompose(startScale, startRotation, startTranslation);

  Vector3 endScale(0.f, 0.f, 0.f);
  Quaternion endRotation;
  Vector3 endTranslation(0.f, 0.f, 0.f);
  endValue.decompose(endScale, endRotation, endTranslation);

  Vector3 resultScale = Vector3::Lerp(startScale, endScale, gradient);
  Quaternion resultRotation
    = Quaternion::Slerp(startRotation, endRotation, gradient);
  Vector3 resultTranslation
    = Vector3::Lerp(startTranslation, endTranslation, gradient);

  return Matrix::Compose(resultScale, resultRotation, resultTranslation);
}

Matrix Matrix::LookAtLH(const Vector3& eye, Vector3& target, const Vector3& up)
{
  Matrix result = Matrix::Zero();

  Matrix::LookAtLHToRef(eye, target, up, result);

  return result;
}

void Matrix::LookAtLHToRef(const Vector3& eye, const Vector3& target,
                           const Vector3& up, Matrix& result)
{
  Vector3 _xAxis = Vector3::Zero();
  Vector3 _yAxis = Vector3::Zero();
  Vector3 _zAxis = Vector3::Zero();

  // Z axis
  target.subtractToRef(eye, _zAxis);
  _zAxis.normalize();

  // X axis
  Vector3::CrossToRef(up, _zAxis, _xAxis);

  if (std_util::almost_equal(_xAxis.lengthSquared(), 0.f)) {
    _xAxis.x = 1.f;
  }
  else {
    _xAxis.normalize();
  }

  // Y axis
  Vector3::CrossToRef(_zAxis, _xAxis, _yAxis);
  _yAxis.normalize();

  // Eye angles
  float ex = -Vector3::Dot(_xAxis, eye);
  float ey = -Vector3::Dot(_yAxis, eye);
  float ez = -Vector3::Dot(_zAxis, eye);

  return Matrix::FromValuesToRef(_xAxis.x, _yAxis.x, _zAxis.x, 0.f, //
                                 _xAxis.y, _yAxis.y, _zAxis.y, 0.f, //
                                 _xAxis.z, _yAxis.z, _zAxis.z, 0.f, //
                                 ex, ey, ez, 1.f, result);
}

Matrix Matrix::LookAtRH(const Vector3& eye, Vector3& target, const Vector3& up)
{
  Matrix result = Matrix::Zero();

  Matrix::LookAtRHToRef(eye, target, up, result);

  return result;
}

void Matrix::LookAtRHToRef(const Vector3& eye, const Vector3& target,
                           const Vector3& up, Matrix& result)
{
  Vector3 _xAxis = Vector3::Zero();
  Vector3 _yAxis = Vector3::Zero();
  Vector3 _zAxis = Vector3::Zero();

  // Z axis
  eye.subtractToRef(target, _zAxis);
  _zAxis.normalize();

  // X axis
  Vector3::CrossToRef(up, _zAxis, _xAxis);

  if (std_util::almost_equal(_xAxis.lengthSquared(), 0.f)) {
    _xAxis.x = 1.f;
  }
  else {
    _xAxis.normalize();
  }

  // Y axis
  Vector3::CrossToRef(_zAxis, _xAxis, _yAxis);
  _yAxis.normalize();

  // Eye angles
  float ex = -Vector3::Dot(_xAxis, eye);
  float ey = -Vector3::Dot(_yAxis, eye);
  float ez = -Vector3::Dot(_zAxis, eye);

  return Matrix::FromValuesToRef(_xAxis.x, _yAxis.x, _zAxis.x, 0, _xAxis.y,
                                 _yAxis.y, _zAxis.y, 0.f, _xAxis.z, _yAxis.z,
                                 _zAxis.z, 0.f, ex, ey, ez, 1.f, result);
}

Matrix Matrix::OrthoLH(float width, float height, float znear, float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::OrthoLHToRef(width, height, znear, zfar, matrix);

  return matrix;
}

void Matrix::OrthoLHToRef(float width, float height, float znear, float zfar,
                          Matrix& result)
{
  float hw  = 2.f / width;
  float hh  = 2.f / height;
  float id  = 1.f / (zfar - znear);
  float nid = znear / (znear - zfar);

  Matrix::FromValuesToRef(hw, 0.f, 0.f, 0.f, //
                          0.f, hh, 0.f, 0.f, //
                          0.f, 0.f, id, 0.f, //
                          0.f, 0.f, nid, 1.f, result);
}

Matrix Matrix::OrthoOffCenterLH(float left, float right, float bottom,
                                float top, float znear, float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::OrthoOffCenterLHToRef(left, right, bottom, top, znear, zfar, matrix);

  return matrix;
}

void Matrix::OrthoOffCenterLHToRef(float left, float right, float bottom,
                                   float top, float znear, float zfar,
                                   Matrix& result)
{
  result.m[0] = 2.f / (right - left);
  result.m[1] = result.m[2] = result.m[3] = 0.f;
  result.m[5]                             = 2.f / (top - bottom);
  result.m[4] = result.m[6] = result.m[7] = 0.f;

  result.m[10] = 1.f / (zfar - znear);
  result.m[8] = result.m[9] = result.m[11] = 0.f;
  result.m[12]                             = (left + right) / (left - right);
  result.m[13]                             = (top + bottom) / (bottom - top);

  result.m[14] = -znear / (zfar - znear);
  result.m[15] = 1.f;
}

Matrix Matrix::OrthoOffCenterRH(float left, float right, float bottom,
                                float top, float znear, float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::OrthoOffCenterRHToRef(left, right, bottom, top, znear, zfar, matrix);

  return matrix;
}

void Matrix::OrthoOffCenterRHToRef(float left, float right, float bottom,
                                   float top, float znear, float zfar,
                                   Matrix& result)
{
  Matrix::OrthoOffCenterLHToRef(left, right, bottom, top, znear, zfar, result);
  result.m[10] *= -1.f;
}

Matrix Matrix::PerspectiveLH(float width, float height, float znear, float zfar)
{
  Matrix matrix = Matrix::Zero();

  matrix.m[0] = (2.f * znear) / width;
  matrix.m[1] = matrix.m[2] = matrix.m[3] = 0.f;
  matrix.m[5]                             = (2.f * znear) / height;
  matrix.m[4] = matrix.m[6] = matrix.m[7] = 0.f;
  matrix.m[10]                            = -zfar / (znear - zfar);
  matrix.m[8] = matrix.m[9] = 0.f;
  matrix.m[11]              = 1.f;
  matrix.m[12] = matrix.m[13] = matrix.m[15] = 0.f;
  matrix.m[14]                               = (znear * zfar) / (znear - zfar);

  return matrix;
}

Matrix Matrix::PerspectiveFovLH(float fov, float aspect, float znear,
                                float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::PerspectiveFovLHToRef(fov, aspect, znear, zfar, matrix);

  return matrix;
}

void Matrix::PerspectiveFovLHToRef(float fov, float aspect, float znear,
                                   float zfar, Matrix& result,
                                   bool isVerticalFovFixed)
{
  float _tan = 1.f / (std::tan(fov * 0.5f));

  if (isVerticalFovFixed) {
    result.m[0] = _tan / aspect;
  }
  else {
    result.m[0] = _tan;
  }

  result.m[1] = result.m[2] = result.m[3] = 0.f;

  if (isVerticalFovFixed) {
    result.m[5] = _tan;
  }
  else {
    result.m[5] = _tan * aspect;
  }

  result.m[4] = result.m[6] = result.m[7] = 0.f;
  result.m[8] = result.m[9] = 0.0;
  result.m[10]              = zfar / (zfar - znear);
  result.m[11]              = 1.f;
  result.m[12] = result.m[13] = result.m[15] = 0.f;
  result.m[14]                               = -(znear * zfar) / (zfar - znear);
}

Matrix Matrix::PerspectiveFovRH(float fov, float aspect, float znear,
                                float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::PerspectiveFovRHToRef(fov, aspect, znear, zfar, matrix);

  return matrix;
}

void Matrix::PerspectiveFovRHToRef(float fov, float aspect, float znear,
                                   float zfar, Matrix& result,
                                   bool isVerticalFovFixed)
{
  float _tan = 1.f / (std::tan(fov * 0.5f));

  if (isVerticalFovFixed) {
    result.m[0] = _tan / aspect;
  }
  else {
    result.m[0] = _tan;
  }

  result.m[1] = result.m[2] = result.m[3] = 0.0;

  if (isVerticalFovFixed) {
    result.m[5] = _tan;
  }
  else {
    result.m[5] = _tan * aspect;
  }

  result.m[4] = result.m[6] = result.m[7] = 0.f;
  result.m[8] = result.m[9] = 0.f;
  result.m[10]              = zfar / (znear - zfar);
  result.m[11]              = -1.f;
  result.m[12] = result.m[13] = result.m[15] = 0.f;
  result.m[14]                               = (znear * zfar) / (znear - zfar);
}

void Matrix::PerspectiveFovWebVRToRef(const VRFov& fov, float znear, float zfar,
                                      Matrix& result,
                                      bool /*isVerticalFovFixed*/)
{
  float upTan    = std::tan(fov.upDegrees * Math::PI / 180.f);
  float downTan  = std::tan(fov.downDegrees * Math::PI / 180.f);
  float leftTan  = std::tan(fov.leftDegrees * Math::PI / 180.f);
  float rightTan = std::tan(fov.rightDegrees * Math::PI / 180.f);
  float xScale   = 2.f / (leftTan + rightTan);
  float yScale   = 2.f / (upTan + downTan);
  result.m[0]    = xScale;
  result.m[1] = result.m[2] = result.m[3] = result.m[4] = 0.f;
  result.m[5]                                           = yScale;
  result.m[6] = result.m[7] = 0.0;
  result.m[8]               = ((leftTan - rightTan) * xScale * 0.5f);
  result.m[9]               = -((upTan - downTan) * yScale * 0.5f);
  result.m[10]              = -(znear + zfar) / (zfar - znear);
  result.m[10]              = -zfar / (znear - zfar);
  result.m[11]              = 1.f;
  result.m[12] = result.m[13] = result.m[15] = 0.f;
  result.m[14] = -(2.f * zfar * znear) / (zfar - znear);
  result.m[14] = (znear * zfar) / (znear - zfar);
}

Matrix Matrix::GetFinalMatrix(const Viewport& viewport, Matrix& world,
                              Matrix& view, Matrix& projection, float zmin,
                              float zmax)
{
  float cw = static_cast<float>(viewport.width);
  float ch = static_cast<float>(viewport.height);
  float cx = static_cast<float>(viewport.x);
  float cy = static_cast<float>(viewport.y);

  Matrix viewportMatrix
    = Matrix::FromValues(cw / 2.f, 0.f, 0.f, 0.f,    //
                         0.f, -ch / 2.f, 0.f, 0.f,   //
                         0.f, 0.f, zmax - zmin, 0.f, //
                         cx + cw / 2.f, ch / 2.f + cy, zmin, 1.f);

  return world.multiply(view).multiply(projection).multiply(viewportMatrix);
}

Float32Array Matrix::GetAsMatrix2x2(const Matrix& matrix)
{
  return {
    matrix.m[0], matrix.m[1], //
    matrix.m[4], matrix.m[5]  //
  };
}

Float32Array Matrix::GetAsMatrix3x3(const Matrix& matrix)
{
  return {
    matrix.m[0], matrix.m[1],
    matrix.m[2], //
    matrix.m[4], matrix.m[5],
    matrix.m[6], //
    matrix.m[8], matrix.m[9],
    matrix.m[10] //
  };
}

Matrix Matrix::Transpose(const Matrix& matrix)
{
  Matrix result;

  result.m[0] = matrix.m[0];
  result.m[1] = matrix.m[4];
  result.m[2] = matrix.m[8];
  result.m[3] = matrix.m[12];

  result.m[4] = matrix.m[1];
  result.m[5] = matrix.m[5];
  result.m[6] = matrix.m[9];
  result.m[7] = matrix.m[13];

  result.m[8]  = matrix.m[2];
  result.m[9]  = matrix.m[6];
  result.m[10] = matrix.m[10];
  result.m[11] = matrix.m[14];

  result.m[12] = matrix.m[3];
  result.m[13] = matrix.m[7];
  result.m[14] = matrix.m[11];
  result.m[15] = matrix.m[15];

  return result;
}

Matrix Matrix::Reflection(Plane& plane)
{
  Matrix matrix;

  Matrix::ReflectionToRef(plane, matrix);

  return matrix;
}

void Matrix::ReflectionToRef(Plane& plane, Matrix& result)
{
  plane.normalize();
  const float x     = plane.normal.x;
  const float y     = plane.normal.y;
  const float z     = plane.normal.z;
  const float temp  = -2.f * x;
  const float temp2 = -2.f * y;
  const float temp3 = -2.f * z;

  result.m[0]  = (temp * x) + 1;
  result.m[1]  = temp2 * x;
  result.m[2]  = temp3 * x;
  result.m[3]  = 0.f;
  result.m[4]  = temp * y;
  result.m[5]  = (temp2 * y) + 1;
  result.m[6]  = temp3 * y;
  result.m[7]  = 0.f;
  result.m[8]  = temp * z;
  result.m[9]  = temp2 * z;
  result.m[10] = (temp3 * z) + 1;
  result.m[11] = 0.f;
  result.m[12] = temp * plane.d;
  result.m[13] = temp2 * plane.d;
  result.m[14] = temp3 * plane.d;
  result.m[15] = 1.f;
}

void Matrix::FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis,
                              const Vector3& zaxis, Matrix& mat)
{
  mat.m[0] = xaxis.x;
  mat.m[1] = xaxis.y;
  mat.m[2] = xaxis.z;

  mat.m[3] = 0.f;

  mat.m[4] = yaxis.x;
  mat.m[5] = yaxis.y;
  mat.m[6] = yaxis.z;

  mat.m[7] = 0.f;

  mat.m[8]  = zaxis.x;
  mat.m[9]  = zaxis.y;
  mat.m[10] = zaxis.z;

  mat.m[11] = 0.f;

  mat.m[12] = 0.f;
  mat.m[13] = 0.f;
  mat.m[14] = 0.f;

  mat.m[15] = 1.f;
}

void FromQuaternionToRef(const Quaternion& quat, Matrix& result)
{
  const float xx = quat.x * quat.x;
  const float yy = quat.y * quat.y;
  const float zz = quat.z * quat.z;
  const float xy = quat.x * quat.y;
  const float zw = quat.z * quat.w;
  const float zx = quat.z * quat.x;
  const float yw = quat.y * quat.w;
  const float yz = quat.y * quat.z;
  const float xw = quat.x * quat.w;

  result.m[0]  = 1.f - (2.f * (yy + zz));
  result.m[1]  = 2.f * (xy + zw);
  result.m[2]  = 2.f * (zx - yw);
  result.m[3]  = 0.f;
  result.m[4]  = 2.f * (xy - zw);
  result.m[5]  = 1.f - (2.f * (zz + xx));
  result.m[6]  = 2.f * (yz + xw);
  result.m[7]  = 0.f;
  result.m[8]  = 2.f * (zx + yw);
  result.m[9]  = 2.f * (yz - xw);
  result.m[10] = 1.f - (2.f * (yy + xx));
  result.m[11] = 0.f;

  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;

  result.m[15] = 1.f;
}

} // end of namespace BABYLON
