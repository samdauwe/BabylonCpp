#include <babylon/gamepads/xbox_360_pad.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

Xbox360Pad::Xbox360Pad(const std::string& iId, int iIndex,
                       const IBrowserGamepadPtr& gamepad, bool xboxOne)
    : Gamepad(iId, iIndex, gamepad, 0, 1, 2, 3)
    , leftTrigger{this, &Xbox360Pad::get_leftTrigger,
                  &Xbox360Pad::set_leftTrigger}
    , rightTrigger{this, &Xbox360Pad::get_rightTrigger,
                   &Xbox360Pad::set_rightTrigger}
    , buttonA{this, &Xbox360Pad::get_buttonA, &Xbox360Pad::set_buttonA}
    , buttonB{this, &Xbox360Pad::get_buttonB, &Xbox360Pad::set_buttonB}
    , buttonX{this, &Xbox360Pad::get_buttonX, &Xbox360Pad::set_buttonX}
    , buttonY{this, &Xbox360Pad::get_buttonY, &Xbox360Pad::set_buttonY}
    , buttonStart{this, &Xbox360Pad::get_buttonStart,
                  &Xbox360Pad::set_buttonStart}
    , buttonBack{this, &Xbox360Pad::get_buttonBack, &Xbox360Pad::set_buttonBack}
    , buttonLB{this, &Xbox360Pad::get_buttonLB, &Xbox360Pad::set_buttonLB}
    , buttonRB{this, &Xbox360Pad::get_buttonRB, &Xbox360Pad::set_buttonRB}
    , buttonLeftStick{this, &Xbox360Pad::get_buttonLeftStick,
                      &Xbox360Pad::set_buttonLeftStick}
    , buttonRightStick{this, &Xbox360Pad::get_buttonRightStick,
                       &Xbox360Pad::set_buttonRightStick}
    , dPadUp{this, &Xbox360Pad::get_dPadUp, &Xbox360Pad::set_dPadUp}
    , dPadDown{this, &Xbox360Pad::get_dPadDown, &Xbox360Pad::set_dPadDown}
    , dPadLeft{this, &Xbox360Pad::get_dPadLeft, &Xbox360Pad::set_dPadLeft}
    , dPadRight{this, &Xbox360Pad::get_dPadRight, &Xbox360Pad::set_dPadRight}
    , _leftTrigger{0}
    , _rightTrigger{0}
    , _buttonA{0}
    , _buttonB{0}
    , _buttonX{0}
    , _buttonY{0}
    , _buttonBack{0}
    , _buttonStart{0}
    , _buttonLB{0}
    , _buttonRB{0}
    , _buttonLeftStick{0}
    , _buttonRightStick{0}
    , _dPadUp{0}
    , _dPadDown{0}
    , _dPadLeft{0}
    , _dPadRight{0}
    , _isXboxOnePad{xboxOne}
{
  type = Gamepad::XBOX;
}

Xbox360Pad::~Xbox360Pad()
{
}

void Xbox360Pad::setOnlefttriggerchanged(
  const std::function<void(float value)>& callback)
{
  _onlefttriggerchanged = callback;
}

void Xbox360Pad::onrighttriggerchanged(
  const std::function<void(float value)>& callback)
{
  _onrighttriggerchanged = callback;
}

float Xbox360Pad::get_leftTrigger() const
{
  return _leftTrigger;
}

void Xbox360Pad::set_leftTrigger(float newValue)
{
  if (_onlefttriggerchanged
      && !stl_util::almost_equal(_leftTrigger, newValue)) {
    _onlefttriggerchanged(newValue);
  }
  _leftTrigger = newValue;
}

float Xbox360Pad::get_rightTrigger() const
{
  return _rightTrigger;
}

void Xbox360Pad::set_rightTrigger(float newValue)
{
  if (_onrighttriggerchanged
      && !stl_util::almost_equal(_rightTrigger, newValue)) {
    _onrighttriggerchanged(newValue);
  }
  _rightTrigger = newValue;
}

void Xbox360Pad::onbuttondown(
  const std::function<void(Xbox360Button buttonPressed)>& callback)
{
  _onbuttondown = callback;
}

void Xbox360Pad::onbuttonup(
  const std::function<void(Xbox360Button buttonReleased)>& callback)
{
  _onbuttonup = callback;
}

void Xbox360Pad::ondpaddown(
  const std::function<void(Xbox360Dpad dPadPressed)>& callback)
{
  _ondpaddown = callback;
}

void Xbox360Pad::ondpadup(
  const std::function<void(Xbox360Dpad dPadReleased)>& callback)
{
  _ondpadup = callback;
}

unsigned int Xbox360Pad::_setButtonValue(unsigned int newValue,
                                         unsigned int currentValue,
                                         Xbox360Button buttonType)
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

unsigned int Xbox360Pad::_setDPadValue(unsigned int newValue,
                                       unsigned int currentValue,
                                       Xbox360Dpad buttonType)
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

unsigned int Xbox360Pad::get_buttonA() const
{
  return _buttonA;
}

void Xbox360Pad::set_buttonA(unsigned int value)
{
  _buttonA = _setButtonValue(value, _buttonA, Xbox360Button::A);
}

unsigned int Xbox360Pad::get_buttonB() const
{
  return _buttonB;
}

void Xbox360Pad::set_buttonB(unsigned int value)
{
  _buttonB = _setButtonValue(value, _buttonB, Xbox360Button::B);
}

unsigned int Xbox360Pad::get_buttonX() const
{
  return _buttonX;
}

