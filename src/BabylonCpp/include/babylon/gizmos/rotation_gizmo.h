#ifndef BABYLON_GIZMOS_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_ROTATION_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class PlaneRotationGizmo;
class UtilityLayerRenderer;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

/**
 * @brief Gizmo that enables rotating a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT RotationGizmo : public Gizmo {

public:
  /**
   * @brief Creates a RotationGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param tessellation Amount of tessellation to be used when creating rotation circles
   * @param useEulerRotation Use and update Euler angle instead of quaternion
   * @param thickness display gizmo axis thickness
   */
  RotationGizmo(const UtilityLayerRendererPtr& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer(),
                unsigned int tessellation = 32, bool useEulerRotation = false,
                float thickness = 1.f);
  ~RotationGizmo() override; // = default

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Disposes and replaces the current meshes in the gizmo with the specified mesh.
   * @param mesh The mesh to replace the default mesh of the gizmo
   * @param useGizmoMaterial If the gizmo's default material should be used (default: false)
   */
  void setCustomMesh(const MeshPtr& mesh, bool useGizmoMaterial = false) override;

protected:
  AbstractMeshPtr& get_attachedMesh() override;
  void set_attachedMesh(const AbstractMeshPtr& mesh) override;
  NodePtr& get_attachedNode() override;
  void set_attachedNode(const NodePtr& node) override;
  void _checkBillboardTransform();

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

public:
  /**
   * Internal gizmo used for interactions on the x axis
   */
  std::unique_ptr<PlaneRotationGizmo> xGizmo;

  /**
   * Internal gizmo used for interactions on the y axis
   */
  std::unique_ptr<PlaneRotationGizmo> yGizmo;

  /**
   * Internal gizmo used for interactions on the z axis
   */
  std::unique_ptr<PlaneRotationGizmo> zGizmo;

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
  Property<RotationGizmo, float> snapDistance;

private:
  AbstractMeshPtr _meshAttached;
  NodePtr _nodeAttached;

}; // end of class RotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_ROTATION_GIZMO_H
