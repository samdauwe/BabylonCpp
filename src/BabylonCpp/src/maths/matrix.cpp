#include <babylon/maths/matrix.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/vr/vr_fov.h>
#include <babylon/maths/math_tmp.h>
#include <babylon/maths/plane.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/maths/vector4.h>
#include <babylon/maths/viewport.h>

namespace BABYLON {

int Matrix::_updateFlagSeed      = 0;
Matrix Matrix::_identityReadOnly = Matrix::Identity();

Matrix::Matrix()
    : updateFlag{-1}
    , _isIdentity{false}
    , _isIdentityDirty{true}
    , _isIdentity3x2{true}
    , _isIdentity3x2Dirty{true}
{
  std::fill(_m.begin(), _m.end(), 0.f);
  _updateIdentityStatus(false);
}

Matrix::Matrix(const Matrix& otherMatrix) = default;

Matrix::Matrix(Matrix&& otherMatrix) = default;

Matrix& Matrix::operator=(const Matrix& otherMatrix) = default;

Matrix& Matrix::operator=(Matrix&& otherMatrix) = default;

Matrix::~Matrix() = default;

Matrix Matrix::copy() const
{
  return Matrix(*this);
}

std::unique_ptr<Matrix> Matrix::clone() const
{
  return std::make_unique<Matrix>(*this);
}

const char* Matrix::getClassName() const
{
  return "Matrix";
}

int Matrix::getHashCode() const
{
  auto hash = _m[0];
  for (unsigned int i = 1; i < 16; ++i) {
    hash = std::pow((hash * 397), _m[i]);
  }
  return static_cast<int>(hash);
}

const std::array<float, 16>& Matrix::m() const
{
  return _m;
}

void Matrix::_markAsUpdated()
{
  updateFlag
    = (Matrix::_updateFlagSeed < std::numeric_limits<int>::max()) ? Matrix::_updateFlagSeed++ : 0;
  _isIdentity         = false;
  _isIdentity3x2      = false;
  _isIdentityDirty    = true;
  _isIdentity3x2Dirty = true;
}

void Matrix::_updateIdentityStatus(bool isIdentity, bool isIdentityDirty, bool isIdentity3x2,
                                   bool isIdentity3x2Dirty)
{
  updateFlag          = Matrix::_updateFlagSeed++;
  _isIdentity         = isIdentity;
  _isIdentity3x2      = isIdentity || isIdentity3x2;
  _isIdentityDirty    = _isIdentity ? false : isIdentityDirty;
  _isIdentity3x2Dirty = _isIdentity3x2 ? false : isIdentity3x2Dirty;
}

/** Properties **/
bool Matrix::isIdentity()
{
  if (_isIdentityDirty) {
    _isIdentityDirty = false;
    const auto& m    = _m;
    _isIdentity
      = (m[0] == 1.f && m[1] == 0.f && m[2] == 0.f && m[3] == 0.f && m[4] == 0.f && m[5] == 1.f
         && m[6] == 0.f && m[7] == 0.f && m[8] == 0.f && m[9] == 0.f && m[10] == 1.f && m[11] == 0.f
         && m[12] == 0.f && m[13] == 0.f && m[14] == 0.f && m[15] == 1.f);
  }

  return _isIdentity;
}

bool Matrix::isIdentityAs3x2()
{
  if (_isIdentity3x2Dirty) {
    _isIdentity3x2Dirty = false;
    if (_m[0] != 1.f || _m[5] != 1.f || _m[15] != 1.f) {
      _isIdentity3x2 = false;
    }
    else if (_m[1] != 0.f || _m[2] != 0.f || _m[3] != 0.f || _m[4] != 0.f || _m[6] != 0.f
             || _m[7] != 0.f || _m[8] != 0.f || _m[9] != 0.f || _m[10] != 0.f || _m[11] != 0.f
             || _m[12] != 0.f || _m[13] != 0.f || _m[14] != 0.f) {
      _isIdentity3x2 = false;
    }
    else {
      _isIdentity3x2 = true;
    }
  }

  return _isIdentity3x2;
}

float Matrix::determinant() const
{
  if (_isIdentity) {
    return 1;
  }

  const auto& m  = _m;
  const auto m00 = m[0], m01 = m[1], m02 = m[2], m03 = m[3];
  const auto m10 = m[4], m11 = m[5], m12 = m[6], m13 = m[7];
  const auto m20 = m[8], m21 = m[9], m22 = m[10], m23 = m[11];
  const auto m30 = m[12], m31 = m[13], m32 = m[14], m33 = m[15];
  // https://en.wikipedia.org/wiki/Laplace_expansion
  // to compute the deterrminant of a 4x4 Matrix we compute the cofactors of any
  // row or column, then we multiply each Cofactor by its corresponding matrix
  // value and sum them all to get the determinant Cofactor(i, j) = sign(i,j) *
  // det(Minor(i, j)) where
  //  - sign(i,j) = (i+j) % 2 === 0 ? 1 : -1
  //  - Minor(i, j) is the 3x3 matrix we get by removing row i and column j from
  //  current Matrix
  //
  // Here we do that for the 1st row.

  const auto det_22_33 = m22 * m33 - m32 * m23;
  const auto det_21_33 = m21 * m33 - m31 * m23;
  const auto det_21_32 = m21 * m32 - m31 * m22;
  const auto det_20_33 = m20 * m33 - m30 * m23;
  const auto det_20_32 = m20 * m32 - m22 * m30;
  const auto det_20_31 = m20 * m31 - m30 * m21;
  const auto cofact_00 = +(m11 * det_22_33 - m12 * det_21_33 + m13 * det_21_32);
  const auto cofact_01 = -(m10 * det_22_33 - m12 * det_20_33 + m13 * det_20_32);
  const auto cofact_02 = +(m10 * det_21_33 - m11 * det_20_33 + m13 * det_20_31);
  const auto cofact_03 = -(m10 * det_21_32 - m11 * det_20_32 + m12 * det_20_31);
  return m00 * cofact_00 + m01 * cofact_01 + m02 * cofact_02 + m03 * cofact_03;
}

/** Methods **/
Float32Array Matrix::toArray() const
{
  Float32Array v(_m.size());
  std::copy(_m.begin(), _m.end(), v.begin());

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
  Matrix::FromValuesToRef(0.f, 0.f, 0.f, 0.f, //
                          0.f, 0.f, 0.f, 0.f, //
                          0.f, 0.f, 0.f, 0.f, //
                          0.f, 0.f, 0.f, 0.f, //
                          *this);
  _updateIdentityStatus(false);
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
  const auto& m      = _m;
  auto& resultM      = result._m;
  const auto& otherM = other.m();
  for (unsigned int index = 0; index < 16; ++index) {
    resultM[index] = m[index] - otherM[index];
  }
  result._markAsUpdated();
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
  const auto& m      = _m;
  auto& resultM      = result._m;
  const auto& otherM = other.m();
  for (unsigned int index = 0; index < 16; ++index) {
    resultM[index] = m[index] + otherM[index];
  }
  result._markAsUpdated();
  return *this;
}

Matrix& Matrix::addToSelf(const Matrix& other)
{
  auto& m            = _m;
  const auto& otherM = other.m();
  for (unsigned int index = 0; index < 16; ++index) {
    m[index] += otherM[index];
  }
  _markAsUpdated();
  return *this;
}

const Matrix& Matrix::invertToRef(Matrix& other) const
{
  if (_isIdentity) {
    Matrix::IdentityToRef(other);
    return *this;
  }

  // the inverse of a Matrix is the transpose of cofactor matrix divided by the determinant
  const auto& m  = _m;
  const auto m00 = m[0], m01 = m[1], m02 = m[2], m03 = m[3];
  const auto m10 = m[4], m11 = m[5], m12 = m[6], m13 = m[7];
  const auto m20 = m[8], m21 = m[9], m22 = m[10], m23 = m[11];
  const auto m30 = m[12], m31 = m[13], m32 = m[14], m33 = m[15];

  const auto det_22_33 = m22 * m33 - m32 * m23;
  const auto det_21_33 = m21 * m33 - m31 * m23;
  const auto det_21_32 = m21 * m32 - m31 * m22;
  const auto det_20_33 = m20 * m33 - m30 * m23;
  const auto det_20_32 = m20 * m32 - m22 * m30;
  const auto det_20_31 = m20 * m31 - m30 * m21;

  const auto cofact_00 = +(m11 * det_22_33 - m12 * det_21_33 + m13 * det_21_32);
  const auto cofact_01 = -(m10 * det_22_33 - m12 * det_20_33 + m13 * det_20_32);
  const auto cofact_02 = +(m10 * det_21_33 - m11 * det_20_33 + m13 * det_20_31);
  const auto cofact_03 = -(m10 * det_21_32 - m11 * det_20_32 + m12 * det_20_31);

  const auto det = m00 * cofact_00 + m01 * cofact_01 + m02 * cofact_02 + m03 * cofact_03;

  if (det == 0.f) {
    // not invertible
    other.copyFrom(*this);
    return *this;
  }

  const auto detInv    = 1.f / det;
  const auto det_12_33 = m12 * m33 - m32 * m13;
  const auto det_11_33 = m11 * m33 - m31 * m13;
  const auto det_11_32 = m11 * m32 - m31 * m12;
  const auto det_10_33 = m10 * m33 - m30 * m13;
  const auto det_10_32 = m10 * m32 - m30 * m12;
  const auto det_10_31 = m10 * m31 - m30 * m11;
  const auto det_12_23 = m12 * m23 - m22 * m13;
  const auto det_11_23 = m11 * m23 - m21 * m13;
  const auto det_11_22 = m11 * m22 - m21 * m12;
  const auto det_10_23 = m10 * m23 - m20 * m13;
  const auto det_10_22 = m10 * m22 - m20 * m12;
  const auto det_10_21 = m10 * m21 - m20 * m11;

  const auto cofact_10 = -(m01 * det_22_33 - m02 * det_21_33 + m03 * det_21_32);
  const auto cofact_11 = +(m00 * det_22_33 - m02 * det_20_33 + m03 * det_20_32);
  const auto cofact_12 = -(m00 * det_21_33 - m01 * det_20_33 + m03 * det_20_31);
  const auto cofact_13 = +(m00 * det_21_32 - m01 * det_20_32 + m02 * det_20_31);

  const auto cofact_20 = +(m01 * det_12_33 - m02 * det_11_33 + m03 * det_11_32);
  const auto cofact_21 = -(m00 * det_12_33 - m02 * det_10_33 + m03 * det_10_32);
  const auto cofact_22 = +(m00 * det_11_33 - m01 * det_10_33 + m03 * det_10_31);
  const auto cofact_23 = -(m00 * det_11_32 - m01 * det_10_32 + m02 * det_10_31);

  const auto cofact_30 = -(m01 * det_12_23 - m02 * det_11_23 + m03 * det_11_22);
  const auto cofact_31 = +(m00 * det_12_23 - m02 * det_10_23 + m03 * det_10_22);
  const auto cofact_32 = -(m00 * det_11_23 - m01 * det_10_23 + m03 * det_10_21);
  const auto cofact_33 = +(m00 * det_11_22 - m01 * det_10_22 + m02 * det_10_21);

  Matrix::FromValuesToRef(
    cofact_00 * detInv, cofact_10 * detInv, cofact_20 * detInv, cofact_30 * detInv, //
    cofact_01 * detInv, cofact_11 * detInv, cofact_21 * detInv, cofact_31 * detInv, //
    cofact_02 * detInv, cofact_12 * detInv, cofact_22 * detInv, cofact_32 * detInv, //
    cofact_03 * detInv, cofact_13 * detInv, cofact_23 * detInv, cofact_33 * detInv, //
    other);

  return *this;
}

Matrix& Matrix::addAtIndex(size_t index, float value)
{
  _m[index] += value;
  _markAsUpdated();
  return *this;
}

Matrix& Matrix::multiplyAtIndex(size_t index, float value)
{
  _m[index] *= value;
  _markAsUpdated();
  return *this;
}

Matrix& Matrix::setAtIndex(size_t index, float value)
{
  _m[index] = value;
  _markAsUpdated();
  return *this;
}

Matrix& Matrix::setTranslationFromFloats(float x, float y, float z)
{
  _m[12] = x;
  _m[13] = y;
  _m[14] = z;
  _markAsUpdated();
  return *this;
}

Matrix& Matrix::addTranslationFromFloats(float x, float y, float z)
{
  _m[12] += x;
  _m[13] += y;
  _m[14] += z;
  _markAsUpdated();
  return *this;
}

Matrix& Matrix::setTranslation(const Vector3& vector3)
{
  return setTranslationFromFloats(vector3.x, vector3.y, vector3.z);
}

Vector3 Matrix::getTranslation() const
{
  return Vector3(_m[12], _m[13], _m[14]);
}

const Matrix& Matrix::getTranslationToRef(Vector3& result) const
{
  result.x = _m[12];
  result.y = _m[13];
  result.z = _m[14];

  return *this;
}

Matrix& Matrix::removeRotationAndScaling()
{
  const auto& m = _m;
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, 1.f, 0.f, 0.f, //
                          0.f, 0.f, 1.f, 0.f, //
                          m[12], m[13], m[14], m[15], *this);
  _updateIdentityStatus(m[12] == 0.f && m[13] == 0.f && m[14] == 0.f && m[15] == 1.f);
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
  other.copyToArray(_m);
  const auto& o = other;
  _updateIdentityStatus(o._isIdentity, o._isIdentityDirty, o._isIdentity3x2, o._isIdentity3x2Dirty);
  return *this;
}

