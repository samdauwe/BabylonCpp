#include <babylon/math/matrix.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/vr/vr_fov.h>
#include <babylon/math/math_tmp.h>
#include <babylon/math/plane.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/math/viewport.h>

namespace BABYLON {

Quaternion Matrix::_tempQuaternion = Quaternion::Zero();
Vector3 Matrix::_xAxis             = Vector3::Zero();
Vector3 Matrix::_yAxis             = Vector3::Zero();
Vector3 Matrix::_zAxis             = Vector3::Zero();
int Matrix::_updateFlagSeed        = 0;

Matrix::Matrix() : updateFlag{0}, _isIdentity{false}, _isIdentityDirty{true}
{
  ::std::fill(m.begin(), m.end(), 0.f);
  _markAsUpdated();
}

Matrix::Matrix(const Matrix& otherMatrix) : m{otherMatrix.m}
{
}

Matrix::Matrix(Matrix&& otherMatrix) : m{::std::move(otherMatrix.m)}
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
    m = ::std::move(otherMatrix.m);
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

unique_ptr_t<Matrix> Matrix::clone() const
{
  return ::std::make_unique<Matrix>(*this);
}

const char* Matrix::getClassName() const
{
  return "Matrix";
}

int Matrix::getHashCode() const
{
  float hash = m[0];
  for (unsigned int i = 1; i < 16; ++i) {
    hash = ::std::pow((hash * 397), m[i]);
  }
  return static_cast<int>(hash);
}

void Matrix::_markAsUpdated()
{
#if 0
  updateFlag = (Matrix::_updateFlagSeed < numeric_limits_t<int>::max()) ?
                 Matrix::_updateFlagSeed++ :
                 0;
  _isIdentityDirty = true;
#endif
}

/** Properties **/
bool Matrix::isIdentity(bool considerAsTextureMatrix)
{
  if (_isIdentityDirty) {
    _isIdentityDirty = false;
    if (m[0] != 1.f || m[5] != 1.f || m[15] != 1.f) {
      _isIdentity = false;
    }
    else if (m[1] != 0.f || m[2] != 0.f || m[3] != 0.f || m[4] != 0.f
             || m[6] != 0.f || m[7] != 0.f || m[8] != 0.f || m[9] != 0.f
             || m[11] != 0.f || m[12] != 0.f || m[13] != 0.f || m[14] != 0.f) {
      _isIdentity = false;
    }
    else {
      _isIdentity = true;
    }

    if (!considerAsTextureMatrix && m[10] != 1.f) {
      _isIdentity = false;
    }
  }

  return _isIdentity;
}

float Matrix::determinant() const
{
  const float temp1 = (m[10] * m[15]) - (m[11] * m[14]);
  const float temp2 = (m[9] * m[15]) - (m[11] * m[13]);
  const float temp3 = (m[9] * m[14]) - (m[10] * m[13]);
  const float temp4 = (m[8] * m[15]) - (m[11] * m[12]);
  const float temp5 = (m[8] * m[14]) - (m[10] * m[12]);
  const float temp6 = (m[8] * m[13]) - (m[9] * m[12]);

  return ((((m[0] * (((m[5] * temp1) - (m[6] * temp2)) + (m[7] * temp3)))
            - (m[1] * (((m[4] * temp1) - (m[6] * temp4)) + (m[7] * temp5))))
           + (m[2] * (((m[4] * temp2) - (m[5] * temp4)) + (m[7] * temp6))))
          - (m[3] * (((m[4] * temp3) - (m[5] * temp5)) + (m[6] * temp6))));
}

/** Methods **/
Float32Array Matrix::toArray() const
{
  Float32Array v(m.size());
  ::std::copy(m.begin(), m.end(), v.begin());

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
  ::std::fill(m.begin(), m.end(), 0.f);

  _markAsUpdated();

  return *this;
}

Matrix Matrix::subtract(const Matrix& other) const
{
  Matrix result;

  subtractFromRef(other, result);

  return result;
}

const Matrix& Matrix::subtractFromRef(const Matrix& other, Matrix& result) const
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

  _markAsUpdated();
  return *this;
}

Matrix& Matrix::addToSelf(const Matrix& other)
{
  for (unsigned int index = 0; index < 16; ++index) {
    m[index] += other.m[index];
  }

  _markAsUpdated();
  return *this;
}

