#ifndef BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/rendering/utility_layer_renderer.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class PointerDragBehavior;

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
  PlaneRotationGizmo(const Vector3& planeNormal,
                     const Color3& color = Color3::Gray(),
                     const std::shared_ptr<UtilityLayerRenderer>& gizmoLayer
                     = UtilityLayerRenderer::DefaultUtilityLayer());
  ~PlaneRotationGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedMeshChanged(const AbstractMeshPtr& value) override;

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  std::unique_ptr<PointerDragBehavior> dragBehavior;

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
  Observer<PointerInfo>::Ptr _pointerObserver;

  Vector3 _lastDragPosition;
  Matrix _rotationMatrix;
  Vector3 _planeNormalTowardsCamera;
  Vector3 _localPlaneNormalTowardsCamera;
  SnapEvent _tmpSnapEvent;
  float _currentSnapDragDistance;
  Matrix _tmpMatrix;
  Vector3 _tmpVector;
  Quaternion _amountToRotate;

}; // end of class PlaneRotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
