#ifndef BABYLON_GIZMOS_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_ROTATION_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

/**
 * @brief Gizmo that enables rotating a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT RotationGizmo : public Gizmo {

public:
  /**
   * @brief Creates a RotationGizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  RotationGizmo(const shared_ptr_t<UtilityLayerRenderer>& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer());
  ~RotationGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief CustomMeshes are not supported by this gizmo.
   * @param mesh The mesh to replace the default mesh of the gizmo
   */
  void setCustomMesh(Mesh* mesh) override;

protected:
  void set_attachedMesh(AbstractMesh* const& mesh) override;
  void set_updateGizmoRotationToMatchAttachedMesh(bool value);
  bool get_updateGizmoRotationToMatchAttachedMesh() const;

public:
  /**
   * Internal gizmo used for interactions on the x axis
   */
  unique_ptr_t<PlaneRotationGizmo> xGizmo;

  /**
   * Internal gizmo used for interactions on the y axis
   */
  unique_ptr_t<PlaneRotationGizmo> yGizmo;

  /**
   * Internal gizmo used for interactions on the z axis
   */
  unique_ptr_t<PlaneRotationGizmo> zGizmo;

  Property<RotationGizmo, bool> updateGizmoRotationToMatchAttachedMesh;

}; // end of class RotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_ROTATION_GIZMO_H
