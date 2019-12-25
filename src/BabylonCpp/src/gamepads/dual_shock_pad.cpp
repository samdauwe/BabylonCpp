#include <babylon/gamepads/dual_shock_pad.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

DualShockPad::DualShockPad(const std::string& iId, int iIndex, const IBrowserGamepadPtr& gamepad)
    : Gamepad{String::replace(iId, "STANDARD GAMEPAD", "SONY PLAYSTATION DUALSHOCK"),
              iIndex,
              gamepad,
              0,
              1,
              2,
              3}
    , leftTrigger{this, &DualShockPad::get_leftTrigger, &DualShockPad::set_leftTrigger}
    , rightTrigger{this, &DualShockPad::get_rightTrigger, &DualShockPad::set_rightTrigger}
    , buttonCross{this, &DualShockPad::get_buttonCross, &DualShockPad::set_buttonCross}
    , buttonCircle{this, &DualShockPad::get_buttonCircle, &DualShockPad::set_buttonCircle}
    , buttonSquare{this, &DualShockPad::get_buttonSquare, &DualShockPad::set_buttonSquare}
    , buttonTriangle{this, &DualShockPad::get_buttonTriangle, &DualShockPad::set_buttonTriangle}
    , buttonOptions{this, &DualShockPad::get_buttonOptions, &DualShockPad::set_buttonOptions}
    , buttonShare{this, &DualShockPad::get_buttonShare, &DualShockPad::set_buttonShare}
    , buttonL1{this, &DualShockPad::get_buttonL1, &DualShockPad::set_buttonL1}
    , buttonR1{this, &DualShockPad::get_buttonR1, &DualShockPad::set_buttonR1}
    , buttonLeftStick{this, &DualShockPad::get_buttonLeftStick, &DualShockPad::set_buttonLeftStick}
    , buttonRightStick{this, &DualShockPad::get_buttonRightStick,
                       &DualShockPad::set_buttonRightStick}
    , dPadUp{this, &DualShockPad::get_dPadUp, &DualShockPad::set_dPadUp}
    , dPadDown{this, &DualShockPad::get_dPadDown, &DualShockPad::set_dPadDown}
    , dPadLeft{this, &DualShockPad::get_dPadLeft, &DualShockPad::set_dPadLeft}
    , dPadRight{this, &DualShockPad::get_dPadRight, &DualShockPad::set_dPadRight}
    , _leftTrigger{0.f}
    , _rightTrigger{0.f}
    , _onlefttriggerchanged{nullptr}
    , _onrighttriggerchanged{nullptr}
    , _onbuttondown{nullptr}
    , _onbuttonup{nullptr}
    , _ondpaddown{nullptr}
    , _ondpadup{nullptr}
    , _buttonCross{0}
    , _buttonCircle{0}
    , _buttonSquare{0}
    , _buttonTriangle{0}
    , _buttonShare{0}
    , _buttonOptions{0}
    , _buttonL1{0}
    , _buttonR1{0}
    , _buttonLeftStick{0}
    , _buttonRightStick{0}
    , _dPadUp{0}
    , _dPadDown{0}
    , _dPadLeft{0}
    , _dPadRight{0}
{
  type = Gamepad::DUALSHOCK;
}

DualShockPad::~DualShockPad() = default;

void DualShockPad::onrighttriggerchanged(const std::function<void(float value)>& callback)
{
  _onrighttriggerchanged = callback;
}

float DualShockPad::get_leftTrigger() const
{
  return _leftTrigger;
}

void DualShockPad::set_leftTrigger(float newValue)
{
  if (_onlefttriggerchanged && !stl_util::almost_equal(_leftTrigger, newValue)) {
    _onlefttriggerchanged(newValue);
  }
  _leftTrigger = newValue;
}

float DualShockPad::get_rightTrigger() const
{
  return _rightTrigger;
}

void DualShockPad::set_rightTrigger(float newValue)
{
  if (_onrighttriggerchanged && !stl_util::almost_equal(_rightTrigger, newValue)) {
    _onrighttriggerchanged(newValue);
  }
  _rightTrigger = newValue;
}

void DualShockPad::onbuttondown(
  const std::function<void(DualShockPad::DualShockButton buttonPressed)>& callback)
{
  _onbuttondown = callback;
}

void DualShockPad::onbuttonup(
  const std::function<void(DualShockPad::DualShockButton buttonReleased)>& callback)
{
  _onbuttonup = callback;
}

void DualShockPad::ondpaddown(
  const std::function<void(DualShockPad::DualShockDpad dPadPressed)>& callback)
{
  _ondpaddown = callback;
}

void DualShockPad::ondpadup(
  const std::function<void(DualShockPad::DualShockDpad dPadReleased)>& callback)
{
  _ondpadup = callback;
}

unsigned int DualShockPad::_setButtonValue(unsigned int newValue, unsigned int currentValue,
                                           DualShockButton buttonType)
{
  if (newValue != currentValue) {
    if (newValue == 1) {
      if (_onbuttondown) {
        _onbuttondown(buttonType);
      }

      onButtonDownObservable.notifyObservers(&buttonType);
    }
    if (newValue == 0) {

      if (_onbuttonup) {
        _onbuttonup(buttonType);
      }

      onButtonUpObservable.notifyObservers(&buttonType);
    }
  }
  return newValue;
}

