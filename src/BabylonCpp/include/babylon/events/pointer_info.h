#ifndef BABYLON_EVENTS_POINTER_INFO_H
#define BABYLON_EVENTS_POINTER_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/events/pointer_info_base.h>

namespace BABYLON {

/**
 * @brief This type contains all the data related to a pointer event in Babylon.js.
 * The event member is an instance of PointerEvent for all types except PointerWheel and is of type
 * MouseWheelEvent when type equals PointerWheel. The different event types can be found in the
 * PointerEventTypes class.
 */
class BABYLON_SHARED_EXPORT PointerInfo : public PointerInfoBase {

public:
  /**
   * @brief Instantiates a PointerInfo to store pointer related info to the onPointerObservable
   * event.
   * @param type Defines the type of event (PointerEventTypes)
   * @param event Defines the related dom event
   * @param pickInfo Defines the picking info associated to the info (if any)\
   */
  PointerInfo(PointerEventTypes type, const PointerEvent& event, const PickingInfo& pickInfo);
  PointerInfo(PointerEventTypes type, const MouseWheelEvent& event, const PickingInfo& pickInfo);
  ~PointerInfo(); // = default

public:
  /**
   * Defines the picking info associated to the info (if any)
   */
  PickingInfo pickInfo;

}; // end of class PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_INFO_H
