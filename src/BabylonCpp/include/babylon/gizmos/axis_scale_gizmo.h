#ifndef BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/rendering/utility_layer_renderer.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Single axis scale gizmo.
 */
class BABYLON_SHARED_EXPORT AxisScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates an AxisScaleGizmo.
   * @param dragAxis The axis which the gizmo will be able to drag on
   * @param color The color of the gizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  AxisScaleGizmo(const Vector3& dragAxis, const Color3& color = Color3::Gray(),
                 const shared_ptr_t<UtilityLayerRenderer>& gizmoLayer
                 = UtilityLayerRenderer::DefaultUtilityLayer());
  ~AxisScaleGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Disposes and replaces the current meshes in the gizmo with the
   * specified mesh.
   * @param mesh The mesh to replace the default mesh of the gizmo
   * @param useGizmoMaterial If the gizmo's default material should be used
   * (default: false)
   */
  void setCustomMesh(const MeshPtr& mesh,
                     bool useGizmoMaterial = false) override;

protected:
  void _attachedMeshChanged(const AbstractMeshPtr& value) override;

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  unique_ptr_t<PointerDragBehavior> _dragBehavior;

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

  /**
   * If the scaling operation should be done on all axis (default: false)
   */
  bool uniformScaling;

private:
  StandardMaterialPtr _coloredMaterial;
  Observer<PointerInfo>::Ptr _pointerObserver;

  float _currentSnapDragDistance;
  Vector3 _tmpVector;
  SnapEvent _tmpSnapEvent;

}; // end of class AxisScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
