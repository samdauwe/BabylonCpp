#ifndef BABYLON_GAMEPADS_DUAL_SHOCK_GAMEPAD_H
#define BABYLON_GAMEPADS_DUAL_SHOCK_GAMEPAD_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/gamepads/gamepad.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

/**
 * @brief Defines a DualShock gamepad.
 */
class BABYLON_SHARED_EXPORT DualShockPad : public Gamepad {

public:
  /**
   * @brief Defines supported buttons for DualShock compatible gamepads.
   */
  enum class DualShockButton {
    /** Cross */
    Cross,
    /** Circle */
    Circle,
    /** Square */
    Square,
    /** Triangle */
    Triangle,
    /** Options */
    Options,
    /** Share */
    Share,
    /** L1 */
    L1,
    /** R1 */
    R1,
    /** Left stick */
    LeftStick,
    /** Right stick */
    RightStick
  }; // end of enum class DualShockButton

  /**
   * @brief Defines values for DualShock DPad.
   */
  enum class DualShockDpad {
    /** Up */
    Up,
    /** Down */
    Down,
    /** Left */
    Left,
    /** Right */
    Right
  }; // end of enum class DualShockDpad

public:
  /**
   * @brief Creates a new DualShock gamepad object.
   * @param id defines the id of this gamepad
   * @param index defines its index
   * @param gamepad defines the internal HTML gamepad object
   */
  DualShockPad(const std::string& id, int index, const IBrowserGamepadPtr& gamepad);
  ~DualShockPad() override; // = default

  /**
   * @brief Defines the callback to call when left trigger is pressed.
   * @param callback defines the callback to use
   */
  void onlefttriggerchanged(const std::function<void(float value)>& callback);

  /**
   * @brief Defines the callback to call when right trigger is pressed.
   * @param callback defines the callback to use
   */
  void onrighttriggerchanged(const std::function<void(float value)>& callback);

  /**
   * @brief Defines the callback to call when a button is pressed.
   * @param callback defines the callback to use
   */
  void onbuttondown(const std::function<void(DualShockButton buttonPressed)>& callback);

  /**
   * @brief Defines the callback to call when a button is released.
   * @param callback defines the callback to use
   */
  void onbuttonup(const std::function<void(DualShockButton buttonReleased)>& callback);

  /**
   * @brief Defines the callback to call when a pad is pressed.
   * @param callback defines the callback to use
   */
  void ondpaddown(const std::function<void(DualShockDpad dPadPressed)>& callback);

  /**
   * @brief Defines the callback to call when a pad is released.
   * @param callback defines the callback to use
   */
  void ondpadup(const std::function<void(DualShockDpad dPadReleased)>& callback);

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
  float get_leftTrigger() const;

  /**
   * @brief Sets the left trigger value.
   */
  void set_leftTrigger(float newValue);

  /**
   * @brief Gets the right trigger value.
   */
  float get_rightTrigger() const;

  /**
   * @brief Sets the right trigger value.
   */
  void set_rightTrigger(float newValue);

  /**
   * @brief Gets the value of the `Cross` button.
   */
  unsigned int get_buttonCross() const;

  /**
   * @brief Gets the value of the `Cross` button.
   */
  void set_buttonCross(unsigned int value);

  /**
   * @brief Gets the value of the `Circle` button.
   */
  unsigned int get_buttonCircle() const;

  /**
   * @brief Gets the value of the `Circle` button.
   */
  void set_buttonCircle(unsigned int value);

  /**
   * @brief Gets the value of the `Square` button.
   */
  unsigned int get_buttonSquare() const;

  /**
   * @brief Gets the value of the `Square` button.
   */
  void set_buttonSquare(unsigned int value);

  /**
   * @brief Gets the value of the `Triangle` button.
   */
  unsigned int get_buttonTriangle() const;

  /**
   * @brief Gets the value of the `Triangle` button.
   */
  void set_buttonTriangle(unsigned int value);

  /**
   * @brief Gets the value of the `Options` button.
   */
  unsigned int get_buttonOptions() const;

  /**
   * @brief Gets the value of the `Options` button.
   */
  void set_buttonOptions(unsigned int value);

  /**
   * @brief Gets the value of the `Share` button.
   */
  unsigned int get_buttonShare() const;

  /**
   * @brief Gets the value of the `Share` button.
   */
  void set_buttonShare(unsigned int value);

  /**
   * @brief Gets the value of the `L1` button.
   */
  unsigned int get_buttonL1() const;

  /**
   * @brief Gets the value of the `L1` button.
   */
  void set_buttonL1(unsigned int value);