Matrix& Matrix::invertToRef(Matrix& other)
{
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  simdMatrix.invertToRefSIMD(other);
#else
  const float l1  = m[0];
  const float l2  = m[1];
  const float l3  = m[2];
  const float l4  = m[3];
  const float l5  = m[4];
  const float l6  = m[5];
  const float l7  = m[6];
  const float l8  = m[7];
  const float l9  = m[8];
  const float l10 = m[9];
  const float l11 = m[10];
  const float l12 = m[11];
  const float l13 = m[12];
  const float l14 = m[13];
  const float l15 = m[14];
  const float l16 = m[15];
  const float l17 = (l11 * l16) - (l12 * l15);
  const float l18 = (l10 * l16) - (l12 * l14);
  const float l19 = (l10 * l15) - (l11 * l14);
  const float l20 = (l9 * l16) - (l12 * l13);
  const float l21 = (l9 * l15) - (l11 * l13);
  const float l22 = (l9 * l14) - (l10 * l13);
  const float l23 = ((l6 * l17) - (l7 * l18)) + (l8 * l19);
  const float l24 = -(((l5 * l17) - (l7 * l20)) + (l8 * l21));
  const float l25 = ((l5 * l18) - (l6 * l20)) + (l8 * l22);
  const float l26 = -(((l5 * l19) - (l6 * l21)) + (l7 * l22));
  const float l27
    = 1.f / ((((l1 * l23) + (l2 * l24)) + (l3 * l25)) + (l4 * l26));
  const float l28 = (l7 * l16) - (l8 * l15);
  const float l29 = (l6 * l16) - (l8 * l14);
  const float l30 = (l6 * l15) - (l7 * l14);
  const float l31 = (l5 * l16) - (l8 * l13);
  const float l32 = (l5 * l15) - (l7 * l13);
  const float l33 = (l5 * l14) - (l6 * l13);
  const float l34 = (l7 * l12) - (l8 * l11);
  const float l35 = (l6 * l12) - (l8 * l10);
  const float l36 = (l6 * l11) - (l7 * l10);
  const float l37 = (l5 * l12) - (l8 * l9);
  const float l38 = (l5 * l11) - (l7 * l9);
  const float l39 = (l5 * l10) - (l6 * l9);

  other.m[0]       = l23 * l27;
  other.m[4]       = l24 * l27;
  other.m[8]       = l25 * l27;
  other.m[12]      = l26 * l27;
  other.m[1]       = -(((l2 * l17) - (l3 * l18)) + (l4 * l19)) * l27;
  other.m[5]       = (((l1 * l17) - (l3 * l20)) + (l4 * l21)) * l27;
  other.m[9]       = -(((l1 * l18) - (l2 * l20)) + (l4 * l22)) * l27;
  other.m[13]      = (((l1 * l19) - (l2 * l21)) + (l3 * l22)) * l27;
  other.m[2]       = (((l2 * l28) - (l3 * l29)) + (l4 * l30)) * l27;
  other.m[6]       = -(((l1 * l28) - (l3 * l31)) + (l4 * l32)) * l27;
  other.m[10]      = (((l1 * l29) - (l2 * l31)) + (l4 * l33)) * l27;
  other.m[14]      = -(((l1 * l30) - (l2 * l32)) + (l3 * l33)) * l27;
  other.m[3]       = -(((l2 * l34) - (l3 * l35)) + (l4 * l36)) * l27;
  other.m[7]       = (((l1 * l34) - (l3 * l37)) + (l4 * l38)) * l27;
  other.m[11]      = -(((l1 * l35) - (l2 * l37)) + (l4 * l39)) * l27;
  other.m[15]      = (((l1 * l36) - (l2 * l38)) + (l3 * l39)) * l27;
#endif

  _markAsUpdated();
  return *this;
}

Matrix& Matrix::setTranslationFromFloats(float x, float y, float z)
{
  m[12] = x;
  m[13] = y;
  m[14] = z;

  _markAsUpdated();
  return *this;
}

Matrix& Matrix::setTranslation(const Vector3& vector3)
{
  m[12] = vector3.x;
  m[13] = vector3.y;
  m[14] = vector3.z;

  _markAsUpdated();
  return *this;
}

