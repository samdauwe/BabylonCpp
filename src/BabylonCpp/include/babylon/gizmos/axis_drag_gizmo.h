#ifndef BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
#define BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>

namespace BABYLON {

/**
 * @brief Single axis drag gizmo.
 */
class BABYLON_SHARED_EXPORT AxisDragGizmo : public Gizmo {

public:
  /**
   * @brief Creates an AxisDragGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param dragAxis The axis which the gizmo will be able to drag on
   * @param color The color of the gizmo
   */
  AxisDragGizmo(UtilityLayerRenderer* gizmoLayer, const Vector3& dragAxis,
                const Color3& color);
  ~AxisDragGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

private:
  unique_ptr_t<PointerDragBehavior> _dragBehavior;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
