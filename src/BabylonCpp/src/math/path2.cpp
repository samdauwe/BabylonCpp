#include <babylon/math/path2.h>

#include <babylon/babylon_stl.h>
#include <babylon/math/arc2.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

Path2::Path2(float x, float y) : closed{false}, _length{0.f}
{
  _points.emplace_back(Vector2(x, y));
}

Path2::Path2(const Path2& otherPath)
    : _points{otherPath._points}, _length{otherPath._length}
{
}

Path2::Path2(Path2&& otherPath)
    : _points{std::move(otherPath._points)}
    , _length{std::move(otherPath._length)}
{
}

Path2& Path2::operator=(const Path2& otherPath)
{
  if (&otherPath != this) {
    closed  = otherPath.closed;
    _points = otherPath._points;
    _length = otherPath._length;
  }

  return *this;
}

Path2& Path2::operator=(Path2&& otherPath)
{
  if (&otherPath != this) {
    closed  = std::move(otherPath.closed);
    _points = std::move(otherPath._points);
    _length = std::move(otherPath._length);
  }

  return *this;
}

Path2::~Path2()
{
}

Path2 Path2::copy() const
{
  return Path2(*this);
}

unique_ptr_t<Path2> Path2::clone() const
{
  return std::make_unique<Path2>(*this);
}

std::ostream& operator<<(std::ostream& os, const Path2& path)
{
  os << "{\"Points\":[";
  if (path._points.size() > 0) {
    for (unsigned int i = 0; i < path._points.size() - 1; ++i) {
      os << path._points[i] << ",";
    }
    os << path._points.back();
  }
  os << "],\"Length\":" << path._length << "}";
  return os;
}

Path2& Path2::addLineTo(float x, float y)
{
  if (closed) {
    return *this;
  }
  const Vector2 newPoint(x, y);
  const Vector2 previousPoint = _points.back();
  _points.emplace_back(newPoint);
  _length += newPoint.subtract(previousPoint).length();
  return *this;
}

Path2& Path2::addArcTo(float midX, float midY, float endX, float endY,
                       unsigned int numberOfSegments)
{
  if (closed) {
    return *this;
  }
  const Vector2 startPoint = _points.back();
  const Vector2 midPoint(midX, midY);
  const Vector2 endPoint(endX, endY);

  const Arc2 arc(startPoint, midPoint, endPoint);

  float increment = arc.angle.radians() / static_cast<float>(numberOfSegments);
  if (arc.orientation == Orientation::CW) {
    increment *= -1;
  }
  float currentAngle = arc.startAngle.radians() + increment;

  float x = 0.f, y = 0.f;
  for (unsigned int i = 0; i < numberOfSegments; ++i) {
    x = std::cos(currentAngle) * arc.radius + arc.centerPoint.x;
    y = std::sin(currentAngle) * arc.radius + arc.centerPoint.y;
    addLineTo(x, y);
    currentAngle += increment;
  }
  return *this;
}

Path2& Path2::close()
{
  closed = true;
  return *this;
}

float Path2::length() const
{
  float result = _length;

  if (!closed) {
    const Vector2 lastPoint  = _points[_points.size() - 1];
    const Vector2 firstPoint = _points[0];
    result += (firstPoint.subtract(lastPoint).length());
  }

  return result;
}

vector_t<Vector2>& Path2::getPoints()
{
  return _points;
}

const vector_t<Vector2>& Path2::getPoints() const
{
  return _points;
}

Vector2 Path2::getPointAtLengthPosition(float normalizedLengthPosition) const
{
  if (normalizedLengthPosition < 0 || normalizedLengthPosition > 1) {
    return Vector2::Zero();
  }

  const float lengthPosition = normalizedLengthPosition * length();

  float previousOffset = 0;
  for (unsigned int i = 0; i < _points.size(); ++i) {
    size_t j = (i + 1) % _points.size();

    const Vector2 a = _points[i];
    const Vector2 b = _points[j];
    Vector2 bToA    = b.subtract(a);

    const float nextOffset = (bToA.length() + previousOffset);
    if (lengthPosition >= previousOffset && lengthPosition <= nextOffset) {
      const Vector2 dir       = bToA.normalize();
      const float localOffset = lengthPosition - previousOffset;

      return Vector2(a.x + (dir.x * localOffset), //
                     a.y + (dir.y * localOffset));
    }
    previousOffset = nextOffset;
  }

  return Vector2::Zero();
}

Path2 Path2::StartingAt(float x, float y)
{
  return Path2(x, y);
}

} // end of namespace BABYLON
