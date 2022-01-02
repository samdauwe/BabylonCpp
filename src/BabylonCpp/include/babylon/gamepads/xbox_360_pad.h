#ifndef BABYLON_GAMEPADS_XBOX_360_PAD_H
#define BABYLON_GAMEPADS_XBOX_360_PAD_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/gamepads/gamepad.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class IBrowserGamepad;

/**
 * @brief Defines a XBox360 gamepad.
 */
class BABYLON_SHARED_EXPORT Xbox360Pad : public Gamepad {

public:
  /**
   * @brief Defines supported buttons for XBox360 compatible gamepads.
   */
  enum class Xbox360Button {
    /** A */
    A = 0,
    /** B */
    B = 1,
    /** X */
    X = 2,
    /** Y */
    Y = 3,
    /** Left button */
    LB = 4,
    /** Right button */
    RB = 5,
    /** Back */
    Back = 8,
    /** Start */
    Start = 9,
    /** Left stick */
    LeftStick = 10,
    /** Right stick */
    RightStick = 11
  }; // end of enum class Xbox360Button

  /**
   * @brief Defines values for XBox360 DPad.
   */
  enum class Xbox360Dpad {
    /** Up */
    Up = 12,
    /** Down */
    Down = 13,
    /** Left */
    Left = 14,
    /** Right */
    Right = 15
  }; // end of enum class Xbox360Dpad

public:
  /**
   * @brief Creates a new XBox360 gamepad object.
   * @param id defines the id of this gamepad
   * @param index defines its index
   * @param gamepad defines the internal HTML gamepad object
   * @param xboxOne defines if it is a XBox One gamepad
   */
  Xbox360Pad(const std::string& id, int index, const IBrowserGamepadPtr& gamepad,
             bool xboxOne = false);
  ~Xbox360Pad() override; // = default

  /**
   * @brief Defines the callback to call when left trigger is pressed.
   * @param callback defines the callback to use
   */
  void setOnlefttriggerchanged(const std::function<void(float value)>& callback);

  /**
   * @brief Defines the callback to call when right trigger is pressed.
   * @param callback defines the callback to use
   */
  void onrighttriggerchanged(const std::function<void(float value)>& callback);

  /**
   * @brief Defines the callback to call when a button is pressed.
   * @param callback defines the callback to use
   */
  void onbuttondown(const std::function<void(Xbox360Button buttonPressed)>& callback);

  /**
   * @brief Defines the callback to call when a button is released.
   * @param callback defines the callback to use
   */
  void onbuttonup(const std::function<void(Xbox360Button buttonReleased)>& callback);

  /**
   * @brief Defines the callback to call when a pad is pressed.
   * @param callback defines the callback to use
   */
  void ondpaddown(const std::function<void(Xbox360Dpad dPadPressed)>& callback);

  /**
   * @brief Defines the callback to call when a pad is released.
   * @param callback defines the callback to use
   */
  void ondpadup(const std::function<void(Xbox360Dpad dPadReleased)>& callback);

  /**
   * @brief Force the gamepad to synchronize with device values.
   */
  void update() override;

  /**
   * @brief Disposes the gamepad.
   */
  void dispose() override;

protected:
  /**
   * @brief Gets the left trigger value.
   */
  [[nodiscard]] float get_leftTrigger() const;

  /**
   * @brief Sets the left trigger value.
   */
  void set_leftTrigger(float newValue);

  /**
   * @brief Gets the right trigger value.
   */
  [[nodiscard]] float get_rightTrigger() const;

  /**
   * @brief Sets the right trigger value.
   */
  void set_rightTrigger(float newValue);

  /**
   * @brief Gets the value of the `A` button.
   */
  [[nodiscard]] unsigned int get_buttonA() const;

  /**
   * @brief Sets the value of the `A` button.
   */
  void set_buttonA(unsigned int value);

  /**
   * @brief Gets the value of the `B` button.
   */
  [[nodiscard]] unsigned int get_buttonB() const;

  /**
   * @brief Sets the value of the `B` button.
   */
  void set_buttonB(unsigned int value);

  /**
   * @brief Gets the value of the `X` button.
   */
  [[nodiscard]] unsigned int get_buttonX() const;

  /**
   * @brief Sets the value of the `X` button.
   */
  void set_buttonX(unsigned int value);

  /**
   * @brief Gets the value of the `Y` button.
   */
  [[nodiscard]] unsigned int get_buttonY() const;

  /**
   * @brief Sets the value of the `Y` button.
   */
  void set_buttonY(unsigned int value);

  /**
   * @brief Gets the value of the `Start` button.
   */
  [[nodiscard]] unsigned int get_buttonStart() const;

  /**
   * @brief Sets the value of the `Start` button.
   */
  void set_buttonStart(unsigned int value);

