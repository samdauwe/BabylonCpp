#include <babylon/maths/_iso_vector.h>

#include <cmath>

#include <babylon/core/logging.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

_IsoVector::_IsoVector() : _IsoVector{0, 0}
{
}

_IsoVector::_IsoVector(float iX, float iY)
    : _IsoVector{static_cast<int>(std::floor(iX)), static_cast<int>(std::floor(iY))}
{
  BABYLON_LOG_WARN("_IsoVector", "x is not an integer, floor(x) used");
  BABYLON_LOG_WARN("_IsoVector", "y is not an integer, floor(y) used");
}

_IsoVector::_IsoVector(int iX, int iY) : x{iX}, y{iY}
{
}

_IsoVector _IsoVector::clone() const
{
  return _IsoVector(x, y);
}

_IsoVector& _IsoVector::rotate60About(const _IsoVector& other)
{ // other IsoVector
  auto _x = x;
  x       = other.x + other.y - y;
  y       = _x + y - other.x;
  return *this;
}

_IsoVector& _IsoVector::rotateNeg60About(const _IsoVector& other)
{
  auto _x = x;
  x       = _x + y - other.y;
  y       = other.x + other.y - _x;
  return *this;
}

_IsoVector& _IsoVector::rotate120(float m, float n)
{
  BABYLON_LOG_WARN("_IsoVector", "x is not an integer, floor(x) used");
  BABYLON_LOG_WARN("_IsoVector", "y is not an integer, floor(y) used");
  return rotate120(static_cast<int>(std::floor(m)), static_cast<int>(std::floor(n)));
}

_IsoVector& _IsoVector::rotate120(int m, int n)
{ // m, n integers
  auto _x = x;
  x       = m - _x - y;
  y       = n + _x;
  return *this;
}

_IsoVector& _IsoVector::rotateNeg120(float m, float n)
{
  BABYLON_LOG_WARN("_IsoVector", "x is not an integer, floor(x) used");
  BABYLON_LOG_WARN("_IsoVector", "y is not an integer, floor(y) used");
  return rotateNeg120(static_cast<int>(std::floor(m)), static_cast<int>(std::floor(n)));
}

_IsoVector& _IsoVector::rotateNeg120(int m, int n)
{ // m, n integers
  auto _x = x;
  x       = y - n;
  y       = m + n - _x - y;
  return *this;
}

Vector3 _IsoVector::toCartesianOrigin(const _IsoVector& origin, int isoGridSize)
{
  auto point = Vector3::Zero();
  point.x    = origin.x + 2.f * x * isoGridSize + y * isoGridSize;
  point.y    = origin.y + std::sqrt(3.f) * y * isoGridSize;
  return point;
}

_IsoVector _IsoVector::Zero()
{
  return _IsoVector(0, 0);
}

} // end of namespace BABYLON
