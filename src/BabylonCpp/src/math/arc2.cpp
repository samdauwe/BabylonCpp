#include <babylon/math/arc2.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

Arc2::Arc2(const Vector2& startPoint, const Vector2& midPoint,
           const Vector2& endPoint)
{
  float temp = ::std::pow(midPoint.x, 2.f) + ::std::pow(midPoint.y, 2.f);
  float startToMid
    = (::std::pow(startPoint.x, 2.f) + ::std::pow(startPoint.y, 2.f) - temp)
      / 2.f;
  float midToEnd
    = (temp - ::std::pow(endPoint.x, 2.f) - ::std::pow(endPoint.y, 2.f)) / 2.f;
  float det = (startPoint.x - midPoint.x) * (midPoint.y - endPoint.y)
              - (midPoint.x - endPoint.x) * (startPoint.y - midPoint.y);

  centerPoint = Vector2((startToMid * (midPoint.y - endPoint.y)
                         - midToEnd * (startPoint.y - midPoint.y))
                          / det,
                        ((startPoint.x - midPoint.x) * midToEnd
                         - (midPoint.x - endPoint.x) * startToMid)
                          / det);

  radius = centerPoint.subtract(startPoint).length();

  startAngle = Angle::BetweenTwoPoints(centerPoint, startPoint);

  float a1 = startAngle.degrees();
  float a2 = Angle::BetweenTwoPoints(centerPoint, midPoint).degrees();
  float a3 = Angle::BetweenTwoPoints(centerPoint, endPoint).degrees();

  // angles correction
  if (a2 - a1 > +180.f) {
    a2 -= 360.f;
  }
  if (a2 - a1 < -180.f) {
    a2 += 360.f;
  }
  if (a3 - a2 > +180.f) {
    a3 -= 360.f;
  }
  if (a3 - a2 < -180.f) {
    a3 += 360.f;
  }

  orientation = (a2 - a1) < 0 ? Orientation::CW : Orientation::CCW;
  angle
    = Angle::FromDegrees(orientation == Orientation::CW ? a1 - a3 : a3 - a1);
}

Arc2::Arc2(const Arc2& otherArc)
    : centerPoint{otherArc.centerPoint}
    , radius{otherArc.radius}
    , angle{otherArc.angle}
    , startAngle{otherArc.startAngle}
    , orientation{otherArc.orientation}
{
}

Arc2::Arc2(Arc2&& otherArc)
    : centerPoint{::std::move(otherArc.centerPoint)}
    , radius{::std::move(otherArc.radius)}
    , angle{::std::move(otherArc.angle)}
    , startAngle{::std::move(otherArc.startAngle)}
    , orientation{::std::move(otherArc.orientation)}
{
}

Arc2& Arc2::operator=(const Arc2& otherArc)
{
  if (&otherArc != this) {
    centerPoint = otherArc.centerPoint;
    radius      = otherArc.radius;
    angle       = otherArc.angle;
    startAngle  = otherArc.startAngle;
    orientation = otherArc.orientation;
  }

  return *this;
}

Arc2& Arc2::operator=(Arc2&& otherArc)
{
  if (&otherArc != this) {
    centerPoint = ::std::move(otherArc.centerPoint);
    radius      = ::std::move(otherArc.radius);
    angle       = ::std::move(otherArc.angle);
    startAngle  = ::std::move(otherArc.startAngle);
    orientation = ::std::move(otherArc.orientation);
  }

  return *this;
}

Arc2::~Arc2()
{
}

Arc2 Arc2::copy() const
{
  return Arc2(*this);
}

std::unique_ptr<Arc2> Arc2::clone() const
{
  return ::std::make_unique<Arc2>(*this);
}

std::ostream& operator<<(std::ostream& os, const Arc2& arc)
{
  os << "{\"CenterPoint\":" << arc.centerPoint << ",\"Radius\":" << arc.radius
     << ",\"Angle\":" << arc.angle << ",\"StartAngle\":" << arc.startAngle
     << ",\"Orientation\":" << static_cast<int>(arc.orientation) << "}";
  return os;
}

bool operator==(const Arc2& lhs, const Arc2& rhs)
{
  return (lhs.centerPoint == rhs.centerPoint)
         && stl_util::almost_equal(lhs.radius, rhs.radius)
         && (lhs.angle == rhs.angle) && (lhs.startAngle == rhs.startAngle)
         && (lhs.orientation == rhs.orientation);
}

bool operator!=(const Arc2& lhs, const Arc2& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
