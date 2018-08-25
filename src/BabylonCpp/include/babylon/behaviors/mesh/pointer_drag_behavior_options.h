#ifndef BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H
#define BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PointerDragBehaviorOptions {
  nullable_t<Vector3> dragAxis        = nullopt_t;
  nullable_t<Vector3> dragPlaneNormal = nullopt_t;
}; // end of struct PointerDragBehaviorOptions

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H