unsigned int DualShockPad::_setDPadValue(unsigned int newValue, unsigned int currentValue,
                                         DualShockDpad buttonType)
{
  if (newValue != currentValue) {
    if (newValue == 1) {
      if (_ondpaddown) {
        _ondpaddown(buttonType);
      }

      onPadDownObservable.notifyObservers(&buttonType);
    }
    if (newValue == 0) {
      if (_ondpadup) {
        _ondpadup(buttonType);
      }

      onPadUpObservable.notifyObservers(&buttonType);
    }
  }
  return newValue;
}

unsigned int DualShockPad::get_buttonCross() const
{
  return _buttonCross;
}

void DualShockPad::set_buttonCross(unsigned int value)
{
  _buttonCross = _setButtonValue(value, _buttonCross, DualShockButton::Cross);
}

unsigned int DualShockPad::get_buttonCircle() const
{
  return _buttonCircle;
}

void DualShockPad::set_buttonCircle(unsigned int value)
{
  _buttonCircle = _setButtonValue(value, _buttonCircle, DualShockButton::Circle);
}

unsigned int DualShockPad::get_buttonSquare() const
{
  return _buttonSquare;
}

void DualShockPad::set_buttonSquare(unsigned int value)
{
  _buttonSquare = _setButtonValue(value, _buttonSquare, DualShockButton::Square);
}

unsigned int DualShockPad::get_buttonTriangle() const
{
  return _buttonTriangle;
}

void DualShockPad::set_buttonTriangle(unsigned int value)
{
  _buttonTriangle = _setButtonValue(value, _buttonTriangle, DualShockButton::Triangle);
}

unsigned int DualShockPad::get_buttonOptions() const
{
  return _buttonOptions;
}

void DualShockPad::set_buttonOptions(unsigned int value)
{
  _buttonOptions = _setButtonValue(value, _buttonOptions, DualShockButton::Options);
}

unsigned int DualShockPad::get_buttonShare() const
{
  return _buttonShare;
}

void DualShockPad::set_buttonShare(unsigned int value)
{
  _buttonShare = _setButtonValue(value, _buttonShare, DualShockButton::Share);
}

unsigned int DualShockPad::get_buttonL1() const
{
  return _buttonL1;
}

void DualShockPad::set_buttonL1(unsigned int value)
{
  _buttonL1 = _setButtonValue(value, _buttonL1, DualShockButton::L1);
}

unsigned int DualShockPad::get_buttonR1() const
{
  return _buttonR1;
}

void DualShockPad::set_buttonR1(unsigned int value)
{
  _buttonR1 = _setButtonValue(value, _buttonR1, DualShockButton::R1);
}

unsigned int DualShockPad::get_buttonLeftStick() const
{
  return _buttonLeftStick;
}

void DualShockPad::set_buttonLeftStick(unsigned int value)
{
  _buttonLeftStick = _setButtonValue(value, _buttonLeftStick, DualShockButton::LeftStick);
}

unsigned int DualShockPad::get_buttonRightStick() const
{
  return _buttonRightStick;
}

void DualShockPad::set_buttonRightStick(unsigned int value)
{
  _buttonRightStick = _setButtonValue(value, _buttonRightStick, DualShockButton::RightStick);
}

unsigned int DualShockPad::get_dPadUp() const
{
  return _dPadUp;
}

void DualShockPad::set_dPadUp(unsigned int value)
{
  _dPadUp = _setDPadValue(value, _dPadUp, DualShockDpad::Up);
}

unsigned int DualShockPad::get_dPadDown() const
{
  return _dPadDown;
}

void DualShockPad::set_dPadDown(unsigned int value)
{
  _dPadDown = _setDPadValue(value, _dPadDown, DualShockDpad::Down);
}

unsigned int DualShockPad::get_dPadLeft() const
{
  return _dPadLeft;
}

void DualShockPad::set_dPadLeft(unsigned int value)
{
  _dPadLeft = _setDPadValue(value, _dPadLeft, DualShockDpad::Left);
}

unsigned int DualShockPad::get_dPadRight() const
{
  return _dPadRight;
}

void DualShockPad::set_dPadRight(unsigned int value)
{
  _dPadRight = _setDPadValue(value, _dPadRight, DualShockDpad::Right);
}

void DualShockPad::update()
{
  Gamepad::update();
  buttonCross      = browserGamepad->buttons[0].value;
  buttonCircle     = browserGamepad->buttons[1].value;
  buttonSquare     = browserGamepad->buttons[2].value;
  buttonTriangle   = browserGamepad->buttons[3].value;
  buttonL1         = browserGamepad->buttons[4].value;
  buttonR1         = browserGamepad->buttons[5].value;
  leftTrigger      = static_cast<float>(browserGamepad->buttons[6].value);
  rightTrigger     = static_cast<float>(browserGamepad->buttons[7].value);
  buttonShare      = browserGamepad->buttons[8].value;
  buttonOptions    = browserGamepad->buttons[9].value;
  buttonLeftStick  = browserGamepad->buttons[10].value;
  buttonRightStick = browserGamepad->buttons[11].value;
  dPadUp           = browserGamepad->buttons[12].value;
  dPadDown         = browserGamepad->buttons[13].value;
  dPadLeft         = browserGamepad->buttons[14].value;
  dPadRight        = browserGamepad->buttons[15].value;
}

void DualShockPad::dispose()
{
  Gamepad::dispose();
  onButtonDownObservable.clear();
  onButtonUpObservable.clear();
  onPadDownObservable.clear();
  onPadUpObservable.clear();
}

} // end of namespace BABYLON
