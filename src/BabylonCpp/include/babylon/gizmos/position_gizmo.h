#ifndef BABYLON_GIZMOS_POSITION_GIZMO_H
#define BABYLON_GIZMOS_POSITION_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

/**
 * @brief Gizmo that enables dragging a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT PositionGizmo : public Gizmo {

public:
  /**
   * @brief Creates a PositionGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  PositionGizmo(const shared_ptr_t<UtilityLayerRenderer>& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer());
  ~PositionGizmo() override;

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
  void set_attachedMesh(const AbstractMeshPtr& mesh) override;
  void set_updateGizmoRotationToMatchAttachedMesh(bool value);
  bool get_updateGizmoRotationToMatchAttachedMesh() const;
  void set_snapDistance(float value);
  float get_snapDistance() const;
  void set_scaleRatio(float value);
  float get_scaleRatio() const;

public:
  /**
   * Internal gizmo used for interactions on the x axis
   */
  unique_ptr_t<AxisDragGizmo> xGizmo;

  /**
   * Internal gizmo used for interactions on the y axis
   */
  unique_ptr_t<AxisDragGizmo> yGizmo;

  /**
   * Internal gizmo used for interactions on the z axis
   */
  unique_ptr_t<AxisDragGizmo> zGizmo;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged
   * (Default: 0)
   */
  Property<PositionGizmo, float> snapDistance;

  /**
   * Ratio for the scale of the gizmo (Default: 1)
   */
  Property<PositionGizmo, float> scaleRatio;

  Property<PositionGizmo, bool> updateGizmoRotationToMatchAttachedMesh;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_POSITION_GIZMO_H
