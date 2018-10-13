#ifndef BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
#define BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/rendering/utility_layer_renderer.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class PointerDragBehavior;

/**
 * @brief Single axis drag gizmo.
 */
class BABYLON_SHARED_EXPORT AxisDragGizmo : public Gizmo {

public:
  /**
   * @brief Creates an AxisDragGizmo.
   * @param dragAxis The axis which the gizmo will be able to drag on
   * @param color The color of the gizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  AxisDragGizmo(const Vector3& dragAxis, const Color3& color = Color3::Gray(),
                const std::shared_ptr<UtilityLayerRenderer>& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer());
  ~AxisDragGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedMeshChanged(AbstractMesh* value) override;

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  std::unique_ptr<PointerDragBehavior> _dragBehavior;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged
   * (Default: 0)
   */
  float snapDistance;

  /**
   * Event that fires each time the gizmo snaps to a new location.
   * * snapDistance is the the change in distance
   */
  Observable<SnapEvent> onSnapObservable;

private:
  Observer<PointerInfo>::Ptr _pointerObserver;

  float _currentSnapDragDistance;
  Vector3 _tmpVector;
  SnapEvent _tmpSnapEvent;
  Vector3 _localDelta;
  Matrix _tmpMatrix;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
