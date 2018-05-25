#ifndef BABYLON_EVENTS_POINTER_INFO_PRE_H
#define BABYLON_EVENTS_POINTER_INFO_PRE_H

#include <babylon/babylon_global.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/events/pointer_info_base.h>

namespace BABYLON {

/**
 * @brief This class is used to store pointer related info for the
 * onPrePointerObservable event.
 * Set the skipOnPointerObservable property to true if you want the engine
 * to stop any process after this event is triggered, even not calling
 * onPointerObservable
 */
class BABYLON_SHARED_EXPORT PointerInfoPre : public PointerInfoBase {

public:
  PointerInfoPre(PointerEventTypes type, const PointerEvent& event,
                 float localX, float localY);
  PointerInfoPre(PointerEventTypes type, const MouseWheelEvent& event,
                 float localX, float localY);
  ~PointerInfoPre();

public:
  Vector2 localPosition;
  bool skipOnPointerObservable;

  /**
   * Ray from a pointer if available (eg. 6dof controller)
   */
  Nullable<Ray> ray;

}; // end of class PointerInfoPre

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_INFO_PRE_H
