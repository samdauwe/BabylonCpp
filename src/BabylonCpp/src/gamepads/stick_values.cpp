#include <babylon/gamepads/stick_values.h>

#include <utility>

namespace BABYLON {

StickValues::StickValues() : x{0.f}, y{0.f}
{
}

StickValues::StickValues(float iX, float iY) : x{iX}, y{iY}
{
}

StickValues::StickValues(const StickValues& other) = default;

StickValues::StickValues(StickValues&& other) = default;

StickValues& StickValues::operator=(const StickValues& other) = default;

StickValues& StickValues::operator=(StickValues&& other) = default;

StickValues::~StickValues() = default;

} // end of namespace BABYLON
