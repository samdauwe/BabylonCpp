#ifndef BABYLON_GAMEPAD_GAMEPAD_H
#define BABYLON_GAMEPAD_GAMEPAD_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/stick_values.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Gamepad {

public:
  static constexpr unsigned int GAMEPAD      = 0;
  static constexpr unsigned int GENERIC      = 1;
  static constexpr unsigned int XBOX         = 2;
  static constexpr unsigned int POSE_ENABLED = 3;

public:
  Gamepad(const string_t& id, int index,
          const shared_ptr_t<IBrowserGamepad>& browserGamepad,
          unsigned int leftStickX = 0, unsigned int leftStickY = 1,
          unsigned int rightStickX = 2, unsigned int rightStickY = 3);
  virtual ~Gamepad();

  void setOnleftstickchanged(
    const ::std::function<void(const StickValues& values)>& callback);
  void setOnrightstickchanged(
    const ::std::function<void(const StickValues& values)>& callback);
  virtual void update();
  virtual void dispose();

public:
  string_t id;
  int index;
  unsigned int type;
  bool _isConnected;

  ReadOnlyProperty<Gamepad, bool> isConnected;
  Property<Gamepad, nullable_t<StickValues>> leftStick;
  Property<Gamepad, nullable_t<StickValues>> rightStick;

protected:
  bool _invertLeftStickY;
  shared_ptr_t<IBrowserGamepad> _browserGamepad;

private:
  bool get_isConnected() const;
  nullable_t<StickValues>& get_leftStick();
  void set_leftStick(const nullable_t<StickValues>& newValues);
  nullable_t<StickValues>& get_rightStick();
  void set_rightStick(const nullable_t<StickValues>& newValues);
  nullable_t<StickValues> _leftStick;
  nullable_t<StickValues> _rightStick;

  unsigned int _leftStickAxisX;
  unsigned int _leftStickAxisY;
  unsigned int _rightStickAxisX;
  unsigned int _rightStickAxisY;

  ::std::function<void(const StickValues& values)> _onleftstickchanged;
  ::std::function<void(const StickValues& values)> _onrightstickchanged;

}; // end of class Gamepad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GAMEPAD_H
