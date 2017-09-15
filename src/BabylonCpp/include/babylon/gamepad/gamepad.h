#ifndef BABYLON_GAMEPAD_GAMEPAD_H
#define BABYLON_GAMEPAD_GAMEPAD_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

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
  StickValues& leftStick();
  const StickValues& leftStick() const;
  void setLeftStick(const StickValues& newValues);
  StickValues& rightStick();
  const StickValues& rightStick() const;
  void setRightStick(const StickValues& newValues);
  virtual void update();
  virtual void dispose();

public:
  string_t id;
  int index;
  unsigned int type;

protected:
  shared_ptr_t<IBrowserGamepad> _browserGamepad;

private:
  Nullable<StickValues> _leftStick;
  Nullable<StickValues> _rightStick;

  unsigned int _leftStickAxisX;
  unsigned int _leftStickAxisY;
  unsigned int _rightStickAxisX;
  unsigned int _rightStickAxisY;

  ::std::function<void(const StickValues& values)> _onleftstickchanged;
  ::std::function<void(const StickValues& values)> _onrightstickchanged;

}; // end of class Gamepad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GAMEPAD_H
