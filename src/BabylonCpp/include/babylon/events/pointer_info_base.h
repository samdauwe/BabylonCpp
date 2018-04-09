#ifndef BABYLON_EVENTS_POINTER_INFO_BASE_H
#define BABYLON_EVENTS_POINTER_INFO_BASE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_event_types.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PointerInfoBase {

public:
  PointerInfoBase(PointerEventTypes type, const PointerEvent& event);
  PointerInfoBase(PointerEventTypes type, const MouseWheelEvent& event);
  ~PointerInfoBase();

public:
  PointerEventTypes type;
  PointerEvent pointerEvent;
  MouseWheelEvent mouseWheelEvent;

}; // end of class PointerInfoBase

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_INFO_BASE_H
