#ifndef BABYLON_GIZMOS_GIZMO_MANAGER_H
#define BABYLON_GIZMOS_GIZMO_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

struct GizmoManagerOptions {
  Nullable<bool> singleGizmo = nullptr;
}; // end of struct GizmoManagerOptions

struct Gizmos {
  unique_ptr_t<PositionGizmo> positionGizmo = nullptr;
  unique_ptr_t<RotationGizmo> rotationGizmo = nullptr;
}; // end of struct Gizmos

/**
 * @brief Helps setup gizmo's in the scene to rotate/scale/position meshes.
 */
class BABYLON_SHARED_EXPORT GizmoManager : public IDisposable {

public:
  /**
   * @brief Instatiates a gizmo manager
   * @param scene the scene to overlay the gizmos on top of
   * @param options If only a single gizmo should exist at one time
   */
  GizmoManager(Scene* scene, Nullable<GizmoManagerOptions> options = nullptr);
  virtual ~GizmoManager();

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

private:
  void _clearGizmos();

private:
  Scene* _scene;
  unique_ptr_t<UtilityLayerRenderer> _gizmoLayer;
  // Set of gizmos that are currently in the scene for each mesh
  unordered_map_t<size_t, Gizmos> _gizmoSet;
  Observer<PointerInfo>::Ptr _pointerObserver;

}; // end of class GizmoManager

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_MANAGER_H
