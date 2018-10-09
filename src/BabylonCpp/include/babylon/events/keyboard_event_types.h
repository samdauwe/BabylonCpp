#ifndef BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H
#define BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Gather the list of keyboard event types as constants.
 */
struct BABYLON_SHARED_EXPORT KeyboardEventTypes {
  /**
   * The keydown event is fired when a key becomes active (pressed).
   */
  static constexpr unsigned int KEYDOWN = 0x01;
  /**
   * The keyup event is fired when a key has been released.
   */
  static constexpr unsigned int KEYUP = 0x02;
}; // end of struct KeyboardEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H
