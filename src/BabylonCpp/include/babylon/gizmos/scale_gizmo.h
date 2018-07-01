#ifndef BABYLON_GIZMOS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_SCALE_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>

namespace BABYLON {

/**
 * @brief Gizmo that enables scaling a mesh along 3 axis.
 */
class BABYLON_SHARED_EXPORT ScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates a ScaleGizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  ScaleGizmo(UtilityLayerRenderer* gizmoLayer);
  ~ScaleGizmo() override;

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
  Property<ScaleGizmo, bool> updateGizmoRotationToMatchAttachedMesh;

private:
  unique_ptr_t<AxisScaleGizmo> _xDrag;
  unique_ptr_t<AxisScaleGizmo> _yDrag;
  unique_ptr_t<AxisScaleGizmo> _zDrag;

}; // end of class ScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_SCALE_GIZMO_H
