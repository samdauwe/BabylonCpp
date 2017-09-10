#ifndef BABYLON_GAMEPAD_STICK_VALUES_H
#define BABYLON_GAMEPAD_STICK_VALUES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StickValues {

  StickValues(float x, float y);
  StickValues(const StickValues& other);
  StickValues(StickValues&& other);
  StickValues& operator=(const StickValues& other);
  StickValues& operator=(StickValues&& other);
  ~StickValues();

  float x, y;

}; // end of struct StickValues

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_STICK_VALUES_H
