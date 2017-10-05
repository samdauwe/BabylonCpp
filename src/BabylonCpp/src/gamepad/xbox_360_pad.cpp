#include <babylon/gamepad/xbox_360_pad.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

Xbox360Pad::Xbox360Pad(const string_t& id, int index,
                       const shared_ptr_t<IBrowserGamepad>& gamepad,
                       bool xboxOne)
    : Gamepad(id, index, gamepad, 0, 1, (xboxOne ? 3 : 2), (xboxOne ? 4 : 3))
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
  const ::std::function<void(float value)>& callback)
{
  _onlefttriggerchanged = callback;
}

void Xbox360Pad::onrighttriggerchanged(
  const ::std::function<void(float value)>& callback)
{
  _onrighttriggerchanged = callback;
}

float Xbox360Pad::leftTrigger() const
{
  return _leftTrigger;
}

void Xbox360Pad::setLeftTrigger(float newValue)
{
  if (_onlefttriggerchanged
      && !stl_util::almost_equal(_leftTrigger, newValue)) {
    _onlefttriggerchanged(newValue);
  }
  _leftTrigger = newValue;
}

float Xbox360Pad::rightTrigger() const
{
  return _rightTrigger;
}

void Xbox360Pad::setRightTrigger(float newValue)
{
  if (_onrighttriggerchanged
      && !stl_util::almost_equal(_rightTrigger, newValue)) {
    _onrighttriggerchanged(newValue);
  }
  _rightTrigger = newValue;
}

void Xbox360Pad::onbuttondown(
  const ::std::function<void(Xbox360Button buttonPressed)>& callback)
{
  _onbuttondown = callback;
}

void Xbox360Pad::onbuttonup(
  const ::std::function<void(Xbox360Button buttonReleased)>& callback)
{
  _onbuttonup = callback;
}

void Xbox360Pad::ondpaddown(
  const ::std::function<void(Xbox360Dpad dPadPressed)>& callback)
{
  _ondpaddown = callback;
}

void Xbox360Pad::ondpadup(
  const ::std::function<void(Xbox360Dpad dPadReleased)>& callback)
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

unsigned int Xbox360Pad::buttonA() const
{
  return _buttonA;
}

void Xbox360Pad::setButtonA(unsigned int value)
{
  _buttonA = _setButtonValue(value, _buttonA, Xbox360Button::A);
}

unsigned int Xbox360Pad::buttonB() const
{
  return _buttonB;
}

void Xbox360Pad::setButtonB(unsigned int value)
{
  _buttonB = _setButtonValue(value, _buttonB, Xbox360Button::B);
}

unsigned int Xbox360Pad::buttonX() const
{
  return _buttonX;
}

void Xbox360Pad::setButtonX(unsigned int value)
{
  _buttonX = _setButtonValue(value, _buttonX, Xbox360Button::X);
}

unsigned int Xbox360Pad::buttonY() const
{
  return _buttonY;
}

void Xbox360Pad::setButtonY(unsigned int value)
{
  _buttonY = _setButtonValue(value, _buttonY, Xbox360Button::Y);
}

unsigned int Xbox360Pad::buttonStart() const
{
  return _buttonStart;
}

void Xbox360Pad::setButtonStart(unsigned int value)
{
  _buttonStart = _setButtonValue(value, _buttonStart, Xbox360Button::Start);
}

unsigned int Xbox360Pad::buttonBack() const
{
  return _buttonBack;
}

void Xbox360Pad::setButtonBack(unsigned int value)
{
  _buttonBack = _setButtonValue(value, _buttonBack, Xbox360Button::Back);
}

unsigned int Xbox360Pad::buttonLB() const
{
  return _buttonLB;
}

void Xbox360Pad::setButtonLB(unsigned int value)
{
  _buttonLB = _setButtonValue(value, _buttonLB, Xbox360Button::LB);
}

unsigned int Xbox360Pad::buttonRB() const
{
  return _buttonRB;
}

