#include <babylon/gamepad/gamepad.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

Gamepad::Gamepad(const string_t& iId, int iIndex,
                 const shared_ptr_t<IBrowserGamepad>& browserGamepad,
                 unsigned int leftStickX, unsigned int leftStickY,
                 unsigned int rightStickX, unsigned int rightStickY)
    : id{iId}
    , index{iIndex}
    , type{Gamepad::GAMEPAD}
    , _isConnected{true}
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

bool Gamepad::isConnected() const
{
  return _isConnected;
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

const StickValues& Gamepad::leftStick() const
{
  return *_leftStick;
}

void Gamepad::setLeftStick(const StickValues& newValues)
{
  if (_onleftstickchanged
      && (!stl_util::almost_equal((*_leftStick).x, newValues.x)
          || !stl_util::almost_equal((*_leftStick).y, newValues.y))) {
    _onleftstickchanged(newValues);
  }
  _leftStick = newValues;
}

const StickValues& Gamepad::rightStick() const
{
  return *_rightStick;
}

void Gamepad::setRightStick(const StickValues& newValues)
{
  if (_onrightstickchanged
      && (!stl_util::almost_equal((*_rightStick).x, newValues.x)
          || !stl_util::almost_equal((*_rightStick).y, newValues.y))) {
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
