#ifndef BABYLON_GIZMOS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_SCALE_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class AxisScaleGizmo;

/**
 * @brief Gizmo that enables scaling a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT ScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates a ScaleGizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  ScaleGizmo(const std::shared_ptr<UtilityLayerRenderer>& gizmoLayer
             = UtilityLayerRenderer::DefaultUtilityLayer());
  ~ScaleGizmo() override; // = default

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  AbstractMeshPtr& get_attachedMesh() override;
  void set_attachedMesh(const AbstractMeshPtr& mesh) override;
  void set_updateGizmoRotationToMatchAttachedMesh(bool value) override;
  bool get_updateGizmoRotationToMatchAttachedMesh() const override;
  void set_snapDistance(float value);
  float get_snapDistance() const;
  void set_scaleRatio(float value) override;
  float get_scaleRatio() const override;

public:
  /**
   * Internal gizmo used for interactions on the x axis
   */
  std::unique_ptr<AxisScaleGizmo> xGizmo;

  /**
   * Internal gizmo used for interactions on the y axis
   */
  std::unique_ptr<AxisScaleGizmo> yGizmo;

  /**
   * Internal gizmo used for interactions on the z axis
   */
  std::unique_ptr<AxisScaleGizmo> zGizmo;

  /**
   * Internal gizmo used to scale all axis equally
   */
  std::unique_ptr<AxisScaleGizmo> uniformScaleGizmo;

  /**
   * Fires an event when any of it's sub gizmos are dragged
   */
  Observable<DragStartOrEndEvent> onDragStartObservable;

  /**
   * Fires an event when any of it's sub gizmos are released from dragging
   */
  Observable<DragStartOrEndEvent> onDragEndObservable;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged
   * (Default: 0)
   */
  Property<ScaleGizmo, float> snapDistance;

private:
  AbstractMeshPtr _meshAttached;
  bool _updateGizmoRotationToMatchAttachedMesh;
  float _snapDistance;
  float _scaleRatio;
  MeshPtr _uniformScalingMesh;
  MeshPtr _octahedron;

}; // end of class ScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_SCALE_GIZMO_H
