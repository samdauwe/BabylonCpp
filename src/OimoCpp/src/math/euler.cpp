#include <oimo/math/euler.h>

#include <math.h>

#include <oimo/math/mat33.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Euler::Euler(float x, float y, float z, const RotationOrder& order)
    : _x{x}, _y{y}, _z{z}, _order{order}
{
  onChangeCallback = []() {};
}

Euler::Euler(const Euler& e)
    : onChangeCallback{e.onChangeCallback}
    , _x{e._x}
    , _y{e._y}
    , _z{e._z}
    , _order{e._order}
{
}

Euler::Euler(Euler&& e)
{
  *this = std::move(e);
}

Euler::~Euler()
{
}

Euler& Euler::operator=(const Euler& e)
{
  if (&e != this) {
    onChangeCallback = e.onChangeCallback;
    _x               = e._x;
    _y               = e._y;
    _z               = e._z;
    _order           = e._order;
  }

  return *this;
}

Euler& Euler::operator=(Euler&& e)
{
  if (&e != this) {
    std::swap(onChangeCallback, e.onChangeCallback);
    std::swap(_x, e._x);
    std::swap(_y, e._y);
    std::swap(_z, e._z);
    std::swap(_order, e._order);
  }

  return *this;
}

Euler Euler::clone() const
{
  return Euler(*this);
}

Euler* Euler::cloneToNewObject() const
{
  return new Euler(*this);
}

float Euler::x() const
{
  return _x;
}

void Euler::setX(float value)
{
  _x = value;
  onChangeCallback();
}

float Euler::Y() const
{
  return _y;
}

void Euler::setY(float value)
{
  _y = value;
  onChangeCallback();
}

float Euler::Z() const
{
  return _z;
}

void Euler::setZ(float value)
{
  _z = value;
  onChangeCallback();
}

Euler::RotationOrder Euler::order() const
{
  return _order;
}

void Euler::setOrder(const RotationOrder& value)
{
  _order = value;
  onChangeCallback();
}

Euler& Euler::set(float x, float y, float z)
{
  _x = x;
  _y = y;
  _z = z;

  onChangeCallback();

  return *this;
}

Euler& Euler::set(float x, float y, float z, const RotationOrder& order)
{
  _x     = x;
  _y     = y;
  _z     = z;
  _order = order;

  onChangeCallback();

  return *this;
}

Euler& Euler::copy(const Euler& euler)
{
  _x     = euler._x;
  _y     = euler._y;
  _z     = euler._z;
  _order = euler._order;

  onChangeCallback();

  return *this;
}

Euler& Euler::setFromRotationMatrix(const Mat33& m)
{
  return setFromRotationMatrix(m, _order);
}

Euler& Euler::setFromRotationMatrix(const Mat33& m, const RotationOrder& order)
{
  // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

  const std::array<float, 9>& te = m.elements;

  float m11 = te[0], m12 = te[1], m13 = te[2];
  float m21 = te[3], m22 = te[4], m23 = te[5];
  float m31 = te[6], m32 = te[7], m33 = te[8];

  if (order == RotationOrder::XYZ) {

    _y = std::asin(clamp(m13, -1.f, 1.f));

    if (std::abs(m13) < 0.99999f) {

      _x = std::atan2(-m23, m33);
      _z = std::atan2(-m12, m11);
    }
    else {

      _x = std::atan2(m32, m22);
      _z = 0.f;
    }
  }
  else if (order == RotationOrder::YXZ) {

    _x = std::asin(-clamp(m23, -1.f, 1.f));

    if (std::abs(m23) < 0.99999f) {

      _y = std::atan2(m13, m33);
      _z = std::atan2(m21, m22);
    }
    else {

      _y = std::atan2(-m31, m11);
      _z = 0.f;
    }
  }
  else if (order == RotationOrder::ZXY) {

    _x = std::asin(clamp(m32, -1.f, 1.f));

    if (std::abs(m32) < 0.99999f) {

      _y = std::atan2(-m31, m33);
      _z = std::atan2(-m12, m22);
    }
    else {

      _y = 0.f;
      _z = std::atan2(m21, m11);
    }
  }
  else if (order == RotationOrder::ZYX) {

    _y = std::asin(-clamp(m31, -1.f, 1.f));

    if (std::abs(m31) < 0.99999f) {

      _x = std::atan2(m32, m33);
      _z = std::atan2(m21, m11);
    }
    else {

      _x = 0;
      _z = std::atan2(-m12, m22);
    }
  }
  else if (order == RotationOrder::YZX) {

    _z = std::asin(clamp(m21, -1.f, 1.f));

    if (std::abs(m21) < 0.99999f) {

      _x = std::atan2(-m23, m22);
      _y = std::atan2(-m31, m11);
    }
    else {

      _x = 0.f;
      _y = std::atan2(m13, m33);
    }
  }
  else if (order == RotationOrder::XZY) {

    _z = std::asin(-clamp(m12, -1.f, 1.f));

    if (std::abs(m12) < 0.99999f) {

      _x = std::atan2(m32, m22);
      _y = std::atan2(m13, m11);
    }
    else {

      _x = std::atan2(-m23, m33);
      _y = 0.f;
    }
  }

  _order = order;

  onChangeCallback();

  return *this;
}

Euler& Euler::setFromQuaternion(const Quat& q, bool update)
{
  return setFromQuaternion(q, _order, update);
}

