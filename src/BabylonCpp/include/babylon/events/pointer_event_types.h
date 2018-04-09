#ifndef BABYLON_EVENTS_POINTER_EVENT_TYPES_H
#define BABYLON_EVENTS_POINTER_EVENT_TYPES_H

namespace BABYLON {

enum class PointerEventTypes {
  POINTERDOWN      = 0x01,
  POINTERUP        = 0x02,
  POINTERMOVE      = 0x04,
  POINTERWHEEL     = 0x08,
  POINTERPICK      = 0x10,
  POINTERTAP       = 0x20,
  POINTERDOUBLETAP = 0x40,
}; // end of enum class PointerEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_EVENT_TYPES_H