Vector3 Matrix::getTranslation() const
{
  return Vector3(m[12], m[13], m[14]);
}

const Matrix& Matrix::getTranslationToRef(Vector3& result) const
{
  result.x = m[12];
  result.y = m[13];
  result.z = m[14];

  return *this;
}

Matrix& Matrix::removeRotationAndScaling()
{
  setRowFromFloats(0, 1.f, 0.f, 0.f, 0.f);
  setRowFromFloats(1, 0.f, 1.f, 0.f, 0.f);
  setRowFromFloats(2, 0.f, 0.f, 1.f, 0.f);

  return *this;
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

  _markAsUpdated();
  return *this;
}

const Matrix& Matrix::copyToArray(array_t<float, 16>& array,
                                  unsigned int offset) const
{
  if (offset != 0) {
    return *this;
  }

  for (unsigned int index = 0; index != 16; ++index) {
    array[offset + index] = m[index];
  }

  return *this;
}

const Matrix& Matrix::copyToArray(Float32Array& array,
                                  unsigned int offset) const
{
  if (array.size() < 16 + offset) {
    return *this;
  }

  for (unsigned int index = 0; index != 16; ++index) {
    array[offset + index] = m[index];
  }

  return *this;
}

Matrix& Matrix::multiplyToRef(const Matrix& other, Matrix& result)
{
  multiplyToArray(other, result.m, 0);

  _markAsUpdated();
  return *this;
}

const Matrix& Matrix::multiplyToArray(const Matrix& other,
                                      array_t<float, 16>& result,
                                      unsigned int offset) const
{
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  simdMatrix.multiplyToArraySIMD(other, result, offset);
#else
  const float tm0  = m[0];
  const float tm1  = m[1];
  const float tm2  = m[2];
  const float tm3  = m[3];
  const float tm4  = m[4];
  const float tm5  = m[5];
  const float tm6  = m[6];
  const float tm7  = m[7];
  const float tm8  = m[8];
  const float tm9  = m[9];
  const float tm10 = m[10];
  const float tm11 = m[11];
  const float tm12 = m[12];
  const float tm13 = m[13];
  const float tm14 = m[14];
  const float tm15 = m[15];

  const float om0  = other.m[0];
  const float om1  = other.m[1];
  const float om2  = other.m[2];
  const float om3  = other.m[3];
  const float om4  = other.m[4];
  const float om5  = other.m[5];
  const float om6  = other.m[6];
  const float om7  = other.m[7];
  const float om8  = other.m[8];
  const float om9  = other.m[9];
  const float om10 = other.m[10];
  const float om11 = other.m[11];
  const float om12 = other.m[12];
  const float om13 = other.m[13];
  const float om14 = other.m[14];
  const float om15 = other.m[15];

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
#endif

  return *this;
}

const Matrix& Matrix::multiplyToArray(const Matrix& other, Float32Array& result,
                                      unsigned int offset) const
{
  if (result.size() < 16 + offset) {
    return *this;
  }

  array_t<float, 16> array;
  multiplyToArray(other, array, offset);
  for (unsigned int i = 0; i != 16; ++i) {
    result[i] = array[i];
  }

  return *this;
}

bool Matrix::equals(const Matrix& other) const
{
  return (stl_util::almost_equal(m[0], other.m[0])
          && stl_util::almost_equal(m[1], other.m[1])
          && stl_util::almost_equal(m[2], other.m[2])
          && stl_util::almost_equal(m[3], other.m[3])
          && stl_util::almost_equal(m[4], other.m[4])
          && stl_util::almost_equal(m[5], other.m[5])
          && stl_util::almost_equal(m[6], other.m[6])
          && stl_util::almost_equal(m[7], other.m[7])
          && stl_util::almost_equal(m[8], other.m[8])
          && stl_util::almost_equal(m[9], other.m[9])
          && stl_util::almost_equal(m[10], other.m[10])
          && stl_util::almost_equal(m[11], other.m[11])
          && stl_util::almost_equal(m[12], other.m[12])
          && stl_util::almost_equal(m[13], other.m[13])
          && stl_util::almost_equal(m[14], other.m[14])
          && stl_util::almost_equal(m[15], other.m[15]));
}