const Matrix& Matrix::copyToArray(std::array<float, 16>& array, unsigned int offset) const
{
  if (offset != 0) {
    return *this;
  }

  auto& source       = _m;
  array[offset]      = source[0];
  array[offset + 1]  = source[1];
  array[offset + 2]  = source[2];
  array[offset + 3]  = source[3];
  array[offset + 4]  = source[4];
  array[offset + 5]  = source[5];
  array[offset + 6]  = source[6];
  array[offset + 7]  = source[7];
  array[offset + 8]  = source[8];
  array[offset + 9]  = source[9];
  array[offset + 10] = source[10];
  array[offset + 11] = source[11];
  array[offset + 12] = source[12];
  array[offset + 13] = source[13];
  array[offset + 14] = source[14];
  array[offset + 15] = source[15];

  return *this;
}

const Matrix& Matrix::copyToArray(Float32Array& array, unsigned int offset) const
{
  if (array.size() < 16 + offset) {
    return *this;
  }

  for (unsigned int index = 0; index != 16; ++index) {
    array[offset + index] = _m[index];
  }

  return *this;
}

Matrix& Matrix::multiplyToRef(const Matrix& other, Matrix& result)
{
  if (_isIdentity) {
    result.copyFrom(other);
    return *this;
  }
  if (other._isIdentity) {
    result.copyFrom(*this);
    return *this;
  }

  multiplyToArray(other, result._m, 0);

  result._markAsUpdated();
  return *this;
}

