#ifndef BABYLON_GAMEPAD_STICK_VALUES_H
#define BABYLON_GAMEPAD_STICK_VALUES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Represents a gamepad control stick position.
 */
struct BABYLON_SHARED_EXPORT StickValues {

  StickValues();

  /**
   * @brief Initializes the gamepad x and y control stick values.
   * @param x The x component of the gamepad control stick value
   * @param y The y component of the gamepad control stick value
   */
  StickValues(float x, float y);

  StickValues(const StickValues& other);
  StickValues(StickValues&& other);
  StickValues& operator=(const StickValues& other);
  StickValues& operator=(StickValues&& other);
  ~StickValues();

  /**
   * The x component of the control stick
   */
  float x;

  /**
   * The y component of the control stick
   */
  float y;

}; // end of struct StickValues

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_STICK_VALUES_H