Euler& Euler::setFromQuaternion(const Quat& q, const RotationOrder& order,
                                bool update)
{
  // q is assumed to be normalized
  // http://www.mathworks.com/matlabcentral/fileexchange/20696-function-to-convert-between-dcm-euler-angles-quaternions-and-euler-vectors/content/SpinCalc.m

  float sqx = q.x * q.x;
  float sqy = q.y * q.y;
  float sqz = q.z * q.z;
  float sqw = q.s * q.s;

  if (order == RotationOrder::XYZ) {

    _x = std::atan2(2.f * (q.x * q.s - q.y * q.z), (sqw - sqx - sqy + sqz));
    _y = std::asin(clamp(2.f * (q.x * q.z + q.y * q.s), -1.f, 1.f));
    _z = std::atan2(2.f * (q.z * q.s - q.x * q.y), (sqw + sqx - sqy - sqz));
  }
  else if (order == RotationOrder::YXZ) {

    _x = std::asin(clamp(2.f * (q.x * q.s - q.y * q.z), -1.f, 1.f));
    _y = std::atan2(2.f * (q.x * q.z + q.y * q.s), (sqw - sqx - sqy + sqz));
    _z = std::atan2(2.f * (q.x * q.y + q.z * q.s), (sqw - sqx + sqy - sqz));
  }
  else if (order == RotationOrder::ZXY) {

    _x = std::asin(clamp(2.f * (q.x * q.s + q.y * q.z), -1.f, 1.f));
    _y = std::atan2(2.f * (q.y * q.s - q.z * q.x), (sqw - sqx - sqy + sqz));
    _z = std::atan2(2.f * (q.z * q.s - q.x * q.y), (sqw - sqx + sqy - sqz));
  }
  else if (order == RotationOrder::ZYX) {

    _x = std::atan2(2.f * (q.x * q.s + q.z * q.y), (sqw - sqx - sqy + sqz));
    _y = std::asin(clamp(2.f * (q.y * q.s - q.x * q.z), -1.f, 1.f));
    _z = std::atan2(2.f * (q.x * q.y + q.z * q.s), (sqw + sqx - sqy - sqz));
  }
  else if (order == RotationOrder::YZX) {

    _x = std::atan2(2.f * (q.x * q.s - q.z * q.y), (sqw - sqx + sqy - sqz));
    _y = std::atan2(2.f * (q.y * q.s - q.x * q.z), (sqw + sqx - sqy - sqz));
    _z = std::asin(clamp(2.f * (q.x * q.y + q.z * q.s), -1.f, 1.f));
  }
  else if (order == RotationOrder::XZY) {

    _x = std::atan2(2.f * (q.x * q.s + q.y * q.z), (sqw - sqx + sqy - sqz));
    _y = std::atan2(2.f * (q.x * q.z + q.y * q.s), (sqw + sqx - sqy - sqz));
    _z = std::asin(clamp(2.f * (q.z * q.s - q.x * q.y), -1.f, 1.f));
  }

  _order = order;

  if (update)
    onChangeCallback();

  return *this;
}

bool Euler::equals(const Euler& euler) const
{
  return (floats_are_equal(euler._x, _x)
          && floats_are_equal(euler._y, _y)
          && floats_are_equal(euler._z, _z) && (euler._order == _order));
}

Euler& Euler::fromArray3(std::array<float, 3> array)
{
  _x = array[0];
  _y = array[1];
  _z = array[2];

  onChangeCallback();

  return *this;
}

Euler& Euler::fromArray4(std::array<float, 4> array)
{
  _x     = array[0];
  _y     = array[1];
  _z     = array[2];
  _order = Euler::NumberToRotationOrder(static_cast<int>(array[3]));

  onChangeCallback();

  return *this;
}

std::array<float, 4> Euler::toArray() const
{
  const float order = static_cast<float>(Euler::RotationOrderToNumber(_order));

  return {{_x, _y, _z, order}};
}

Euler& Euler::onChange(const std::function<void()>& callback)
{
  onChangeCallback = callback;

  return *this;
}

int Euler::RotationOrderToNumber(const RotationOrder& order)
{
  switch (order) {
    case RotationOrder::XYZ:
      return static_cast<int>(RotationOrder::XYZ);
    case RotationOrder::YZX:
      return static_cast<int>(RotationOrder::YZX);
    case RotationOrder::ZXY:
      return static_cast<int>(RotationOrder::ZXY);
    case RotationOrder::XZY:
      return static_cast<int>(RotationOrder::XZY);
    case RotationOrder::YXZ:
      return static_cast<int>(RotationOrder::YXZ);
    case RotationOrder::ZYX:
      return static_cast<int>(RotationOrder::ZYX);
    default:
      return static_cast<int>(RotationOrder::XYZ);
  }
}

Euler::RotationOrder Euler::NumberToRotationOrder(int number)
{
  if (number == static_cast<int>(RotationOrder::XYZ))
    return RotationOrder::XYZ;
  else if (number == static_cast<int>(RotationOrder::YZX))
    return RotationOrder::YZX;
  else if (number == static_cast<int>(RotationOrder::ZXY))
    return RotationOrder::ZXY;
  else if (number == static_cast<int>(RotationOrder::XZY))
    return RotationOrder::XZY;
  else if (number == static_cast<int>(RotationOrder::YXZ))
    return RotationOrder::YXZ;
  else if (number == static_cast<int>(RotationOrder::ZYX))
    return RotationOrder::ZYX;
  else
    return RotationOrder::XYZ;
}

} // end of namespace OIMO