void Xbox360Pad::set_buttonX(unsigned int value)
{
  _buttonX = _setButtonValue(value, _buttonX, Xbox360Button::X);
}

unsigned int Xbox360Pad::get_buttonY() const
{
  return _buttonY;
}

void Xbox360Pad::set_buttonY(unsigned int value)
{
  _buttonY = _setButtonValue(value, _buttonY, Xbox360Button::Y);
}

unsigned int Xbox360Pad::get_buttonStart() const
{
  return _buttonStart;
}

void Xbox360Pad::set_buttonStart(unsigned int value)
{
  _buttonStart = _setButtonValue(value, _buttonStart, Xbox360Button::Start);
}

unsigned int Xbox360Pad::get_buttonBack() const
{
  return _buttonBack;
}

void Xbox360Pad::set_buttonBack(unsigned int value)
{
  _buttonBack = _setButtonValue(value, _buttonBack, Xbox360Button::Back);
}

unsigned int Xbox360Pad::get_buttonLB() const
{
  return _buttonLB;
}

void Xbox360Pad::set_buttonLB(unsigned int value)
{
  _buttonLB = _setButtonValue(value, _buttonLB, Xbox360Button::LB);
}

unsigned int Xbox360Pad::get_buttonRB() const
{
  return _buttonRB;
}

void Xbox360Pad::set_buttonRB(unsigned int value)
{
  _buttonRB = _setButtonValue(value, _buttonRB, Xbox360Button::RB);
}

unsigned int Xbox360Pad::get_buttonLeftStick() const
{
  return _buttonLeftStick;
}

void Xbox360Pad::set_buttonLeftStick(unsigned int value)
{
  _buttonLeftStick
    = _setButtonValue(value, _buttonLeftStick, Xbox360Button::LeftStick);
}

unsigned int Xbox360Pad::get_buttonRightStick() const
{
  return _buttonRightStick;
}

void Xbox360Pad::set_buttonRightStick(unsigned int value)
{
  _buttonRightStick
    = _setButtonValue(value, _buttonRightStick, Xbox360Button::RightStick);
}

unsigned int Xbox360Pad::get_dPadUp() const
{
  return _dPadUp;
}

void Xbox360Pad::set_dPadUp(unsigned int value)
{
  _dPadUp = _setDPadValue(value, _dPadUp, Xbox360Dpad::Up);
}

unsigned int Xbox360Pad::get_dPadDown() const
{
  return _dPadDown;
}

void Xbox360Pad::set_dPadDown(unsigned int value)
{
  _dPadDown = _setDPadValue(value, _dPadDown, Xbox360Dpad::Down);
}

unsigned int Xbox360Pad::get_dPadLeft() const
{
  return _dPadLeft;
}

void Xbox360Pad::set_dPadLeft(unsigned int value)
{
  _dPadLeft = _setDPadValue(value, _dPadLeft, Xbox360Dpad::Left);
}

unsigned int Xbox360Pad::get_dPadRight() const
{
  return _dPadRight;
}

void Xbox360Pad::set_dPadRight(unsigned int value)
{
  _dPadRight = _setDPadValue(value, _dPadRight, Xbox360Dpad::Right);
}

void Xbox360Pad::update()
{
  Gamepad::update();
  if (_isXboxOnePad) {
    buttonA          = browserGamepad->buttons[0].value;
    buttonB          = browserGamepad->buttons[1].value;
    buttonX          = browserGamepad->buttons[2].value;
    buttonY          = browserGamepad->buttons[3].value;
    buttonLB         = browserGamepad->buttons[4].value;
    buttonRB         = browserGamepad->buttons[5].value;
    leftTrigger      = browserGamepad->buttons[6].value;
    rightTrigger     = browserGamepad->buttons[7].value;
    buttonBack       = browserGamepad->buttons[8].value;
    buttonStart      = browserGamepad->buttons[9].value;
    buttonLeftStick  = browserGamepad->buttons[10].value;
    buttonRightStick = browserGamepad->buttons[11].value;
    dPadUp           = browserGamepad->buttons[12].value;
    dPadDown         = browserGamepad->buttons[13].value;
    dPadLeft         = browserGamepad->buttons[14].value;
    dPadRight        = browserGamepad->buttons[15].value;
  }
  else {
    buttonA          = browserGamepad->buttons[0].value;
    buttonB          = browserGamepad->buttons[1].value;
    buttonX          = browserGamepad->buttons[2].value;
    buttonY          = browserGamepad->buttons[3].value;
    buttonLB         = browserGamepad->buttons[4].value;
    buttonRB         = browserGamepad->buttons[5].value;
    leftTrigger      = static_cast<float>(browserGamepad->buttons[6].value);
    rightTrigger     = static_cast<float>(browserGamepad->buttons[7].value);
    buttonBack       = browserGamepad->buttons[8].value;
    buttonStart      = browserGamepad->buttons[9].value;
    buttonLeftStick  = browserGamepad->buttons[10].value;
    buttonRightStick = browserGamepad->buttons[11].value;
    dPadUp           = browserGamepad->buttons[12].value;
    dPadDown         = browserGamepad->buttons[13].value;
    dPadLeft         = browserGamepad->buttons[14].value;
    dPadRight        = browserGamepad->buttons[15].value;
  }
}

void Xbox360Pad::dispose()
{
  Gamepad::dispose();
  onButtonDownObservable.clear();
  onButtonUpObservable.clear();
  onPadDownObservable.clear();
  onPadUpObservable.clear();
}

} // end of namespace BABYLON