const Matrix& Matrix::multiplyToArray(const Matrix& other, std::array<float, 16>& result,
                                      unsigned int offset) const
{
  const auto& m      = _m;
  const auto& otherM = other.m();
  const auto tm0 = m[0], tm1 = m[1], tm2 = m[2], tm3 = m[3];
  const auto tm4 = m[4], tm5 = m[5], tm6 = m[6], tm7 = m[7];
  const auto tm8 = m[8], tm9 = m[9], tm10 = m[10], tm11 = m[11];
  const auto tm12 = m[12], tm13 = m[13], tm14 = m[14], tm15 = m[15];

  const auto om0 = otherM[0], om1 = otherM[1], om2 = otherM[2], om3 = otherM[3];
  const auto om4 = otherM[4], om5 = otherM[5], om6 = otherM[6], om7 = otherM[7];
  const auto om8 = otherM[8], om9 = otherM[9], om10 = otherM[10], om11 = otherM[11];
  const auto om12 = otherM[12], om13 = otherM[13], om14 = otherM[14], om15 = otherM[15];

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

const Matrix& Matrix::multiplyToArray(const Matrix& other, Float32Array& result,
                                      unsigned int offset) const
{
  if (result.size() < 16 + offset) {
    return *this;
  }

  const auto& m      = _m;
  const auto& otherM = other.m();
  const auto tm0 = m[0], tm1 = m[1], tm2 = m[2], tm3 = m[3];
  const auto tm4 = m[4], tm5 = m[5], tm6 = m[6], tm7 = m[7];
  const auto tm8 = m[8], tm9 = m[9], tm10 = m[10], tm11 = m[11];
  const auto tm12 = m[12], tm13 = m[13], tm14 = m[14], tm15 = m[15];

  const auto om0 = otherM[0], om1 = otherM[1], om2 = otherM[2], om3 = otherM[3];
  const auto om4 = otherM[4], om5 = otherM[5], om6 = otherM[6], om7 = otherM[7];
  const auto om8 = otherM[8], om9 = otherM[9], om10 = otherM[10], om11 = otherM[11];
  const auto om12 = otherM[12], om13 = otherM[13], om14 = otherM[14], om15 = otherM[15];

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

bool Matrix::equals(const Matrix& value) const
{
  const auto& other = value;
  if (_isIdentity || other._isIdentity) {
    if (!_isIdentityDirty && !other._isIdentityDirty) {
      return _isIdentity && other._isIdentity;
    }
  }

  const auto& m  = _m;
  const auto& om = other.m();

  return (stl_util::almost_equal(m[0], om[0]) && stl_util::almost_equal(m[1], om[1])
          && stl_util::almost_equal(m[2], om[2]) && stl_util::almost_equal(m[3], om[3])
          && stl_util::almost_equal(m[4], om[4]) && stl_util::almost_equal(m[5], om[5])
          && stl_util::almost_equal(m[6], om[6]) && stl_util::almost_equal(m[7], om[7])
          && stl_util::almost_equal(m[8], om[8]) && stl_util::almost_equal(m[9], om[9])
          && stl_util::almost_equal(m[10], om[10]) && stl_util::almost_equal(m[11], om[11])
          && stl_util::almost_equal(m[12], om[12]) && stl_util::almost_equal(m[13], om[13])
          && stl_util::almost_equal(m[14], om[14]) && stl_util::almost_equal(m[15], om[15]));
}

bool Matrix::decompose(std::optional<Vector3>& scale, std::optional<Quaternion>& rotation,
                       std::optional<Vector3>& translation) const
{
  if (_isIdentity) {
    if (translation) {
      translation->setAll(0.f);
    }
    if (scale) {
      scale->setAll(1.f);
    }
    if (rotation) {
      rotation->copyFromFloats(0.f, 0.f, 0.f, 1.f);
    }
    return true;
  }

  const auto& m = _m;
  if (translation) {
    translation->copyFromFloats(m[12], m[13], m[14]);
  }

  scale    = scale ? scale : MathTmp::Vector3Array[0];
  scale->x = std::sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
  scale->y = std::sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
  scale->z = std::sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

  if (determinant() <= 0.f) {
    scale->y *= -1.f;
  }

  if (scale->x == 0.f || scale->y == 0.f || scale->z == 0.f) {
    if (rotation) {
      rotation->copyFromFloats(0.f, 0.f, 0.f, 1.f);
    }
    return false;
  }

  if (rotation) {
    const auto sx = 1.f / scale->x, sy = 1.f / scale->y, sz = 1.f / scale->z;
    Matrix::FromValuesToRef(m[0] * sx, m[1] * sx, m[2] * sx, 0.0,  //
                            m[4] * sy, m[5] * sy, m[6] * sy, 0.0,  //
                            m[8] * sz, m[9] * sz, m[10] * sz, 0.0, //
                            0.f, 0.f, 0.f, 1.f,                    //
                            MathTmp::MatrixArray[0]);

    Quaternion::FromRotationMatrixToRef(MathTmp::MatrixArray[0], *rotation);
  }

  return true;
}

std::optional<Vector4> Matrix::getRow(unsigned int index) const
{
  if (index > 3) {
    return std::nullopt;
  }

  const unsigned int i = index * 4;
  return Vector4(_m[i + 0], _m[i + 1], _m[i + 2], _m[i + 3]);
}

Matrix& Matrix::setRow(unsigned int index, const Vector4& row)
{
  return setRowFromFloats(index, row.x, row.y, row.z, row.w);
}

Matrix Matrix::transpose() const
{
  return Matrix::Transpose(*this);
}

Matrix& Matrix::transposeToRef(Matrix& result)
{
  Matrix::TransposeToRef(*this, result);

  return *this;
}

Matrix& Matrix::setRowFromFloats(unsigned int index, float x, float y, float z, float w)
{
  if (index > 3) {
    return *this;
  }

  const auto i = index * 4;
  _m[i + 0]    = x;
  _m[i + 1]    = y;
  _m[i + 2]    = z;
  _m[i + 3]    = w;

  _markAsUpdated();

  return *this;
}

Matrix Matrix::scale(float iscale)
{
  Matrix result;
  scaleToRef(iscale, result);
  return result;
}

const Matrix& Matrix::scaleToRef(float iscale, Matrix& result) const
{
  for (unsigned int index = 0; index < 16; ++index) {
    result._m[index] = _m[index] * iscale;
  }
  result._markAsUpdated();

  return *this;
}

const Matrix& Matrix::scaleAndAddToRef(float iscale, Matrix& result) const
{
  for (unsigned int index = 0; index < 16; ++index) {
    result._m[index] += _m[index] * iscale;
  }
  result._markAsUpdated();

  return *this;
}

void Matrix::toNormalMatrix(Matrix& ref)
{
  auto& tmp = MathTmp::MatrixArray[0];
  invertToRef(tmp);
  tmp.transposeToRef(ref);
  const auto& m = ref._m;
  Matrix::FromValuesToRef(m[0], m[1], m[2], 0.f,  //
                          m[4], m[5], m[6], 0.f,  //
                          m[8], m[9], m[10], 0.f, //
                          0.f, 0.f, 0.f, 1.f,     //
                          ref);
}

Matrix Matrix::getRotationMatrix() const
{
  Matrix result = Matrix::Identity();
  getRotationMatrixToRef(result);
  return result;
}

const Matrix& Matrix::getRotationMatrixToRef(Matrix& result) const
{
  std::optional<Vector3> scale       = MathTmp::Vector3Array[0];
  std::optional<Quaternion> rotation = std::nullopt;
  std::optional<Vector3> translation = std::nullopt;
  if (!decompose(scale, rotation, translation)) {
    Matrix::IdentityToRef(result);
    return *this;
  }

  const auto& m = _m;
  const auto sx = 1.f / scale->x, sy = 1.f / scale->y, sz = 1.f / scale->z;
  Matrix::FromValuesToRef(m[0] * sx, m[1] * sx, m[2] * sx, 0.f,  //
                          m[4] * sy, m[5] * sy, m[6] * sy, 0.f,  //
                          m[8] * sz, m[9] * sz, m[10] * sz, 0.f, //
                          0.f, 0.f, 0.f, 1.f,                    //
                          result);
  return *this;
}

void Matrix::toggleModelMatrixHandInPlace()
{
  auto& m = _m;
  m[2] *= -1.f;
  m[6] *= -1.f;
  m[8] *= -1.f;
  m[9] *= -1.f;
  m[14] *= -1.f;
  _markAsUpdated();
}

void Matrix::toggleProjectionMatrixHandInPlace()
{
  auto& m = _m;
  m[8] *= -1.f;
  m[9] *= -1.f;
  m[10] *= -1.f;
  m[11] *= -1.f;
  _markAsUpdated();
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Matrix& matrix)
{
  os << "{\"M\":[";
  for (unsigned int i = 0; i < 15; ++i) {
    os << matrix._m[i] << ",";
  }
  os << matrix._m[15] << "}";
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
  return _m[index];
}

const float& Matrix::operator[](unsigned int index) const
{
  return _m[index];
}

/** Statics **/
Matrix Matrix::FromArray(const Float32Array& array, unsigned int offset)
{
  Matrix result;
  Matrix::FromArrayToRef(array, offset, result);
  return result;
}

void Matrix::FromArrayToRef(const Float32Array& array, unsigned int offset, Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result._m[index] = array[index + offset];
  }
  result._markAsUpdated();
}

void Matrix::FromFloat32ArrayToRefScaled(const Float32Array& array, unsigned int offset,
                                         float scale, Matrix& result)
{
  for (unsigned int index = 0; index < 16; ++index) {
    result._m[index] = array[index + offset] * scale;
  }
  result._markAsUpdated();
}

Matrix Matrix::IdentityReadOnly()
{
  return Matrix::_identityReadOnly;
}

void Matrix::FromValuesToRef(float initialM11, float initialM12, float initialM13, float initialM14,
                             float initialM21, float initialM22, float initialM23, float initialM24,
                             float initialM31, float initialM32, float initialM33, float initialM34,
                             float initialM41, float initialM42, float initialM43, float initialM44,
                             Matrix& result)
{
  auto& m = result._m;
  m[0]    = initialM11;
  m[1]    = initialM12;
  m[2]    = initialM13;
  m[3]    = initialM14;
  m[4]    = initialM21;
  m[5]    = initialM22;
  m[6]    = initialM23;
  m[7]    = initialM24;
  m[8]    = initialM31;
  m[9]    = initialM32;
  m[10]   = initialM33;
  m[11]   = initialM34;
  m[12]   = initialM41;
  m[13]   = initialM42;
  m[14]   = initialM43;
  m[15]   = initialM44;

  result._markAsUpdated();
}

Matrix Matrix::FromValues(float initialM11, float initialM12, float initialM13, float initialM14,
                          float initialM21, float initialM22, float initialM23, float initialM24,
                          float initialM31, float initialM32, float initialM33, float initialM34,
                          float initialM41, float initialM42, float initialM43, float initialM44)
{
  Matrix result;
  auto& m = result._m;
  m[0]    = initialM11;
  m[1]    = initialM12;
  m[2]    = initialM13;
  m[3]    = initialM14;
  m[4]    = initialM21;
  m[5]    = initialM22;
  m[6]    = initialM23;
  m[7]    = initialM24;
  m[8]    = initialM31;
  m[9]    = initialM32;
  m[10]   = initialM33;
  m[11]   = initialM34;
  m[12]   = initialM41;
  m[13]   = initialM42;
  m[14]   = initialM43;
  m[15]   = initialM44;
  result._markAsUpdated();
  return result;
}

Matrix Matrix::Compose(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
{
  Matrix result;
  Matrix::ComposeToRef(scale, rotation, translation, result);
  return result;
}

void Matrix::ComposeToRef(const Vector3& scale, const Quaternion& rotation,
                          const Vector3& translation, Matrix& result)
{
  auto& m      = result._m;
  const auto x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;
  const auto x2 = x + x, y2 = y + y, z2 = z + z;
  const auto xx = x * x2, xy = x * y2, xz = x * z2;
  const auto yy = y * y2, yz = y * z2, zz = z * z2;
  const auto wx = w * x2, wy = w * y2, wz = w * z2;

  const auto sx = scale.x, sy = scale.y, sz = scale.z;

  m[0] = (1 - (yy + zz)) * sx;
  m[1] = (xy + wz) * sx;
  m[2] = (xz - wy) * sx;
  m[3] = 0;

  m[4] = (xy - wz) * sy;
  m[5] = (1 - (xx + zz)) * sy;
  m[6] = (yz + wx) * sy;
  m[7] = 0;

  m[8]  = (xz + wy) * sz;
  m[9]  = (yz - wx) * sz;
  m[10] = (1 - (xx + yy)) * sz;
  m[11] = 0;

  m[12] = translation.x;
  m[13] = translation.y;
  m[14] = translation.z;
  m[15] = 1;

  result._markAsUpdated();
}

Matrix Matrix::Identity()
{
  auto identity = Matrix::FromValues(1.f, 0.f, 0.f, 0.f, //
                                     0.f, 1.f, 0.f, 0.f, //
                                     0.f, 0.f, 1.f, 0.f, //
                                     0.f, 0.f, 0.f, 1.f);
  identity._updateIdentityStatus(true);
  return identity;
}

void Matrix::IdentityToRef(Matrix& result)
{
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, 1.f, 0.f, 0.f, //
                          0.f, 0.f, 1.f, 0.f, //
                          0.f, 0.f, 0.f, 1.f, result);
  result._updateIdentityStatus(true);
}

Matrix Matrix::Zero()
{
  auto zero = Matrix::FromValues(0.f, 0.f, 0.f, 0.f, //
                                 0.f, 0.f, 0.f, 0.f, //
                                 0.f, 0.f, 0.f, 0.f, //
                                 0.f, 0.f, 0.f, 0.f);
  zero._updateIdentityStatus(false);
  return zero;
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
  const auto s = std::sin(angle);
  const auto c = std::cos(angle);
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, c, s, 0.f,     //
                          0.f, -s, c, 0.f,    //
                          0.f, 0.f, 0.f, 1.f, //
                          result);

  result._updateIdentityStatus(c == 1.f && s == 0.f);
}

