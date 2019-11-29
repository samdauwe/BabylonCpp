#include <babylon/maths/arc2.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

Arc2::Arc2(const Vector2& iStartPoint, const Vector2& iMidPoint,
           const Vector2& iEndPoint)
    : startPoint{iStartPoint}, midPoint{iMidPoint}, endPoint{iEndPoint}
{
  float temp = std::pow(iMidPoint.x, 2.f) + std::pow(iMidPoint.y, 2.f);
  float startToMid
    = (std::pow(iStartPoint.x, 2.f) + std::pow(iStartPoint.y, 2.f) - temp)
      / 2.f;
  float midToEnd
    = (temp - std::pow(iEndPoint.x, 2.f) - std::pow(iEndPoint.y, 2.f)) / 2.f;
  float det = (iStartPoint.x - iMidPoint.x) * (iMidPoint.y - iEndPoint.y)
              - (iMidPoint.x - iEndPoint.x) * (iStartPoint.y - iMidPoint.y);

  centerPoint = Vector2((startToMid * (iMidPoint.y - iEndPoint.y)
                         - midToEnd * (iStartPoint.y - iMidPoint.y))
                          / det,
                        ((iStartPoint.x - iMidPoint.x) * midToEnd
                         - (iMidPoint.x - iEndPoint.x) * startToMid)
                          / det);

  radius = centerPoint.subtract(iStartPoint).length();

  startAngle = Angle::BetweenTwoPoints(centerPoint, iStartPoint);

  float a1 = startAngle.degrees();
  float a2 = Angle::BetweenTwoPoints(centerPoint, iMidPoint).degrees();
  float a3 = Angle::BetweenTwoPoints(centerPoint, iEndPoint).degrees();

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
    : centerPoint{std::move(otherArc.centerPoint)}
    , radius{std::move(otherArc.radius)}
    , angle{std::move(otherArc.angle)}
    , startAngle{std::move(otherArc.startAngle)}
    , orientation{std::move(otherArc.orientation)}
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
    centerPoint = std::move(otherArc.centerPoint);
    radius      = std::move(otherArc.radius);
    angle       = std::move(otherArc.angle);
    startAngle  = std::move(otherArc.startAngle);
    orientation = std::move(otherArc.orientation);
  }

  return *this;
}

Arc2::~Arc2() = default;

Arc2 Arc2::copy() const
{
  return Arc2(*this);
}

std::unique_ptr<Arc2> Arc2::clone() const
{
  return std::make_unique<Arc2>(*this);
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
