#ifndef BABYLON_GAMEPAD_GAMEPAD_H
#define BABYLON_GAMEPAD_GAMEPAD_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/gamepad/stick_values.h>

namespace BABYLON {

class IBrowserGamepad;

class BABYLON_SHARED_EXPORT Gamepad {

public:
  static constexpr unsigned int GAMEPAD      = 0;
  static constexpr unsigned int GENERIC      = 1;
  static constexpr unsigned int XBOX         = 2;
  static constexpr unsigned int POSE_ENABLED = 3;

public:
  Gamepad(const std::string& id, int index,
          const std::shared_ptr<IBrowserGamepad>& browserGamepad,
          unsigned int leftStickX = 0, unsigned int leftStickY = 1,
          unsigned int rightStickX = 2, unsigned int rightStickY = 3);
  virtual ~Gamepad();

  void setOnleftstickchanged(
    const std::function<void(const StickValues& values)>& callback);
  void setOnrightstickchanged(
    const std::function<void(const StickValues& values)>& callback);
  virtual void update();
  virtual void dispose();

public:
  std::string id;
  int index;
  unsigned int type;
  /** Hidden */
  bool _isConnected;

  ReadOnlyProperty<Gamepad, bool> isConnected;
  Property<Gamepad, std::optional<StickValues>> leftStick;
  Property<Gamepad, std::optional<StickValues>> rightStick;

protected:
  bool _invertLeftStickY;
  std::shared_ptr<IBrowserGamepad> _browserGamepad;

  bool get_isConnected() const;
  std::optional<StickValues>& get_leftStick();
  void set_leftStick(const std::optional<StickValues>& newValues);
  std::optional<StickValues>& get_rightStick();
  void set_rightStick(const std::optional<StickValues>& newValues);
  std::optional<StickValues> _leftStick;
  std::optional<StickValues> _rightStick;

private:
  unsigned int _leftStickAxisX;
  unsigned int _leftStickAxisY;
  unsigned int _rightStickAxisX;
  unsigned int _rightStickAxisY;

  std::function<void(const StickValues& values)> _onleftstickchanged;
  std::function<void(const StickValues& values)> _onrightstickchanged;

}; // end of class Gamepad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GAMEPAD_H
