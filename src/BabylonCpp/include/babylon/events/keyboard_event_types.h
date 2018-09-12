#ifndef BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H
#define BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT KeyboardEventTypes {
  static constexpr unsigned int KEYDOWN = 0x01;
  static constexpr unsigned int KEYUP   = 0x02;
}; // end of struct KeyboardEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_KEYBOARD_EVENT_TYPES_H