  /**
   * @brief Gets the value of the `R1` button.
   */
  unsigned int get_buttonR1() const;

  /**
   * @brief Gets the value of the `R1` button.
   */
  void set_buttonR1(unsigned int value);

  /**
   * @brief Gets the value of the Left joystick.
   */
  unsigned int get_buttonLeftStick() const;

  /**
   * @brief Sets the value of the Left joystick.
   */
  void set_buttonLeftStick(unsigned int value);

  /**
   * @brief Gets the value of the Right joystick.
   */
  unsigned int get_buttonRightStick() const;

  /**
   * @brief Sets the value of the Right joystick.
   */
  void set_buttonRightStick(unsigned int value);

  /**
   * @brief Gets the value of D-pad up.
   */
  unsigned int get_dPadUp() const;

  /**
   * @brief  Sets the value of D-pad up.
   */
  void set_dPadUp(unsigned int value);

  /**
   * @brief Gets the value of D-pad down.
   */
  unsigned int get_dPadDown() const;

  /**
   * @brief Sets the value of D-pad down.
   */
  void set_dPadDown(unsigned int value);

  /**
   * @brief Gets the value of D-pad left.
   */
  unsigned int get_dPadLeft() const;

  /**
   * @brief Sets the value of D-pad left.
   */
  void set_dPadLeft(unsigned int value);

  /**
   * @brief Gets the value of D-pad right.
   */
  unsigned int get_dPadRight() const;

  /**
   * @brief Sets the value of D-pad right.
   */
  void set_dPadRight(unsigned int value);

private:
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               DualShockButton buttonType);
  unsigned int _setDPadValue(unsigned int newValue, unsigned int currentValue,
                             DualShockDpad buttonType);

public:
  /** Observable raised when a button is pressed */
  Observable<DualShockButton> onButtonDownObservable;
  /** Observable raised when a button is released */
  Observable<DualShockButton> onButtonUpObservable;
  /** Observable raised when a pad is pressed */
  Observable<DualShockDpad> onPadDownObservable;
  /** Observable raised when a pad is released */
  Observable<DualShockDpad> onPadUpObservable;

  /** Left trigger */
  Property<DualShockPad, float> leftTrigger;
  /** Right trigger */
  Property<DualShockPad, float> rightTrigger;
  /** Cross button */
  Property<DualShockPad, unsigned int> buttonCross;
  /** Circle button */
  Property<DualShockPad, unsigned int> buttonCircle;
  /** Square button */
  Property<DualShockPad, unsigned int> buttonSquare;
  /** Triangle button */
  Property<DualShockPad, unsigned int> buttonTriangle;
  /** Options button */
  Property<DualShockPad, unsigned int> buttonOptions;
  /** Share button */
  Property<DualShockPad, unsigned int> buttonShare;
  /** L1 button */
  Property<DualShockPad, unsigned int> buttonL1;
  /** R1 button */
  Property<DualShockPad, unsigned int> buttonR1;
  /** Left stick */
  Property<DualShockPad, unsigned int> buttonLeftStick;
  /** Right stick */
  Property<DualShockPad, unsigned int> buttonRightStick;
  /** DPad up */
  Property<DualShockPad, unsigned int> dPadUp;
  /**  DPad down */
  Property<DualShockPad, unsigned int> dPadDown;
  /** DPad left */
  Property<DualShockPad, unsigned int> dPadLeft;
  /** DPad right */
  Property<DualShockPad, unsigned int> dPadRight;

private:
  float _leftTrigger;
  float _rightTrigger;

  std::function<void(float value)> _onlefttriggerchanged;
  std::function<void(float value)> _onrighttriggerchanged;

  std::function<void(DualShockButton buttonPressed)> _onbuttondown;
  std::function<void(DualShockButton buttonReleased)> _onbuttonup;
  std::function<void(DualShockDpad dPadPressed)> _ondpaddown;
  std::function<void(DualShockDpad dPadReleased)> _ondpadup;

  unsigned int _buttonCross;
  unsigned int _buttonCircle;
  unsigned int _buttonSquare;
  unsigned int _buttonTriangle;
  unsigned int _buttonShare;
  unsigned int _buttonOptions;
  unsigned int _buttonL1;
  unsigned int _buttonR1;

  unsigned int _buttonLeftStick;
  unsigned int _buttonRightStick;
  unsigned int _dPadUp;
  unsigned int _dPadDown;
  unsigned int _dPadLeft;
  unsigned int _dPadRight;

}; // end of class DualShockPad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_DUAL_SHOCK_GAMEPAD_H
