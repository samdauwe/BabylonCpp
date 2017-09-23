#ifndef BABYLON_EVENTS_POINTER_EVENT_TYPES_H
#define BABYLON_EVENTS_POINTER_EVENT_TYPES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PointerEventTypes {
  static constexpr unsigned int POINTERDOWN      = 0x01;
  static constexpr unsigned int POINTERUP        = 0x02;
  static constexpr unsigned int POINTERMOVE      = 0x04;
  static constexpr unsigned int POINTERWHEEL     = 0x08;
  static constexpr unsigned int POINTERPICK      = 0x10;
  static constexpr unsigned int POINTERTAP       = 0x20;
  static constexpr unsigned int POINTERDOUBLETAP = 0x40;
}; // end of struct PointerEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_EVENT_TYPES_H
