#include <babylon/animations/path_cursor.h>

#include <babylon/core/logging.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

PathCursor::PathCursor(const Path2& _path) : path{_path}, value{0.f}
{
}

PathCursor::PathCursor(const PathCursor& otherPathCursor)
    : _onchange{otherPathCursor._onchange}
    , path{otherPathCursor.path}
    , value{otherPathCursor.value}
    , animations{otherPathCursor.animations}
{
}

PathCursor::PathCursor(PathCursor&& otherPathCursor)
    : _onchange{std::move(otherPathCursor._onchange)}
    , path{std::move(otherPathCursor.path)}
    , value{std::move(otherPathCursor.value)}
    , animations{std::move(otherPathCursor.animations)}
{
}

PathCursor& PathCursor::operator=(const PathCursor& otherPathCursor)
{
  if (&otherPathCursor != this) {
    _onchange  = otherPathCursor._onchange;
    path       = otherPathCursor.path;
    value      = otherPathCursor.value;
    animations = otherPathCursor.animations;
  }

  return *this;
}

PathCursor& PathCursor::operator=(PathCursor&& otherPathCursor)
{
  if (&otherPathCursor != this) {
    _onchange  = std::move(otherPathCursor._onchange);
    path       = std::move(otherPathCursor.path);
    value      = std::move(otherPathCursor.value);
    animations = std::move(otherPathCursor.animations);
  }

  return *this;
}

PathCursor::~PathCursor()
{
}

Vector3 PathCursor::getPoint() const
{
  auto point = path.getPointAtLengthPosition(value);
  return Vector3(point.x, 0.f, point.y);
}

PathCursor& PathCursor::moveAhead(float step)
{
  move(step);

  return *this;
}

PathCursor& PathCursor::moveBack(float step)
{
  move(-step);

  return *this;
}

PathCursor& PathCursor::move(float step)
{
  if (std::abs(step) > 1.f) {
    BABYLON_LOG_ERROR("PathCursor", "Step size should be less than 1.")
    return *this;
  }

  value += step;
  ensureLimits();
  raiseOnChange();

  return *this;
}

PathCursor PathCursor::ensureLimits()
{
  while (value > 1.f) {
    value -= 1.f;
  }
  while (value < 0.f) {
    value += 1.f;
  }

  return *this;
}

// used by animation engine
PathCursor& PathCursor::raiseOnChange()
{
  for (auto& f : _onchange) {
    f(*this);
  }

  return *this;
}

PathCursor&
PathCursor::onchange(const std::function<void(const PathCursor& cursor)>& f)
{
  _onchange.emplace_back(f);

  return *this;
}

} // end of namespace BABYLON
