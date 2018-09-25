#include <babylon/math/curve3.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

Curve3::Curve3()
{
}

Curve3::Curve3(const vector_t<Vector3>& points) : _length{0.f}
{
  _points = points;
  _length = computeLength(points);
}

Curve3::Curve3(const Curve3& otherCurve)
    : _points{otherCurve._points}, _length{otherCurve._length}
{
}

Curve3::Curve3(Curve3&& otherCurve)
    : _points{std::move(otherCurve._points)}
    , _length{std::move(otherCurve._length)}
{
}

Curve3& Curve3::operator=(const Curve3& otherCurve)
{
  if (&otherCurve != this) {
    _points = otherCurve._points;
    _length = otherCurve._length;
  }

  return *this;
}

Curve3& Curve3::operator=(Curve3&& otherCurve)
{
  if (&otherCurve != this) {
    _points = std::move(otherCurve._points);
    _length = std::move(otherCurve._length);
  }

  return *this;
}

Curve3::~Curve3()
{
}

Curve3 Curve3::copy() const
{
  return Curve3(*this);
}

unique_ptr_t<Curve3> Curve3::clone() const
{
  return std::make_unique<Curve3>(*this);
}

std::ostream& operator<<(std::ostream& os, const Curve3& curve)
{
  os << "{\"Points\":[";
  if (curve._points.size() > 0) {
    for (unsigned int i = 0; i < curve._points.size() - 1; ++i) {
      os << curve._points[i] << ",";
    }
    os << curve._points.back();
  }
  os << "],\"Length\":" << curve._length << "}";
  return os;
}

vector_t<Vector3>& Curve3::getPoints()
{
  return _points;
}

float Curve3::length() const
{
  return _length;
}

Curve3 Curve3::continueCurve3(Curve3& curve) const
{
  const Vector3 lastPoint           = _points[_points.size() - 1];
  vector_t<Vector3> continuedPoints = _points;
  vector_t<Vector3> curvePoints     = curve.getPoints();
  for (unsigned int i = 1; i < curvePoints.size(); ++i) {
    continuedPoints.emplace_back(
      curvePoints[i].subtract(curvePoints[0]).add(lastPoint));
  }
  return Curve3(continuedPoints);
}

Curve3 Curve3::CreateQuadraticBezier(const Vector3& v0, const Vector3& v1,
                                     const Vector3& v2, int nbPoints)
{
  const float nbPointsVal = nbPoints > 2 ? static_cast<float>(nbPoints) : 3.f;
  vector_t<Vector3> bez;
  const auto equation
    = [](float t, float val0, float val1, float val2) -> float {
    return (1.f - t) * (1.f - t) * val0 + 2.f * t * (1.f - t) * val1
           + t * t * val2;
  };
  for (float i = 0; i <= nbPointsVal; ++i) {
    bez.emplace_back(Vector3(equation(i / nbPointsVal, v0.x, v1.x, v2.x),
                             equation(i / nbPointsVal, v0.y, v1.y, v2.y),
                             equation(i / nbPointsVal, v0.z, v1.z, v2.z)));
  }
  return Curve3(bez);
}

Curve3 Curve3::CreateCubicBezier(const Vector3& v0, const Vector3& v1,
                                 const Vector3& v2, const Vector3& v3,
                                 int nbPoints)
{
  const float nbPointsVal = nbPoints > 3 ? static_cast<float>(nbPoints) : 4.f;
  vector_t<Vector3> bez;
  const auto equation
    = [](float t, float val0, float val1, float val2, float val3) -> float {
    return (1.f - t) * (1.f - t) * (1.f - t) * val0
           + 3.f * t * (1.f - t) * (1.f - t) * val1
           + 3.f * t * t * (1.f - t) * val2 + t * t * t * val3;
  };
  for (float i = 0; i <= nbPointsVal; ++i) {
    bez.emplace_back(
      Vector3(equation(i / nbPointsVal, v0.x, v1.x, v2.x, v3.x),
              equation(i / nbPointsVal, v0.y, v1.y, v2.y, v3.y),
              equation(i / nbPointsVal, v0.z, v1.z, v2.z, v3.z)));
  }
  return Curve3(bez);
}

Curve3 Curve3::CreateHermiteSpline(const Vector3& p1, const Vector3& t1,
                                   const Vector3& p2, const Vector3& t2,
                                   int nbPoints)
{
  vector_t<Vector3> hermite;
  const float nbPointsVal = static_cast<float>(nbPoints);
  const float step        = 1.f / nbPointsVal;
  for (float i = 0; i <= nbPointsVal; ++i) {
    hermite.emplace_back(Vector3::Hermite(p1, t1, p2, t2, i * step));
  }
  return Curve3(hermite);
}

Curve3 Curve3::CreateCatmullRomSpline(const vector_t<Vector3>& points,
                                      size_t nbPoints, bool closed)
{
  vector_t<Vector3> catmullRom;
  const float step = 1.f / static_cast<float>(nbPoints);
  float amount     = 0.f;
  if (closed) {
    const auto pointsCount = points.size();
    for (size_t i = 0; i < pointsCount; ++i) {
      amount = 0;
      for (size_t c = 0; c < nbPoints; ++c) {
        catmullRom.emplace_back(Vector3::CatmullRom(
          points[i % pointsCount], points[(i + 1) % pointsCount],
          points[(i + 2) % pointsCount], points[(i + 3) % pointsCount],
          amount));
        amount += step;
      }
    }
    catmullRom.emplace_back(catmullRom[0]);
  }
  else {
    vector_t<Vector3> totalPoints;
    totalPoints.emplace_back(points[0]);
    stl_util::concat(totalPoints, points);
    totalPoints.emplace_back(points.back());
    size_t i = 0;
    for (i = 0; i < totalPoints.size() - 3; ++i) {
      amount = 0;
      for (size_t c = 0; c < nbPoints; ++c) {
        catmullRom.emplace_back(
          Vector3::CatmullRom(totalPoints[i], totalPoints[i + 1],
                              totalPoints[i + 2], totalPoints[i + 3], amount));
        amount += step;
      }
    }
    --i;
    catmullRom.emplace_back(
      Vector3::CatmullRom(totalPoints[i], totalPoints[i + 1],
                          totalPoints[i + 2], totalPoints[i + 3], amount));
  }
  return Curve3(catmullRom);
}

float Curve3::computeLength(const vector_t<Vector3>& path)
{
  float l = 0;
  for (unsigned int i = 1; i < path.size(); ++i) {
    l += (path[i].subtract(path[i - 1])).length();
  }
  return l;
}

} // end of namespace BABYLON