bool Matrix::decompose(Vector3& scale, Quaternion& rotation,
                       Vector3& translation) const
{
  translation.x = m[12];
  translation.y = m[13];
  translation.z = m[14];

  scale.x = ::std::sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
  scale.y = ::std::sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
  scale.z = ::std::sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

  if (determinant() <= 0.f) {
    scale.y *= -1.f;
  }

  if (stl_util::almost_equal(scale.x, 0.f)
      || stl_util::almost_equal(scale.y, 0.f)
      || stl_util::almost_equal(scale.z, 0.f)) {
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
    0.f, 0.f, 0.f, 1.f, MathTmp::MatrixArray[0]);

  Quaternion::FromRotationMatrixToRef(MathTmp::MatrixArray[0], rotation);

  return true;
}

Matrix Matrix::getRotationMatrix() const
{
  Matrix result = Matrix::Identity();

  getRotationMatrixToRef(result);

  return result;
}

const Matrix& Matrix::getRotationMatrixToRef(Matrix& result) const
{
  const float xs = m[0] * m[1] * m[2] * m[3] < 0 ? -1.f : 1.f;
  const float ys = m[4] * m[5] * m[6] * m[7] < 0 ? -1.f : 1.f;
  const float zs = m[8] * m[9] * m[10] * m[11] < 0 ? -1.f : 1.f;

  const float sx = xs * ::std::sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
  const float sy = ys * ::std::sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
  const float sz = zs * ::std::sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

  Matrix::FromValuesToRef(m[0] / sx, m[1] / sx, m[2] / sx, 0.f,  //
                          m[4] / sy, m[5] / sy, m[6] / sy, 0.f,  //
                          m[8] / sz, m[9] / sz, m[10] / sz, 0.f, //
                          0.f, 0.f, 0.f, 1.f, result);

  return *this;
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Matrix& matrix)
{
  os << "{\"M\":[";
  for (unsigned int i = 0; i < 15; ++i) {
    os << matrix.m[i] << ",";
  }
  os << matrix.m[15] << "}";
  return os;
}

Matrix Matrix::operator+(const Matrix& other)
{
  return add(other);
}

Matrix& Matrix::operator+=(const Matrix& other)
{
  return addToSelf(other);
}

Matrix Matrix::operator-(const Matrix& other) const
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
  return !(operator==(other));
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
  result._markAsUpdated();
}

void Matrix::FromFloat32ArrayToRefScaled(const Float32Array& array,
                                         unsigned int offset, float scale,
                                         Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result.m[index] = array[index + offset] * scale;
  }
  result._markAsUpdated();
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

  result._markAsUpdated();
}

Vector4 Matrix::getRow(unsigned int index) const
{
  if (index > 3) {
    return Vector4(-1.f, -1.f, -1.f, -1.f);
  }

  const unsigned int i = index * 4;
  return Vector4(m[i + 0], m[i + 1], m[i + 2], m[i + 3]);
}

Matrix& Matrix::setRow(unsigned int index, const Vector4& row)
{
  if (index > 3) {
    return *this;
  }

  const auto i = index * 4;
  m[i + 0]     = row.x;
  m[i + 1]     = row.y;
  m[i + 2]     = row.z;
  m[i + 3]     = row.w;

  _markAsUpdated();

  return *this;
}

Matrix& Matrix::setRowFromFloats(unsigned int index, float x, float y, float z,
                                 float w)
{
  if (index > 3) {
    return *this;
  }

  const auto i = index * 4;
  m[i + 0]     = x;
  m[i + 1]     = y;
  m[i + 2]     = z;
  m[i + 3]     = w;

  _markAsUpdated();

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
  auto result = Matrix::Identity();
  Matrix::ComposeToRef(scale, rotation, translation, result);
  return result;
}

void Matrix::ComposeToRef(const Vector3& scale, Quaternion& rotation,
                          const Vector3& translation, Matrix& result)
{
  Matrix::FromValuesToRef(scale.x, 0.f, 0.f, 0.f, //
                          0.f, scale.y, 0.f, 0.f, //
                          0.f, 0.f, scale.z, 0.f, //
                          0.f, 0.f, 0.f, 1.f, MathTmp::MatrixArray[1]);

  rotation.toRotationMatrix(MathTmp::MatrixArray[0]);

  MathTmp::MatrixArray[1].multiplyToRef(MathTmp::MatrixArray[0], result);

  result.setTranslation(translation);
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
  const float s = ::std::sin(angle);
  const float c = ::std::cos(angle);

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

  result._markAsUpdated();
}

