#ifndef BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Single axis scale gizmo.
 */
class BABYLON_SHARED_EXPORT AxisScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates an AxisScaleGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param dragAxis The axis which the gizmo will be able to drag on
   * @param color The color of the gizmo
   */
  AxisScaleGizmo(UtilityLayerRenderer* gizmoLayer, const Vector3& dragAxis,
                 const Color3& color);
  ~AxisScaleGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedMeshChanged(AbstractMesh* value) override;

public:
  /**
   * Scale distance in babylon units that the gizmo will snap to when dragged
   * (Default: 0)
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

  float _currentSnapDragDistance;
  Vector3 _tmpVector;
  SnapEvent _tmpSnapEvent;

}; // end of class AxisScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
