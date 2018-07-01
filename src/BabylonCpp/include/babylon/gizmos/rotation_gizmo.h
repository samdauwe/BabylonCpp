#ifndef BABYLON_GIZMOS_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_ROTATION_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>

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
  RotationGizmo(UtilityLayerRenderer* gizmoLayer);
  ~RotationGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void set_attachedMesh(AbstractMesh* const& mesh) override;
  void set_updateGizmoRotationToMatchAttachedMesh(bool value);
  bool get_updateGizmoRotationToMatchAttachedMesh() const;

public:
  Property<RotationGizmo, bool> updateGizmoRotationToMatchAttachedMesh;

private:
  unique_ptr_t<PlaneRotationGizmo> _xDrag;
  unique_ptr_t<PlaneRotationGizmo> _yDrag;
  unique_ptr_t<PlaneRotationGizmo> _zDrag;

}; // end of class RotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_ROTATION_GIZMO_H
