#ifndef BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_OPTIONS_H
#define BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PointerDragBehaviorOptions {
  std::optional<Vector3> dragAxis        = std::nullopt;
  std::optional<Vector3> dragPlaneNormal = std::nullopt;
}; // end of struct PointerDragBehaviorOptions

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_OPTIONS_H