Matrix Matrix::RotationY(float angle)
{
  Matrix result;

  Matrix::RotationYToRef(angle, result);

  return result;
}

void Matrix::RotationYToRef(float angle, Matrix& result)
{
  const float s = ::std::sin(angle);
  const float c = ::std::cos(angle);

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
  result.m[11] = 0.f;
  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;

  result._markAsUpdated();
}

Matrix Matrix::RotationZ(float angle)
{
  Matrix result;

  Matrix::RotationZToRef(angle, result);

  return result;
}

void Matrix::RotationZToRef(float angle, Matrix& result)
{
  const float s = ::std::sin(angle);
  const float c = ::std::cos(angle);

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

  result._markAsUpdated();
}

Matrix Matrix::RotationAxis(Vector3& axis, float angle)
{
  Matrix result = Matrix::Zero();
  Matrix::RotationAxisToRef(axis, angle, result);
  return result;
}

void Matrix::RotationAxisToRef(Vector3& axis, float angle, Matrix& result)
{
  const float s  = ::std::sin(-angle);
  const float c  = ::std::cos(-angle);
  const float c1 = 1.f - c;

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

  result._markAsUpdated();
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

  result._markAsUpdated();
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

  result._markAsUpdated();

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
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  SIMD::SIMDMatrix::LookAtLHToRefSIMD(eye, target, up, result);
#else
  Vector3 _xAxis      = Vector3::Zero();
  Vector3 _yAxis      = Vector3::Zero();
  Vector3 _zAxis      = Vector3::Zero();

  // Z axis
  target.subtractToRef(eye, _zAxis);
  _zAxis.normalize();

  // X axis
  Vector3::CrossToRef(up, _zAxis, _xAxis);

  if (stl_util::almost_equal(_xAxis.lengthSquared(), 0.f)) {
    _xAxis.x = 1.f;
  }
  else {
    _xAxis.normalize();
  }

  // Y axis
  Vector3::CrossToRef(_zAxis, _xAxis, _yAxis);
  _yAxis.normalize();

  // Eye angles
  const float ex = -Vector3::Dot(_xAxis, eye);
  const float ey = -Vector3::Dot(_yAxis, eye);
  const float ez = -Vector3::Dot(_zAxis, eye);

  return Matrix::FromValuesToRef(_xAxis.x, _yAxis.x, _zAxis.x, 0.f, //
                                 _xAxis.y, _yAxis.y, _zAxis.y, 0.f, //
                                 _xAxis.z, _yAxis.z, _zAxis.z, 0.f, //
                                 ex, ey, ez, 1.f, result);
#endif
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

  if (stl_util::almost_equal(_xAxis.lengthSquared(), 0.f)) {
    _xAxis.x = 1.f;
  }
  else {
    _xAxis.normalize();
  }

  // Y axis
  Vector3::CrossToRef(_zAxis, _xAxis, _yAxis);
  _yAxis.normalize();

  // Eye angles
  const float ex = -Vector3::Dot(_xAxis, eye);
  const float ey = -Vector3::Dot(_yAxis, eye);
  const float ez = -Vector3::Dot(_zAxis, eye);

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
  const float n = znear;
  const float f = zfar;

  const float a = 2.f / width;
  const float b = 2.f / height;
  const float c = 2.f / (f - n);
  const float d = -(f + n) / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 0.f, //
                          0.f, 0.f, d, 1.f, //
                          result);
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
  const float n = znear;
  const float f = zfar;

  const float a = 2.f / (right - left);
  const float b = 2.f / (top - bottom);
  const float c = 2.f / (f - n);
  const float d = -(f + n) / (f - n);

  const float i0 = (left + right) / (left - right);
  const float i1 = (top + bottom) / (bottom - top);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 0.f, //
                          i0, i1, d, 1.f,   //
                          result);
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
  auto matrix = Matrix::Zero();

  const float n = znear;
  const float f = zfar;

  const float a = 2.f * n / width;
  const float b = 2.f * n / height;
  const float c = (f + n) / (f - n);
  const float d = -2.f * f * n / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 1.f, //
                          0.f, 0.f, d, 0.f, //
                          matrix);
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
  const float n = znear;
  const float f = zfar;

  const float t = 1.f / (::std::tan(fov * 0.5f));
  const float a = isVerticalFovFixed ? (t / aspect) : t;
  const float b = isVerticalFovFixed ? t : (t * aspect);
  const float c = (f + n) / (f - n);
  const float d = -2.f * f * n / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 1.f, //
                          0.f, 0.f, d, 0.f, //
                          result);
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
  // alternatively this could be expressed as:
  //    m = PerspectiveFovLHToRef
  //    m[10] *= -1.0;
  //    m[11] *= -1.0;

  const float n = znear;
  const float f = zfar;

  const float t = 1.f / (::std::tan(fov * 0.5f));
  const float a = isVerticalFovFixed ? (t / aspect) : t;
  const float b = isVerticalFovFixed ? t : (t * aspect);
  const float c = -(f + n) / (f - n);
  const float d = -2.f * f * n / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f,  //
                          0.f, b, 0.f, 0.f,  //
                          0.f, 0.f, c, -1.f, //
                          0.f, 0.f, d, 0.f,  //
                          result);
}

