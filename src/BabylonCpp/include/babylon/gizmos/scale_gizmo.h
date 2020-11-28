#ifndef BABYLON_GIZMOS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_SCALE_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class AxisScaleGizmo;
class GizmoManager;

/**
 * @brief Gizmo that enables scaling a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT ScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates a ScaleGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param thickness display gizmo axis thickness
   */
  ScaleGizmo(const std::shared_ptr<UtilityLayerRenderer>& gizmoLayer
             = UtilityLayerRenderer::DefaultUtilityLayer(),
             float thickness = 1.f, GizmoManager* gizmoManager = nullptr);
  ~ScaleGizmo() override; // = default

  /**
   * @brief Builds Gizmo Axis Cache to enable features such as hover state preservation and graying
   * out other axis during manipulation.
   * @param mesh Axis gizmo mesh
   * @param cache Gizmo axis definition used for reactive gizmo UI
   */
  void addToAxisCache(Mesh* mesh, const GizmoAxisCache& cache);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  AbstractMeshPtr& get_attachedMesh() override;
  void set_attachedMesh(const AbstractMeshPtr& mesh) override;
  NodePtr& get_attachedNode() override;
  void set_attachedNode(const NodePtr& node) override;

  /**
   * @brief Returns True when the mouse pointer is hovered a gizmo mesh.
   */
  bool get_isHovered() const override;

  void set_updateGizmoRotationToMatchAttachedMesh(bool value) override;
  [[nodiscard]] bool get_updateGizmoRotationToMatchAttachedMesh() const override;
  void set_snapDistance(float value);
  [[nodiscard]] float get_snapDistance() const;
  void set_scaleRatio(float value) override;
  [[nodiscard]] float get_scaleRatio() const override;
  void set_sensitivity(float value);
  [[nodiscard]] float get_sensitivity() const;

private:
  /**
   * @brief Create Geometry for Gizmo.
   */
  std::unique_ptr<AxisScaleGizmo> _createUniformScaleMesh();

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
   * Drag distance in babylon units that the gizmo will snap to when dragged (Default: 0)
   */
  Property<ScaleGizmo, float> snapDistance;

  /**
   * Sensitivity factor for dragging (Default: 1)
   */
  Property<ScaleGizmo, float> sensitivity;

private:
  AbstractMeshPtr _meshAttached;
  NodePtr _nodeAttached;
  float _snapDistance;
  MeshPtr _uniformScalingMesh;
  MeshPtr _octahedron;
  float _sensitivity;
  StandardMaterialPtr _coloredMaterial;
  StandardMaterialPtr _hoverMaterial;
  StandardMaterialPtr _disableMaterial;
  std::vector<Observer<PointerInfo>::Ptr> _observables;

  /** Node Caching for quick lookup */
  std::unordered_map<Mesh*, GizmoAxisCache> _gizmoAxisCache;

}; // end of class ScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_SCALE_GIZMO_H
