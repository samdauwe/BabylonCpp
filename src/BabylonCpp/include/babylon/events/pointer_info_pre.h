#ifndef BABYLON_EVENTS_POINTER_INFO_PRE_H
#define BABYLON_EVENTS_POINTER_INFO_PRE_H

#include <babylon/babylon_api.h>
#include <babylon/culling/ray.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/events/pointer_info_base.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

/**
 * @brief This class is used to store pointer related info for the onPrePointerObservable event.
 * Set the skipOnPointerObservable property to true if you want the engine to stop any process after
 * this event is triggered, even not calling onPointerObservable
 */
class BABYLON_SHARED_EXPORT PointerInfoPre : public PointerInfoBase {

public:
  /**
   * @brief Instantiates a PointerInfoPre to store pointer related info to the
   * onPrePointerObservable event.
   * @param type Defines the type of event (PointerEventTypes)
   * @param event Defines the related dom event
   * @param localX Defines the local x coordinates of the pointer when the event occured
   * @param localY Defines the local y coordinates of the pointer when the event occured
   */
  PointerInfoPre(PointerEventTypes type, const IPointerEvent& event, float localX, float localY);
  PointerInfoPre(PointerEventTypes type, const IWheelEvent& event, float localX, float localY);
  ~PointerInfoPre(); // = default

public:
  /**
   * Ray from a pointer if available (eg. 6dof controller)
   */
  std::optional<Ray> ray;

  /**
   * Defines the local position of the pointer on the canvas.
   */
  Vector2 localPosition;

  /**
   * Defines whether the engine should skip the next OnPointerObservable associated to this pre.
   */
  bool skipOnPointerObservable;

}; // end of class PointerInfoPre

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_INFO_PRE_H