void Matrix::PerspectiveFovWebVRToRef(const VRFov& fov, float znear, float zfar,
                                      Matrix& result, bool rightHanded)
{
  const float rightHandedFactor = rightHanded ? -1.f : 1.f;
  const float upTan             = ::std::tan(fov.upDegrees * Math::PI / 180.f);
  const float downTan  = ::std::tan(fov.downDegrees * Math::PI / 180.f);
  const float leftTan  = ::std::tan(fov.leftDegrees * Math::PI / 180.f);
  const float rightTan = ::std::tan(fov.rightDegrees * Math::PI / 180.f);
  const float xScale   = 2.f / (leftTan + rightTan);
  const float yScale   = 2.f / (upTan + downTan);
  result.m[0]          = xScale;
  result.m[1] = result.m[2] = result.m[3] = result.m[4] = 0.f;
  result.m[5]                                           = yScale;
  result.m[6] = result.m[7] = 0.0;
  result.m[8] = ((leftTan - rightTan) * xScale * 0.5f); // * rightHandedFactor;
  result.m[9] = -((upTan - downTan) * yScale * 0.5f);   // * rightHandedFactor;
  // result.m[10] = -(znear + zfar) / (zfar - znear) * rightHandedFactor;
  result.m[10] = -zfar / (znear - zfar);
  result.m[11] = 1.f * rightHandedFactor;
  result.m[12] = result.m[13] = result.m[15] = 0.f;
  result.m[14] = -(2.f * zfar * znear) / (zfar - znear);
  // result.m[14] = (znear * zfar) / (znear - zfar);

  result._markAsUpdated();
}

Matrix Matrix::GetFinalMatrix(const Viewport& viewport, Matrix& world,
                              Matrix& view, Matrix& projection, float zmin,
                              float zmax)
{
  const float cw = static_cast<float>(viewport.width);
  const float ch = static_cast<float>(viewport.height);
  const float cx = static_cast<float>(viewport.x);
  const float cy = static_cast<float>(viewport.y);

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

  result._markAsUpdated();
}

void Matrix::FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis,
                              const Vector3& zaxis, Matrix& result)
{
  result.m[0] = xaxis.x;
  result.m[1] = xaxis.y;
  result.m[2] = xaxis.z;

  result.m[3] = 0.f;

  result.m[4] = yaxis.x;
  result.m[5] = yaxis.y;
  result.m[6] = yaxis.z;

  result.m[7] = 0.f;

  result.m[8]  = zaxis.x;
  result.m[9]  = zaxis.y;
  result.m[10] = zaxis.z;

  result.m[11] = 0.f;

  result.m[12] = 0.f;
  result.m[13] = 0.f;
  result.m[14] = 0.f;

  result.m[15] = 1.f;

  result._markAsUpdated();
}

void Matrix::FromQuaternionToRef(const Quaternion& quat, Matrix& result)
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

  result._markAsUpdated();
}

} // end of namespace BABYLON