Matrix Matrix::RotationY(float angle)
{
  Matrix result;
  Matrix::RotationYToRef(angle, result);
  return result;
}

void Matrix::RotationYToRef(float angle, Matrix& result)
{
  const auto s = std::sin(angle);
  const auto c = std::cos(angle);
  Matrix::FromValuesToRef(c, 0.f, -s, 0.f,    //
                          0.f, 1.f, 0.f, 0.f, //
                          s, 0.f, c, 0.f,     //
                          0.f, 0.f, 0.f, 1.f, //
                          result);

  result._updateIdentityStatus(c == 1.f && s == 0.f);
}

Matrix Matrix::RotationZ(float angle)
{
  Matrix result;
  Matrix::RotationZToRef(angle, result);
  return result;
}

void Matrix::RotationZToRef(float angle, Matrix& result)
{
  const auto s = std::sin(angle);
  const auto c = std::cos(angle);
  Matrix::FromValuesToRef(c, s, 0.f, 0.f,     //
                          -s, c, 0.f, 0.f,    //
                          0.f, 0.f, 1.f, 0.f, //
                          0.f, 0.f, 0.f, 1.f, //
                          result);

  result._updateIdentityStatus(c == 1.f && s == 0.f);
}

Matrix Matrix::RotationAxis(Vector3& axis, float angle)
{
  Matrix result;
  Matrix::RotationAxisToRef(axis, angle, result);
  return result;
}