void Xbox360Pad::setButtonRB(unsigned int value)
{
  _buttonRB = _setButtonValue(value, _buttonRB, Xbox360Button::RB);
}

unsigned int Xbox360Pad::buttonLeftStick() const
{
  return _buttonLeftStick;
}

void Xbox360Pad::setButtonLeftStick(unsigned int value)
{
  _buttonLeftStick
    = _setButtonValue(value, _buttonLeftStick, Xbox360Button::LeftStick);
}

unsigned int Xbox360Pad::buttonRightStick() const
{
  return _buttonRightStick;
}

void Xbox360Pad::setButtonRightStick(unsigned int value)
{
  _buttonRightStick
    = _setButtonValue(value, _buttonRightStick, Xbox360Button::RightStick);
}

unsigned int Xbox360Pad::dPadUp() const
{
  return _dPadUp;
}

void Xbox360Pad::setDPadUp(unsigned int value)
{
  _dPadUp = _setDPadValue(value, _dPadUp, Xbox360Dpad::Up);
}

unsigned int Xbox360Pad::dPadDown() const
{
  return _dPadDown;
}

void Xbox360Pad::setDPadDown(unsigned int value)
{
  _dPadDown = _setDPadValue(value, _dPadDown, Xbox360Dpad::Down);
}

unsigned int Xbox360Pad::dPadLeft() const
{
  return _dPadLeft;
}

void Xbox360Pad::setDPadLeft(unsigned int value)
{
  _dPadLeft = _setDPadValue(value, _dPadLeft, Xbox360Dpad::Left);
}

unsigned int Xbox360Pad::dPadRight() const
{
  return _dPadRight;
}

void Xbox360Pad::setDPadRight(unsigned int value)
{
  _dPadRight = _setDPadValue(value, _dPadRight, Xbox360Dpad::Right);
}

void Xbox360Pad::update()
{
  Gamepad::update();
  if (_isXboxOnePad) {
    setButtonA(_browserGamepad->buttons[0].value);
    setButtonB(_browserGamepad->buttons[1].value);
    setButtonX(_browserGamepad->buttons[2].value);
    setButtonY(_browserGamepad->buttons[3].value);
    setButtonLB(_browserGamepad->buttons[4].value);
    setButtonRB(_browserGamepad->buttons[5].value);
    setLeftTrigger(_browserGamepad->axes[2]);
    setRightTrigger(_browserGamepad->axes[5]);
    setButtonBack(_browserGamepad->buttons[9].value);
    setButtonStart(_browserGamepad->buttons[8].value);
    setButtonLeftStick(_browserGamepad->buttons[6].value);
    setButtonRightStick(_browserGamepad->buttons[7].value);
    setDPadUp(_browserGamepad->buttons[11].value);
    setDPadDown(_browserGamepad->buttons[12].value);
    setDPadLeft(_browserGamepad->buttons[13].value);
    setDPadRight(_browserGamepad->buttons[14].value);
  }
  else {
    setButtonA(_browserGamepad->buttons[0].value);
    setButtonB(_browserGamepad->buttons[1].value);
    setButtonX(_browserGamepad->buttons[2].value);
    setButtonY(_browserGamepad->buttons[3].value);
    setButtonLB(_browserGamepad->buttons[4].value);
    setButtonRB(_browserGamepad->buttons[5].value);
    setLeftTrigger(_browserGamepad->buttons[6].value * 1.f);
    setRightTrigger(_browserGamepad->buttons[7].value * 1.f);
    setButtonBack(_browserGamepad->buttons[8].value);
    setButtonStart(_browserGamepad->buttons[9].value);
    setButtonLeftStick(_browserGamepad->buttons[10].value);
    setButtonRightStick(_browserGamepad->buttons[11].value);
    setDPadUp(_browserGamepad->buttons[12].value);
    setDPadDown(_browserGamepad->buttons[13].value);
    setDPadLeft(_browserGamepad->buttons[14].value);
    setDPadRight(_browserGamepad->buttons[15].value);
  }
}

} // end of namespace BABYLON
