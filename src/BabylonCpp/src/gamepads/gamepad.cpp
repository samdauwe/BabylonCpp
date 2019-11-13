#include <babylon/gamepads/gamepad.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

Gamepad::Gamepad(const std::string& iId, int iIndex, const IBrowserGamepadPtr& iBrowserGamepad,
                 unsigned int leftStickX, unsigned int leftStickY, unsigned int rightStickX,
                 unsigned int rightStickY)
    : id{iId}
    , index{iIndex}
    , browserGamepad{iBrowserGamepad}
    , type{Gamepad::GAMEPAD}
    , _isConnected{true}
    , isConnected{this, &Gamepad::get_isConnected}
    , leftStick{this, &Gamepad::get_leftStick, &Gamepad::set_leftStick}
    , rightStick{this, &Gamepad::get_rightStick, &Gamepad::set_rightStick}
    , _invertLeftStickY{false}
    , _leftStick{StickValues(0.f, 0.f)}
    , _rightStick{StickValues(0.f, 0.f)}
    , _leftStickAxisX{leftStickX}
    , _leftStickAxisY{leftStickY}
    , _rightStickAxisX{rightStickX}
    , _rightStickAxisY{rightStickY}
{
  if (browserGamepad->axes.size() >= 2) {
    _leftStick
      = StickValues(browserGamepad->axes[_leftStickAxisX], browserGamepad->axes[_leftStickAxisY]);
  }
  if (browserGamepad->axes.size() >= 4) {
    _rightStick
      = StickValues(browserGamepad->axes[_rightStickAxisX], browserGamepad->axes[_rightStickAxisY]);
  }
}

Gamepad::~Gamepad() = default;

bool Gamepad::get_isConnected() const
{
  return _isConnected;
}

void Gamepad::onleftstickchanged(const std::function<void(const StickValues& values)>& callback)
{
  _onleftstickchanged = callback;
}

void Gamepad::onrightstickchanged(const std::function<void(const StickValues& values)>& callback)
{
  _onrightstickchanged = callback;
}

std::optional<StickValues>& Gamepad::get_leftStick()
{
  return _leftStick;
}

void Gamepad::set_leftStick(const std::optional<StickValues>& newValues)
{
  if (_onleftstickchanged
      && (!stl_util::almost_equal((*_leftStick).x, (*newValues).x)
          || !stl_util::almost_equal((*_leftStick).y, (*newValues).y))) {
    _onleftstickchanged(*newValues);
  }
  _leftStick = newValues;
}

std::optional<StickValues>& Gamepad::get_rightStick()
{
  return _rightStick;
}

void Gamepad::set_rightStick(const std::optional<StickValues>& newValues)
{
  if (_onrightstickchanged
      && (!stl_util::almost_equal((*_rightStick).x, (*newValues).x)
          || !stl_util::almost_equal((*_rightStick).y, (*newValues).y))) {
    _onrightstickchanged(*newValues);
  }
  _rightStick = newValues;
}

void Gamepad::update()
{
  if (_leftStick) {
    if (_invertLeftStickY) {
      _leftStick = StickValues(browserGamepad->axes[_leftStickAxisX],
                               browserGamepad->axes[_leftStickAxisY] * -1.f);
    }
    else {
      _leftStick
        = StickValues(browserGamepad->axes[_leftStickAxisX], browserGamepad->axes[_leftStickAxisY]);
    }
  }
  if (_rightStick) {
    _rightStick
      = StickValues(browserGamepad->axes[_rightStickAxisX], browserGamepad->axes[_rightStickAxisY]);
  }
}

void Gamepad::dispose()
{
}

} // end of namespace BABYLON