void Matrix::RotationAxisToRef(Vector3& axis, float angle, Matrix& result)
{
  const auto s  = std::sin(-angle);
  const auto c  = std::cos(-angle);
  const auto c1 = 1.f - c;

  axis.normalize();
  auto& m = result._m;
  m[0]    = (axis.x * axis.x) * c1 + c;
  m[1]    = (axis.x * axis.y) * c1 - (axis.z * s);
  m[2]    = (axis.x * axis.z) * c1 + (axis.y * s);
  m[3]    = 0.f;

  m[4] = (axis.y * axis.x) * c1 + (axis.z * s);
  m[5] = (axis.y * axis.y) * c1 + c;
  m[6] = (axis.y * axis.z) * c1 - (axis.x * s);
  m[7] = 0.f;

  m[8]  = (axis.z * axis.x) * c1 - (axis.y * s);
  m[9]  = (axis.z * axis.y) * c1 + (axis.x * s);
  m[10] = (axis.z * axis.z) * c1 + c;
  m[11] = 0.f;

  m[12] = 0.f;
  m[13] = 0.f;
  m[14] = 0.f;
  m[15] = 1.f;

  result._markAsUpdated();
}

void Matrix::RotationAlignToRef(const Vector3& from, const Vector3& to, Matrix& result)
{
  const auto v = Vector3::Cross(to, from);
  const auto c = Vector3::Dot(to, from);
  const auto k = 1.f / (1.f + c);

  auto& m = result._m;
  m[0]    = v.x * v.x * k + c;
  m[1]    = v.y * v.x * k - v.z;
  m[2]    = v.z * v.x * k + v.y;
  m[3]    = 0.f;
  m[4]    = v.x * v.y * k + v.z;
  m[5]    = v.y * v.y * k + c;
  m[6]    = v.z * v.y * k - v.x;
  m[7]    = 0.f;
  m[8]    = v.x * v.z * k - v.y;
  m[9]    = v.y * v.z * k + v.x;
  m[10]   = v.z * v.z * k + c;
  m[11]   = 0.f;
  m[12]   = 0.f;
  m[13]   = 0.f;
  m[14]   = 0.f;
  m[15]   = 1.f;

  result._markAsUpdated();
}

Matrix Matrix::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
  Matrix result;
  Matrix::RotationYawPitchRollToRef(yaw, pitch, roll, result);
  return result;
}

void Matrix::RotationYawPitchRollToRef(float yaw, float pitch, float roll, Matrix& result)
{
  Quaternion::RotationYawPitchRollToRef(yaw, pitch, roll, MathTmp::QuaternionArray[0]);
  MathTmp::QuaternionArray[0].toRotationMatrix(result);
}

Matrix Matrix::Scaling(float x, float y, float z)
{
  Matrix result;
  Matrix::ScalingToRef(x, y, z, result);
  return result;
}