  /**
   * @brief Gets the value of the `Back` button.
   */
  [[nodiscard]] unsigned int get_buttonBack() const;

  /**
   * @brief Sets the value of the `Back` button.
   */
  void set_buttonBack(unsigned int value);

  /**
   * @brief Gets the value of the `Left` button.
   */
  [[nodiscard]] unsigned int get_buttonLB() const;

  /**
   * @brief Sets the value of the `Left` button.
   */
  void set_buttonLB(unsigned int value);

  /**
   * @brief Gets the value of the `Right` button.
   */
  [[nodiscard]] unsigned int get_buttonRB() const;

  /**
   * @brief Sets the value of the `Right` button.
   */
  void set_buttonRB(unsigned int value);

  /**
   * @brief Gets the value of the Left joystick.
   */
  [[nodiscard]] unsigned int get_buttonLeftStick() const;

  /**
   * @brief Sets the value of the Left joystick.
   */
  void set_buttonLeftStick(unsigned int value);

  /**
   * @brief Gets the value of the Right joystick.
   */
  [[nodiscard]] unsigned int get_buttonRightStick() const;

  /**
   * @brief Sets the value of the Right joystick.
   */
  void set_buttonRightStick(unsigned int value);

  /**
   * @brief Gets the value of D-pad up.
   */
  [[nodiscard]] unsigned int get_dPadUp() const;

  /**
   * @brief  Sets the value of D-pad up.
   */
  void set_dPadUp(unsigned int value);

  /**
   * @brief Gets the value of D-pad down.
   */
  [[nodiscard]] unsigned int get_dPadDown() const;

  /**
   * @brief Sets the value of D-pad down.
   */
  void set_dPadDown(unsigned int value);

  /**
   * @brief Gets the value of D-pad left.
   */
  [[nodiscard]] unsigned int get_dPadLeft() const;

  /**
   * @brief Sets the value of D-pad left.
   */
  void set_dPadLeft(unsigned int value);

  /**
   * @brief Gets the value of D-pad right.
   */
  [[nodiscard]] unsigned int get_dPadRight() const;

  /**
   * @brief Sets the value of D-pad right.
   */
  void set_dPadRight(unsigned int value);

private:
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               Xbox360Button buttonType);
  unsigned int _setDPadValue(unsigned int newValue, unsigned int currentValue,
                             Xbox360Dpad buttonType);

public:
  /** Observable raised when a button is pressed */
  Observable<Xbox360Button> onButtonDownObservable;
  /** Observable raised when a button is released */
  Observable<Xbox360Button> onButtonUpObservable;
  /** Observable raised when a pad is pressed */
  Observable<Xbox360Dpad> onPadDownObservable;
  /** Observable raised when a pad is released */
  Observable<Xbox360Dpad> onPadUpObservable;

  /** Left trigger */
  Property<Xbox360Pad, float> leftTrigger;
  /** Right trigger */
  Property<Xbox360Pad, float> rightTrigger;
  /** A button */
  Property<Xbox360Pad, unsigned int> buttonA;
  /** B button */
  Property<Xbox360Pad, unsigned int> buttonB;
  /** X button */
  Property<Xbox360Pad, unsigned int> buttonX;
  /** Y button */
  Property<Xbox360Pad, unsigned int> buttonY;
  /** Start button */
  Property<Xbox360Pad, unsigned int> buttonStart;
  /** Back button */
  Property<Xbox360Pad, unsigned int> buttonBack;
  /** Left button */
  Property<Xbox360Pad, unsigned int> buttonLB;
  /** Right button */
  Property<Xbox360Pad, unsigned int> buttonRB;
  /** Left stick */
  Property<Xbox360Pad, unsigned int> buttonLeftStick;
  /** Right stick */
  Property<Xbox360Pad, unsigned int> buttonRightStick;
  /** DPad up */
  Property<Xbox360Pad, unsigned int> dPadUp;
  /**  DPad down */
  Property<Xbox360Pad, unsigned int> dPadDown;
  /** DPad left */
  Property<Xbox360Pad, unsigned int> dPadLeft;
  /** DPad right */
  Property<Xbox360Pad, unsigned int> dPadRight;

private:
  float _leftTrigger;
  float _rightTrigger;

  std::function<void(float value)> _onlefttriggerchanged;
  std::function<void(float value)> _onrighttriggerchanged;

  std::function<void(Xbox360Button buttonPressed)> _onbuttondown;
  std::function<void(Xbox360Button buttonReleased)> _onbuttonup;
  std::function<void(Xbox360Dpad dPadPressed)> _ondpaddown;
  std::function<void(Xbox360Dpad dPadReleased)> _ondpadup;

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

#endif // end of BABYLON_GAMEPADS_XBOX_360_PAD_H
