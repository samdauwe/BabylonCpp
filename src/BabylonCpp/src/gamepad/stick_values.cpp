#include <babylon/gamepad/stick_values.h>

namespace BABYLON {

StickValues::StickValues(float iX, float iY) : x{iX}, y{iY}
{
}

StickValues::StickValues(const StickValues& other) : x{other.x}, y{other.y}
{
}

StickValues::StickValues(StickValues&& other)
    : x{std::move(other.x)}, y{std::move(other.y)}
{
}

StickValues& StickValues::operator=(const StickValues& other)
{
  if (&other != this) {
    x = other.x;
    y = other.y;
  }

  return *this;
}

StickValues& StickValues::operator=(StickValues&& other)
{
  if (&other != this) {
    x = std::move(other.x);
    y = std::move(other.y);
  }

  return *this;
}

StickValues::~StickValues()
{
}

} // end of namespace BABYLON
