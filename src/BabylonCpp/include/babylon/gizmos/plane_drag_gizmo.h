#ifndef BABYLON_GIZMOS_PLANE_DRAG_GIZMO_H
#define BABYLON_GIZMOS_PLANE_DRAG_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class PositionGizmo;
class StandardMaterial;
class TransformNode;
class UtilityLayerRenderer;
using PointerDragBehaviorPtr = std::shared_ptr<PointerDragBehavior>;
using StandardMaterialPtr     = std::shared_ptr<StandardMaterial>;
using TransformNodePtr        = std::shared_ptr<TransformNode>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

/**
 * @brief Single plane drag gizmo.
 */
class BABYLON_SHARED_EXPORT PlaneDragGizmo : public Gizmo {

public:
  /**
   * @brief Hidden
   */
  static TransformNodePtr _CreatePlane(Scene* scene,
                                       const StandardMaterialPtr& material);

  /**
   * @brief Hidden
   */
  static TransformNodePtr _CreateArrowInstance(Scene* scene,
                                               const TransformNodePtr& arrow);

public:
  /**
   * @brief Creates a PlaneDragGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param dragPlaneNormal The axis normal to which the gizmo will be able to
   * drag on
   * @param color The color of the gizmo
   */
  PlaneDragGizmo(const Vector3& dragPlaneNormal,
                 const Color3& color = Color3::Gray(),
                 const UtilityLayerRendererPtr& gizmoLayer
                 = UtilityLayerRenderer::DefaultUtilityLayer(),
                 PositionGizmo* parent = nullptr);
  ~PlaneDragGizmo() override;

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedMeshChanged(const AbstractMeshPtr& value) override;

  /**
   * @brief Gets if the gizmo is enabled.
   */
  bool get_isEnabled() const;

  /**
   * @brief Sets if the gizmo is enabled.
   */
  void set_isEnabled(bool value);

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  PointerDragBehaviorPtr dragBehavior;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged
   * (Default: 0)
   */
  float snapDistance;
  /**
   * Event that fires each time the gizmo snaps to a new location.
   * * snapDistance is the the change in distance
   */
  Observable<SnapEvent> onSnapObservable;

  /**
   * If the gizmo is enabled
   */
  Property<PlaneDragGizmo, bool> isEnabled;

private:
  Observer<PointerInfo>::Ptr _pointerObserver;

  TransformNodePtr _plane;
  StandardMaterialPtr _coloredMaterial;
  StandardMaterialPtr _hoverMaterial;

  bool _isEnabled;
  PositionGizmo* _parent;

  float currentSnapDragDistance;
  Vector3 tmpVector;
  SnapEvent tmpSnapEvent;

}; // end of class PlaneDragGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_PLANE_DRAG_GIZMO_H
