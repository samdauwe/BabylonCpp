#ifndef BABYLON_GAMEPAD_XBOX_360_PAD_H
#define BABYLON_GAMEPAD_XBOX_360_PAD_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/gamepad.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Xbox360Pad : public Gamepad {

public:
  enum class Xbox360Button {
    A,
    B,
    X,
    Y,
    Start,
    Back,
    LB,
    RB,
    LeftStick,
    RightStick
  }; // end of enum class Xbox360Button

  enum class Xbox360Dpad {
    Up,
    Down,
    Left,
    Right
  }; // end of enum class Xbox360Dpad

public:
  Xbox360Pad(const string_t& id, int index,
             const shared_ptr_t<IBrowserGamepad>& gamepad,
             bool xboxOne = false);
  ~Xbox360Pad();

  void
  setOnlefttriggerchanged(const ::std::function<void(float value)>& callback);
  void
  onrighttriggerchanged(const ::std::function<void(float value)>& callback);
  float leftTrigger() const;
  void setLeftTrigger(float newValue);
  float rightTrigger() const;
  void setRightTrigger(float newValue);
  void onbuttondown(
    const ::std::function<void(Xbox360Button buttonPressed)>& callback);
  void onbuttonup(
    const ::std::function<void(Xbox360Button buttonReleased)>& callback);
  void
  ondpaddown(const ::std::function<void(Xbox360Dpad dPadPressed)>& callback);
  void
  ondpadup(const ::std::function<void(Xbox360Dpad dPadReleased)>& callback);
  unsigned int buttonA() const;
  void setButtonA(unsigned int value);
  unsigned int buttonB() const;
  void setButtonB(unsigned int value);
  unsigned int buttonX() const;
  void setButtonX(unsigned int value);
  unsigned int buttonY() const;
  void setButtonY(unsigned int value);
  unsigned int buttonStart() const;
  void setButtonStart(unsigned int value);
  unsigned int buttonBack() const;
  void setButtonBack(unsigned int value);
  unsigned int buttonLB() const;
  void setButtonLB(unsigned int value);
  unsigned int buttonRB() const;
  void setButtonRB(unsigned int value);
  unsigned int buttonLeftStick() const;
  void setButtonLeftStick(unsigned int value);
  unsigned int buttonRightStick() const;
  void setButtonRightStick(unsigned int value);
  unsigned int dPadUp() const;
  void setDPadUp(unsigned int value);
  unsigned int dPadDown() const;
  void setDPadDown(unsigned int value);
  unsigned int dPadLeft() const;
  void setDPadLeft(unsigned int value);
  unsigned int dPadRight() const;
  void setDPadRight(unsigned int value);
  void update() override;

private:
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               Xbox360Button buttonType);
  unsigned int _setDPadValue(unsigned int newValue, unsigned int currentValue,
                             Xbox360Dpad buttonType);

private:
  float _leftTrigger;
  float _rightTrigger;

  ::std::function<void(float value)> _onlefttriggerchanged;
  ::std::function<void(float value)> _onrighttriggerchanged;

  ::std::function<void(Xbox360Button buttonPressed)> _onbuttondown;
  ::std::function<void(Xbox360Button buttonReleased)> _onbuttonup;
  ::std::function<void(Xbox360Dpad dPadPressed)> _ondpaddown;
  ::std::function<void(Xbox360Dpad dPadReleased)> _ondpadup;

  unsigned int _buttonA;
  unsigned int _buttonB;
  unsigned int _buttonX;
  unsigned int _buttonY;
  unsigned int _buttonBack;
  unsigned int _buttonStart;
  unsigned int _buttonLB;
  unsigned int _buttonRB;

  unsigned int _buttonLeftStick;
  unsigned int _buttonRightStick;
  unsigned int _dPadUp;
  unsigned int _dPadDown;
  unsigned int _dPadLeft;
  unsigned int _dPadRight;

  bool _isXboxOnePad;

}; // end of class Xbox360Pad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_XBOX_360_PAD_H
