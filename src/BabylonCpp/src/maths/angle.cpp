#include <babylon/maths/angle.h>

#include <cmath>

#include <babylon/babylon_constants.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

Angle::Angle() = default;

Angle::Angle(float iradians) : _radians{iradians}
{
  if (_radians < 0.f) {
    _radians += Math::PI2;
  }
}

Angle::Angle(const Angle& otherAngle) = default;

Angle::Angle(Angle&& otherAngle) = default;

Angle& Angle::operator=(const Angle& otherAngle) = default;

Angle& Angle::operator=(Angle&& otherAngle) = default;

Angle::~Angle() = default;

Angle Angle::copy() const
{
  return Angle(*this);
}

std::unique_ptr<Angle> Angle::clone() const
{
  return std::make_unique<Angle>(*this);
}

std::ostream& operator<<(std::ostream& os, const Angle& angle)
{
  os << "{\"Degrees\":" << angle.degrees() << "}";
  return os;
}

bool operator==(const Angle& lhs, const Angle& rhs)
{
  return stl_util::almost_equal(lhs._radians, rhs._radians);
}

bool operator!=(const Angle& lhs, const Angle& rhs)
{
  return !(lhs == rhs);
}

float Angle::degrees() const
{
  return _radians * 180.f / Math::PI;
}

float Angle::radians() const
{
  return _radians;
}

Angle Angle::BetweenTwoPoints(const Vector2& a, const Vector2& b)
{
  const auto delta = b.subtract(a);
  const auto theta = std::atan2(delta.y, delta.x);
  return Angle(theta);
}

Angle Angle::FromRadians(float radians)
{
  return Angle(radians);
}

Angle Angle::FromDegrees(float degrees)
{
  return Angle(degrees * Math::PI / 180.f);
}

} // end of namespace BABYLON
