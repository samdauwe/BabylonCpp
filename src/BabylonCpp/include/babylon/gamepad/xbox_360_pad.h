#ifndef BABYLON_GAMEPAD_XBOX_360_PAD_H
#define BABYLON_GAMEPAD_XBOX_360_PAD_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/gamepad.h>
#include <babylon/tools/observable.h>

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
  ~Xbox360Pad() override;

  void
  setOnlefttriggerchanged(const ::std::function<void(float value)>& callback);
  void
  onrighttriggerchanged(const ::std::function<void(float value)>& callback);
  void onbuttondown(
    const ::std::function<void(Xbox360Button buttonPressed)>& callback);
  void onbuttonup(
    const ::std::function<void(Xbox360Button buttonReleased)>& callback);
  void
  ondpaddown(const ::std::function<void(Xbox360Dpad dPadPressed)>& callback);
  void
  ondpadup(const ::std::function<void(Xbox360Dpad dPadReleased)>& callback);

  void update() override;
  void dispose() override;

private:
  float get_leftTrigger() const;
  void set_leftTrigger(float newValue);
  float get_rightTrigger() const;
  void set_rightTrigger(float newValue);
  unsigned int get_buttonA() const;
  void set_buttonA(unsigned int value);
  unsigned int get_buttonB() const;
  void set_buttonB(unsigned int value);
  unsigned int get_buttonX() const;
  void set_buttonX(unsigned int value);
  unsigned int get_buttonY() const;
  void set_buttonY(unsigned int value);
  unsigned int get_buttonStart() const;
  void set_buttonStart(unsigned int value);
  unsigned int get_buttonBack() const;
  void set_buttonBack(unsigned int value);
  unsigned int get_buttonLB() const;
  void set_buttonLB(unsigned int value);
  unsigned int get_buttonRB() const;
  void set_buttonRB(unsigned int value);
  unsigned int get_buttonLeftStick() const;
  void set_buttonLeftStick(unsigned int value);
  unsigned int get_buttonRightStick() const;
  void set_buttonRightStick(unsigned int value);
  unsigned int get_dPadUp() const;
  void set_dPadUp(unsigned int value);
  unsigned int get_dPadDown() const;
  void set_dPadDown(unsigned int value);
  unsigned int get_dPadLeft() const;
  void set_dPadLeft(unsigned int value);
  unsigned int get_dPadRight() const;
  void set_dPadRight(unsigned int value);
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               Xbox360Button buttonType);
  unsigned int _setDPadValue(unsigned int newValue, unsigned int currentValue,
                             Xbox360Dpad buttonType);

public:
  Observable<Xbox360Button> onButtonDownObservable;
  Observable<Xbox360Button> onButtonUpObservable;
  Observable<Xbox360Dpad> onPadDownObservable;
  Observable<Xbox360Dpad> onPadUpObservable;

  Property<Xbox360Pad, float> leftTrigger;
  Property<Xbox360Pad, float> rightTrigger;
  Property<Xbox360Pad, unsigned int> buttonA;
  Property<Xbox360Pad, unsigned int> buttonB;
  Property<Xbox360Pad, unsigned int> buttonX;
  Property<Xbox360Pad, unsigned int> buttonY;
  Property<Xbox360Pad, unsigned int> buttonStart;
  Property<Xbox360Pad, unsigned int> buttonBack;
  Property<Xbox360Pad, unsigned int> buttonLB;
  Property<Xbox360Pad, unsigned int> buttonRB;
  Property<Xbox360Pad, unsigned int> buttonLeftStick;
  Property<Xbox360Pad, unsigned int> buttonRightStick;
  Property<Xbox360Pad, unsigned int> dPadUp;
  Property<Xbox360Pad, unsigned int> dPadDown;
  Property<Xbox360Pad, unsigned int> dPadLeft;
  Property<Xbox360Pad, unsigned int> dPadRight;

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