void Matrix::ScalingToRef(float x, float y, float z, Matrix& result)
{
  Matrix::FromValuesToRef(x, 0.f, 0.f, 0.f,   //
                          0.f, y, 0.f, 0.f,   //
                          0.f, 0.f, z, 0.f,   //
                          0.f, 0.f, 0.f, 1.f, //
                          result);

  result._updateIdentityStatus(x == 1.f && y == 1.f && z == 1.f);
}

Matrix Matrix::Translation(float x, float y, float z)
{
  Matrix result;
  Matrix::TranslationToRef(x, y, z, result);
  return result;
}

void Matrix::TranslationToRef(float x, float y, float z, Matrix& result)
{
  Matrix::FromValuesToRef(1.f, 0.f, 0.f, 0.f, //
                          0.f, 1.f, 0.f, 0.f, //
                          0.f, 0.f, 1.f, 0.f, //
                          x, y, z, 1.f, result);
  result._updateIdentityStatus(x == 0.f && y == 0.f && z == 0.f);
}

Matrix Matrix::Lerp(const Matrix& startValue, const Matrix& endValue, float gradient)
{
  Matrix result;
  Matrix::LerpToRef(startValue, endValue, gradient, result);
  return result;
}

void Matrix::LerpToRef(const Matrix& startValue, const Matrix& endValue, float gradient,
                       Matrix& result)
{
  auto& resultM      = result._m;
  const auto& startM = startValue.m();
  const auto& endM   = endValue.m();
  for (unsigned int index = 0; index < 16; ++index) {
    resultM[index] = startM[index] * (1.f - gradient) + endM[index] * gradient;
  }

  result._markAsUpdated();
}

Matrix Matrix::DecomposeLerp(Matrix& startValue, Matrix& endValue, float gradient)
{
  Matrix result;
  Matrix::DecomposeLerpToRef(startValue, endValue, gradient, result);
  return result;
}

void Matrix::DecomposeLerpToRef(Matrix& startValue, Matrix& endValue, float gradient,
                                Matrix& result)
{
  std::optional<Vector3> startScale       = MathTmp::Vector3Array[0];
  std::optional<Quaternion> startRotation = MathTmp::QuaternionArray[0];
  std::optional<Vector3> startTranslation = MathTmp::Vector3Array[1];
  startValue.decompose(startScale, startRotation, startTranslation);

  std::optional<Vector3> endScale       = MathTmp::Vector3Array[2];
  std::optional<Quaternion> endRotation = MathTmp::QuaternionArray[1];
  std::optional<Vector3> endTranslation = MathTmp::Vector3Array[3];
  endValue.decompose(endScale, endRotation, endTranslation);

  auto& resultScale = MathTmp::Vector3Array[4];
  Vector3::LerpToRef(*startScale, *endScale, gradient, resultScale);
  auto& resultRotation = MathTmp::QuaternionArray[2];
  Quaternion::SlerpToRef(*startRotation, *endRotation, gradient, resultRotation);

  auto& resultTranslation = MathTmp::Vector3Array[5];
  Vector3::LerpToRef(*startTranslation, *endTranslation, gradient, resultTranslation);

  Matrix::ComposeToRef(resultScale, resultRotation, resultTranslation, result);
}

Matrix Matrix::LookAtLH(const Vector3& eye, Vector3& target, const Vector3& up)
{
  Matrix result;
  Matrix::LookAtLHToRef(eye, target, up, result);
  return result;
}

void Matrix::LookAtLHToRef(const Vector3& eye, const Vector3& target, const Vector3& up,
                           Matrix& result)
{
  auto& xAxis = MathTmp::Vector3Array[0];
  auto& yAxis = MathTmp::Vector3Array[1];
  auto& zAxis = MathTmp::Vector3Array[2];

  // Z axis
  target.subtractToRef(eye, zAxis);
  zAxis.normalize();

  // X axis
  Vector3::CrossToRef(up, zAxis, xAxis);

  const auto xSquareLength = xAxis.lengthSquared();
  if (xSquareLength == 0.f) {
    xAxis.x = 1.f;
  }
  else {
    xAxis.normalizeFromLength(std::sqrt(xSquareLength));
  }

  // Y axis
  Vector3::CrossToRef(zAxis, xAxis, yAxis);
  yAxis.normalize();

  // Eye angles
  const auto ex = -Vector3::Dot(xAxis, eye);
  const auto ey = -Vector3::Dot(yAxis, eye);
  const auto ez = -Vector3::Dot(zAxis, eye);

  Matrix::FromValuesToRef(xAxis.x, yAxis.x, zAxis.x, 0.f, //
                          xAxis.y, yAxis.y, zAxis.y, 0.f, //
                          xAxis.z, yAxis.z, zAxis.z, 0.f, //
                          ex, ey, ez, 1.f,                //
                          result);
}

Matrix Matrix::LookAtRH(const Vector3& eye, Vector3& target, const Vector3& up)
{
  Matrix result;
  Matrix::LookAtRHToRef(eye, target, up, result);

  return result;
}

void Matrix::LookAtRHToRef(const Vector3& eye, const Vector3& target, const Vector3& up,
                           Matrix& result)
{
  auto& xAxis = MathTmp::Vector3Array[0];
  auto& yAxis = MathTmp::Vector3Array[1];
  auto& zAxis = MathTmp::Vector3Array[2];

  // Z axis
  eye.subtractToRef(target, zAxis);
  zAxis.normalize();

  // X axis
  Vector3::CrossToRef(up, zAxis, xAxis);

  const auto xSquareLength = xAxis.lengthSquared();
  if (xSquareLength == 0.f) {
    xAxis.x = 1.f;
  }
  else {
    xAxis.normalizeFromLength(std::sqrt(xSquareLength));
  }

  // Y axis
  Vector3::CrossToRef(zAxis, xAxis, yAxis);
  yAxis.normalize();

  // Eye angles
  const auto ex = -Vector3::Dot(xAxis, eye);
  const auto ey = -Vector3::Dot(yAxis, eye);
  const auto ez = -Vector3::Dot(zAxis, eye);

  Matrix::FromValuesToRef(xAxis.x, yAxis.x, zAxis.x, 0.f, //
                          xAxis.y, yAxis.y, zAxis.y, 0.f, //
                          xAxis.z, yAxis.z, zAxis.z, 0.f, //
                          ex, ey, ez, 1.f,                //
                          result);
}

Matrix Matrix::LookDirectionLH(const Vector3& forward, const Vector3& up)
{
  Matrix result;
  Matrix::LookDirectionLHToRef(forward, up, result);
  return result;
}

void Matrix::LookDirectionLHToRef(const Vector3& forward, const Vector3& up, Matrix& result)
{
  auto& back = MathTmp::Vector3Array[0];
  back.copyFrom(forward);
  back.scaleInPlace(-1.f);
  auto& left = MathTmp::Vector3Array[1];
  Vector3::CrossToRef(up, back, left);

  // Generate the rotation matrix.
  Matrix::FromValuesToRef(left.x, left.y, left.z, 0.f, //
                          up.x, up.y, up.z, 0.f,       //
                          back.x, back.y, back.z, 0.f, //
                          0.f, 0.f, 0.f, 1.f,          //
                          result                       //
  );
}

