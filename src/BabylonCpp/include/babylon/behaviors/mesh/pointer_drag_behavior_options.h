#ifndef BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H
#define BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PointerDragBehaviorOptions {
  Nullable<Vector3> dragAxis        = nullptr;
  Nullable<Vector3> dragPlaneNormal = nullptr;
}; // end of struct PointerDragBehaviorOptions

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_OPTIONS_H
