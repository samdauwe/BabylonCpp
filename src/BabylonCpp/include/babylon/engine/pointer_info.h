#ifndef BABYLON_ENGINE_POINTER_INFO_H
#define BABYLON_ENGINE_POINTER_INFO_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/engine/pointer_info_base.h>

namespace BABYLON {

/**
 * @brief This type contains all the data related to a pointer event in
 * Babylon.js.
 * The event member is an instance of PointerEvent for all types except
 * PointerWheel and is of type MouseWheelEvent when type equals PointerWheel.
 * The different event types can be found in the PointerEventTypes class.
 */
class BABYLON_SHARED_EXPORT PointerInfo : public PointerInfoBase {

public:
  PointerInfo(unsigned int type, const PointerEvent& event,
              const PickingInfo& pickInfo);
  PointerInfo(unsigned int type, const MouseWheelEvent& event,
              const PickingInfo& pickInfo);
  ~PointerInfo();

public:
  PickingInfo pickInfo;

}; // end of struct PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_POINTER_INFO_H
