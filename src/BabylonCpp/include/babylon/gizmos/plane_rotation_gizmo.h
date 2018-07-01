#ifndef BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Single axis scale gizmo.
 */
class BABYLON_SHARED_EXPORT PlaneRotationGizmo : public Gizmo {

public:
  /**
   * @brief Creates a PlaneRotationGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param planeNormal The normal of the plane which the gizmo will be able to
   * rotate on
   * @param color The color of the gizmo
   */
  PlaneRotationGizmo(UtilityLayerRenderer* gizmoLayer,
                     const Vector3& planeNormal, const Color3& color);
  ~PlaneRotationGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedMeshChanged(AbstractMesh* value) override;

public:
  /**
   * Rotation distance in radians that the gizmo will snap to (Default: 0)
   */
  float snapDistance;

  /**
   * Event that fires each time the gizmo snaps to a new location.
   * * snapDistance is the the change in distance
   */
  Observable<SnapEvent> onSnapObservable;

private:
  unique_ptr_t<PointerDragBehavior> _dragBehavior;
  Observer<PointerInfo>::Ptr _pointerObserver;

  Nullable<Vector3> _lastDragPosition;
  Matrix _rotationMatrix;
  Vector3 _planeNormalTowardsCamera;
  Vector3 _localPlaneNormalTowardsCamera;
  SnapEvent _tmpSnapEvent;

}; // end of class PlaneRotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
