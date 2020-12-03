#ifndef BABYLON_GAMEPADS_GAMEPAD_H
#define BABYLON_GAMEPADS_GAMEPAD_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/gamepads/stick_values.h>

namespace BABYLON {

FWD_CLASS_SPTR(IBrowserGamepad)

/**
 * @brief Represents a gamepad.
 */
class BABYLON_SHARED_EXPORT Gamepad {

public:
  /**
   * Represents a gamepad controller
   */
  static constexpr unsigned int GAMEPAD = 0;

  /**
   * Represents a generic controller
   */
  static constexpr unsigned int GENERIC = 1;

  /**
   * Represents an XBox controller
   */
  static constexpr unsigned int XBOX = 2;

  /**
   * Represents a pose-enabled controller
   */
  static constexpr unsigned int POSE_ENABLED = 3;

  /**
   * Represents an Dual Shock controller
   */
  static constexpr unsigned int DUALSHOCK = 4;

public:
  /**
   * @brief Initializes the gamepad.
   * @param id The id of the gamepad
   * @param index The index of the gamepad
   * @param browserGamepad The browser gamepad
   * @param leftStickX The x component of the left joystick
   * @param leftStickY The y component of the left joystick
   * @param rightStickX The x component of the right joystick
   * @param rightStickY The y component of the right joystick
   */
  Gamepad(const std::string& id, int index, const IBrowserGamepadPtr& browserGamepad,
          unsigned int leftStickX = 0, unsigned int leftStickY = 1, unsigned int rightStickX = 2,
          unsigned int rightStickY = 3);
  virtual ~Gamepad(); // = default

  /**
   * @brief Callback triggered when the left joystick has changed.
   * @param callback
   */
  void onleftstickchanged(const std::function<void(const StickValues& values)>& callback);

  /**
   * @brief Callback triggered when the right joystick has changed.
   * @param callback
   */
  void onrightstickchanged(const std::function<void(const StickValues& values)>& callback);

  /**
   * @brief Updates the gamepad joystick positions.
   */
  virtual void update();

  /**
   * @brief Disposes the gamepad.
   */
  virtual void dispose();

protected:
  /**
   * @brief Gets if the gamepad has been connected.
   */
  [[nodiscard]] bool get_isConnected() const;

  /**
   * @brief Gets the left joystick.
   */
  std::optional<StickValues>& get_leftStick();

  /**
   * @brief Sets the left joystick values.
   */
  void set_leftStick(const std::optional<StickValues>& newValues);

  /**
   * @brief Gets the right joystick.
   */
  std::optional<StickValues>& get_rightStick();

  /**
   * @brief Sets the right joystick value.
   */
  void set_rightStick(const std::optional<StickValues>& newValues);

public:
  /**
   * The id of the gamepad
   */
  std::string id;

  /**
   * The index of the gamepad
   */
  int index;

  /**
   * The browser gamepad
   */
  IBrowserGamepadPtr browserGamepad;

  /**
   * Specifies what type of gamepad this represents
   */
  unsigned int type;

  /**
   * Hidden
   */
  bool _isConnected;

  /**
   * Specifies if the gamepad has been connected
   */
  ReadOnlyProperty<Gamepad, bool> isConnected;

  /**
   * Gets or sets the left joystick.
   */
  Property<Gamepad, std::optional<StickValues>> leftStick;

  /**
   * Gets or sets the right joystick.
   */
  Property<Gamepad, std::optional<StickValues>> rightStick;

protected:
  /**
   * Specifies whether the left control stick should be Y-inverted
   */
  bool _invertLeftStickY;

  std::optional<StickValues> _leftStick;
  std::optional<StickValues> _rightStick;

private:
  unsigned int _leftStickAxisX;
  unsigned int _leftStickAxisY;
  unsigned int _rightStickAxisX;
  unsigned int _rightStickAxisY;

  /**
   * Triggered when the left control stick has been changed
   */
  std::function<void(const StickValues& values)> _onleftstickchanged;

  /**
   * Triggered when the right control stick has been changed
   */
  std::function<void(const StickValues& values)> _onrightstickchanged;

}; // end of class Gamepad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_GAMEPAD_H