Matrix Matrix::LookDirectionRH(const Vector3& forward, const Vector3& up)
{
  Matrix result;
  Matrix::LookDirectionRHToRef(forward, up, result);
  return result;
}

void Matrix::LookDirectionRHToRef(const Vector3& forward, const Vector3& up, Matrix& result)
{
  auto& right = MathTmp::Vector3Array[2];
  Vector3::CrossToRef(up, forward, right);

  // Generate the rotation matrix.
  Matrix::FromValuesToRef(right.x, right.y, right.z, 0.f,       //
                          up.x, up.y, up.z, 0.f,                //
                          forward.x, forward.y, forward.z, 0.f, //
                          0.f, 0.f, 0.f, 1.f,                   //
                          result                                //
  );
}

Matrix Matrix::OrthoLH(float width, float height, float znear, float zfar)
{
  Matrix matrix;
  Matrix::OrthoLHToRef(width, height, znear, zfar, matrix);
  return matrix;
}

void Matrix::OrthoLHToRef(float width, float height, float znear, float zfar, Matrix& result)
{
  const auto n = znear;
  const auto f = zfar;

  const auto a = 2.f / width;
  const auto b = 2.f / height;
  const auto c = 2.f / (f - n);
  const auto d = -(f + n) / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 0.f, //
                          0.f, 0.f, d, 1.f, //
                          result);

  result._updateIdentityStatus(a == 1.f && b == 1.f && c == 1.f && d == 0.f);
}

Matrix Matrix::OrthoOffCenterLH(float left, float right, float bottom, float top, float znear,
                                float zfar)
{
  Matrix matrix;
  Matrix::OrthoOffCenterLHToRef(left, right, bottom, top, znear, zfar, matrix);
  return matrix;
}

void Matrix::OrthoOffCenterLHToRef(float left, float right, float bottom, float top, float znear,
                                   float zfar, Matrix& result)
{
  const auto n = znear;
  const auto f = zfar;

  const auto a  = 2.f / (right - left);
  const auto b  = 2.f / (top - bottom);
  const auto c  = 2.f / (f - n);
  const auto d  = -(f + n) / (f - n);
  const auto i0 = (left + right) / (left - right);
  const auto i1 = (top + bottom) / (bottom - top);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 0.f, //
                          i0, i1, d, 1.f,   //
                          result);

  result._markAsUpdated();
}

Matrix Matrix::OrthoOffCenterRH(float left, float right, float bottom, float top, float znear,
                                float zfar)
{
  Matrix matrix;
  Matrix::OrthoOffCenterRHToRef(left, right, bottom, top, znear, zfar, matrix);
  return matrix;
}

void Matrix::OrthoOffCenterRHToRef(float left, float right, float bottom, float top, float znear,
                                   float zfar, Matrix& result)
{
  Matrix::OrthoOffCenterLHToRef(left, right, bottom, top, znear, zfar, result);
  result._m[10] *= -1.f; // No need to call _markAsUpdated as previous function
                         // already called it and let _isIdentityDirty to true
}

Matrix Matrix::PerspectiveLH(float width, float height, float znear, float zfar)
{
  Matrix matrix;

  const auto n = znear;
  const auto f = zfar;

  const auto a = 2.f * n / width;
  const auto b = 2.f * n / height;
  const auto c = (f + n) / (f - n);
  const auto d = -2.f * f * n / (f - n);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 1.f, //
                          0.f, 0.f, d, 0.f, //
                          matrix);

  matrix._updateIdentityStatus(false);
  return matrix;
}

Matrix Matrix::PerspectiveFovLH(float fov, float aspect, float znear, float zfar)
{
  Matrix matrix;
  Matrix::PerspectiveFovLHToRef(fov, aspect, znear, zfar, matrix);
  return matrix;
}

void Matrix::PerspectiveFovLHToRef(float fov, float aspect, float znear, float zfar, Matrix& result,
                                   bool isVerticalFovFixed)
{
  const auto n = znear;
  const auto f = zfar;

  const auto t = 1.f / (std::tan(fov * 0.5f));
  const auto a = isVerticalFovFixed ? (t / aspect) : t;
  const auto b = isVerticalFovFixed ? t : (t * aspect);
  const auto c = f != 0.f ? (f + n) / (f - n) : 1.f;
  const auto d = f != 0.f ? -2.f * f * n / (f - n) : -2.f * n;

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f, //
                          0.f, b, 0.f, 0.f, //
                          0.f, 0.f, c, 1.f, //
                          0.f, 0.f, d, 0.f, //
                          result);
  result._updateIdentityStatus(false);
}

void Matrix::PerspectiveFovReverseLHToRef(float fov, float aspect, float znear, float /*zfar*/,
                                          Matrix& result, bool isVerticalFovFixed)
{
  const auto t = 1.f / (std::tan(fov * 0.5f));
  const auto a = isVerticalFovFixed ? (t / aspect) : t;
  const auto b = isVerticalFovFixed ? t : (t * aspect);
  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f,      //
                          0.f, b, 0.f, 0.f,      //
                          0.f, 0.f, -znear, 1.f, //
                          0.f, 0.f, 1.f, 0.f,    //
                          result                 //
  );
  result._updateIdentityStatus(false);
}

Matrix Matrix::PerspectiveFovRH(float fov, float aspect, float znear, float zfar)
{
  Matrix matrix = Matrix::Zero();

  Matrix::PerspectiveFovRHToRef(fov, aspect, znear, zfar, matrix);

  return matrix;
}

void Matrix::PerspectiveFovRHToRef(float fov, float aspect, float znear, float zfar, Matrix& result,
                                   bool isVerticalFovFixed)
{
  // alternatively this could be expressed as:
  //    m = PerspectiveFovLHToRef
  //    m[10] *= -1.0;
  //    m[11] *= -1.0;

  const auto n = znear;
  const auto f = zfar;

  const auto t = 1.f / (std::tan(fov * 0.5f));
  const auto a = isVerticalFovFixed ? (t / aspect) : t;
  const auto b = isVerticalFovFixed ? t : (t * aspect);
  const auto c = f != 0.f ? -(f + n) / (f - n) : -1.f;
  const auto d = f != 0.f ? -2.f * f * n / (f - n) : -2.f * n;

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f,  //
                          0.f, b, 0.f, 0.f,  //
                          0.f, 0.f, c, -1.f, //
                          0.f, 0.f, d, 0.f,  //
                          result);

  result._updateIdentityStatus(false);
}

