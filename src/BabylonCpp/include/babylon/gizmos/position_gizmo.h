#ifndef BABYLON_GIZMOS_POSITION_GIZMO_H
#define BABYLON_GIZMOS_POSITION_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>

#include <babylon/core/nullable.h>

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
  PositionGizmo(UtilityLayerRenderer* gizmoLayer);
  ~PositionGizmo() override;

  void setAttachedMesh(Mesh* mesh);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

private:
  unique_ptr_t<AxisDragGizmo> _xDrag;
  unique_ptr_t<AxisDragGizmo> _yDrag;
  unique_ptr_t<AxisDragGizmo> _zDrag;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_POSITION_GIZMO_H
