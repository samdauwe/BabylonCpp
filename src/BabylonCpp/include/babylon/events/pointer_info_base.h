#ifndef BABYLON_EVENTS_POINTER_INFO_BASE_H
#define BABYLON_EVENTS_POINTER_INFO_BASE_H

#include <babylon/babylon_api.h>
#include <babylon/events/device_input_events.h>
#include <babylon/events/pointer_event_types.h>

namespace BABYLON {

/**
 * @brief Base class of pointer info types.
 */
class BABYLON_SHARED_EXPORT PointerInfoBase {

public:
  /**
   * @brief Instantiates the base class of pointers info.
   * @param type Defines the type of event (PointerEventTypes)
   * @param event Defines the related dom event
   */
  PointerInfoBase(PointerEventTypes type, const IPointerEvent& event);
  PointerInfoBase(PointerEventTypes type, const IWheelEvent& event);
  ~PointerInfoBase(); // = default

public:
  /**
   * Defines the type of event (PointerEventTypes)
   */
  PointerEventTypes type;

  /**
   * Defines the related pointer event
   */
  IPointerEvent pointerEvent;

  /**
   * Defines the related mouse event
   */
  IWheelEvent mouseWheelEvent;

}; // end of class PointerInfoBase

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_INFO_BASE_H