void Matrix::PerspectiveFovReverseRHToRef(float fov, float aspect, float znear, float /*zfar*/,
                                          Matrix& result, bool isVerticalFovFixed)
{
  // alternatively this could be expressed as:
  //    m = PerspectiveFovLHToRef
  //    m[10] *= -1.0;
  //    m[11] *= -1.0;

  const auto t = 1.f / (std::tan(fov * 0.5f));
  const auto a = isVerticalFovFixed ? (t / aspect) : t;
  const auto b = isVerticalFovFixed ? t : (t * aspect);

  Matrix::FromValuesToRef(a, 0.f, 0.f, 0.f,       //
                          0.f, b, 0.f, 0.f,       //
                          0.f, 0.f, -znear, -1.f, //
                          0.f, 0.f, -1.f, 0.f,    //
                          result                  //
  );

  result._updateIdentityStatus(false);
}

void Matrix::PerspectiveFovWebVRToRef(const VRFov& fov, float znear, float zfar, Matrix& result,
                                      bool rightHanded)
{
  const float rightHandedFactor = rightHanded ? -1.f : 1.f;
  const float upTan             = std::tan(fov.upDegrees * Math::PI / 180.f);
  const float downTan           = std::tan(fov.downDegrees * Math::PI / 180.f);
  const float leftTan           = std::tan(fov.leftDegrees * Math::PI / 180.f);
  const float rightTan          = std::tan(fov.rightDegrees * Math::PI / 180.f);
  const float xScale            = 2.f / (leftTan + rightTan);
  const float yScale            = 2.f / (upTan + downTan);
  auto& m                       = result._m;
  m[0]                          = xScale;
  m[1] = m[2] = m[3] = m[4] = 0.f;
  m[5]                      = yScale;
  m[6] = m[7] = 0.f;
  m[8]        = ((leftTan - rightTan) * xScale * 0.5f);
  m[9]        = -((upTan - downTan) * yScale * 0.5f);
  m[10]       = -zfar / (znear - zfar);
  m[11]       = 1.f * rightHandedFactor;
  m[12] = m[13] = m[15] = 0.f;
  m[14]                 = -(2.f * zfar * znear) / (zfar - znear);

  result._markAsUpdated();
}

Matrix Matrix::GetFinalMatrix(const Viewport& viewport, Matrix& world, Matrix& view,
                              Matrix& projection, float zmin, float zmax)
{
  const auto cw = static_cast<float>(viewport.width);
  const auto ch = static_cast<float>(viewport.height);
  const auto cx = static_cast<float>(viewport.x);
  const auto cy = static_cast<float>(viewport.y);

  auto viewportMatrix = Matrix::FromValues(cw / 2.f, 0.f, 0.f, 0.f,    //
                                           0.f, -ch / 2.f, 0.f, 0.f,   //
                                           0.f, 0.f, zmax - zmin, 0.f, //
                                           cx + cw / 2.f, ch / 2.f + cy, zmin, 1.f);

  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(view, matrix);
  matrix.multiplyToRef(projection, matrix);
  return matrix.multiply(viewportMatrix);
}

Float32Array Matrix::GetAsMatrix2x2(const Matrix& matrix)
{
  const auto& m = matrix.m();
  return {
    m[0], m[1], //
    m[4], m[5]  //
  };
}

Float32Array Matrix::GetAsMatrix3x3(const Matrix& matrix)
{
  const auto& m = matrix.m();
  return {
    m[0], m[1], m[2], //
    m[4], m[5], m[6], //
    m[8], m[9], m[10] //
  };
}

Matrix Matrix::Transpose(const Matrix& matrix)
{
  Matrix result;
  Matrix::TransposeToRef(matrix, result);
  return result;
}

void Matrix::TransposeToRef(const Matrix& matrix, Matrix& result)
{
  auto& rm = result._m;
  auto& mm = matrix.m();
  rm[0]    = mm[0];
  rm[1]    = mm[4];
  rm[2]    = mm[8];
  rm[3]    = mm[12];

  rm[4] = mm[1];
  rm[5] = mm[5];
  rm[6] = mm[9];
  rm[7] = mm[13];

  rm[8]  = mm[2];
  rm[9]  = mm[6];
  rm[10] = mm[10];
  rm[11] = mm[14];

  rm[12] = mm[3];
  rm[13] = mm[7];
  rm[14] = mm[11];
  rm[15] = mm[15];
  // identity-ness does not change when transposing
  result._updateIdentityStatus((matrix)._isIdentity, (matrix)._isIdentityDirty);
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
  const auto& x    = plane.normal.x;
  const auto& y    = plane.normal.y;
  const auto& z    = plane.normal.z;
  const auto temp  = -2.f * x;
  const auto temp2 = -2.f * y;
  const auto temp3 = -2.f * z;
  Matrix::FromValuesToRef(temp * x + 1.f, temp2 * x, temp3 * x, 0.f, //
                          temp * y, temp2 * y + 1, temp3 * y, 0.f,   //
                          temp * z, temp2 * z, temp3 * z + 1, 0.f,   //
                          temp * plane.d, temp2 * plane.d, temp3 * plane.d,
                          1.f, //
                          result);
}

void Matrix::FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis,
                              Matrix& result)
{
  Matrix::FromValuesToRef(xaxis.x, xaxis.y, xaxis.z, 0.f, //
                          yaxis.x, yaxis.y, yaxis.z, 0.f, //
                          zaxis.x, zaxis.y, zaxis.z, 0.f, //
                          0.f, 0.f, 0.f, 1.f,             //
                          result);
}

void Matrix::FromQuaternionToRef(const Quaternion& quat, Matrix& result)
{
  const auto xx = quat.x * quat.x;
  const auto yy = quat.y * quat.y;
  const auto zz = quat.z * quat.z;
  const auto xy = quat.x * quat.y;
  const auto zw = quat.z * quat.w;
  const auto zx = quat.z * quat.x;
  const auto yw = quat.y * quat.w;
  const auto yz = quat.y * quat.z;
  const auto xw = quat.x * quat.w;

  result._m[0] = 1.f - (2.f * (yy + zz));
  result._m[1] = 2.f * (xy + zw);
  result._m[2] = 2.f * (zx - yw);
  result._m[3] = 0.f;

  result._m[4] = 2.f * (xy - zw);
  result._m[5] = 1.f - (2.f * (zz + xx));
  result._m[6] = 2.f * (yz + xw);
  result._m[7] = 0.f;

  result._m[8]  = 2.f * (zx + yw);
  result._m[9]  = 2.f * (yz - xw);
  result._m[10] = 1.f - (2.f * (yy + xx));
  result._m[11] = 0.f;

  result._m[12] = 0.f;
  result._m[13] = 0.f;
  result._m[14] = 0.f;
  result._m[15] = 1.f;

  result._markAsUpdated();
}

} // end of namespace BABYLON
