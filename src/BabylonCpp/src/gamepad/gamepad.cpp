#include <babylon/gamepad/gamepad.h>

#include <babylon/gamepad/stick_values.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

Gamepad::Gamepad(const std::string& iId, int iIndex,
                 const std::shared_ptr<IBrowserGamepad>& browserGamepad,
                 unsigned int leftStickX, unsigned int leftStickY,
                 unsigned int rightStickX, unsigned int rightStickY)
    : id{iId}
    , index{iIndex}
    , type{Gamepad::GAMEPAD}
    , _browserGamepad{browserGamepad}
    , _leftStickAxisX{leftStickX}
    , _leftStickAxisY{leftStickY}
    , _rightStickAxisX{rightStickX}
    , _rightStickAxisY{rightStickY}
{
  if (browserGamepad->axes.size() >= 2) {
    _leftStick = StickValues(browserGamepad->axes[_leftStickAxisX],
                             browserGamepad->axes[_leftStickAxisY]);
  }
  if (browserGamepad->axes.size() >= 4) {
    _rightStick = StickValues(browserGamepad->axes[_rightStickAxisX],
                              browserGamepad->axes[_rightStickAxisY]);
  }
}

Gamepad::~Gamepad()
{
}

void Gamepad::setOnleftstickchanged(
  const ::std::function<void(const StickValues& values)>& callback)
{
  _onleftstickchanged = callback;
}

void Gamepad::setOnrightstickchanged(
  const ::std::function<void(const StickValues& values)>& callback)
{
  _onrightstickchanged = callback;
}

StickValues& Gamepad::leftStick()
{
  return _leftStick;
}

const StickValues& Gamepad::leftStick() const
{
  return _leftStick;
}

void Gamepad::setLeftStick(const StickValues& newValues)
{
  if (_onleftstickchanged
      && (_leftStick.x != newValues.x || _leftStick.y != newValues.y)) {
    _onleftstickchanged(newValues);
  }
  _leftStick = newValues;
}

StickValues& Gamepad::rightStick()
{
  return _rightStick;
}

const StickValues& Gamepad::rightStick() const
{
  return _rightStick;
}

void Gamepad::setRightStick(const StickValues& newValues)
{
  if (_onrightstickchanged
      && (_rightStick.x != newValues.x || _rightStick.y != newValues.y)) {
    _onrightstickchanged(newValues);
  }
  _rightStick = newValues;
}

void Gamepad::update()
{
  if (_leftStick) {
    _leftStick = StickValues(_browserGamepad->axes[_leftStickAxisX],
                             _browserGamepad->axes[_leftStickAxisY]);
  }
  if (_rightStick) {
    _rightStick = StickValues(_browserGamepad->axes[_rightStickAxisX],
                              _browserGamepad->axes[_rightStickAxisY]);
  }
}

void Gamepad::dispose()
{
}

} // end of namespace BABYLON
