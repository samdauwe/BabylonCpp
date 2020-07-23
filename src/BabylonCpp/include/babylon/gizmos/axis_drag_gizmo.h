#ifndef BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
#define BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class PointerDragBehavior;
class PositionGizmo;
class StandardMaterial;
class TransformNode;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using TransformNodePtr    = std::shared_ptr<TransformNode>;

/**
 * @brief Single axis drag gizmo.
 */
class BABYLON_SHARED_EXPORT AxisDragGizmo : public Gizmo {

public:
  /**
   * @brief Hidden
   */
  static TransformNodePtr _CreateArrow(Scene* scene, const StandardMaterialPtr& material);

  /**
   * @brief Hidden
   */
  static TransformNodePtr _CreateArrowInstance(Scene* scene, const TransformNodePtr& arrow);

public:
  /**
   * @brief Creates an AxisDragGizmo.
   * @param dragAxis The axis which the gizmo will be able to drag on
   * @param color The color of the gizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  AxisDragGizmo(const Vector3& dragAxis, const Color3& color = Color3::Gray(),
                const UtilityLayerRendererPtr& gizmoLayer
                = UtilityLayerRenderer::DefaultUtilityLayer(),
                PositionGizmo* parent = nullptr);
  ~AxisDragGizmo() override; // = default

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedNodeChanged(const NodePtr& value) override;

  /**
   * @brief Gets if the gizmo is enabled.
   */
  [[nodiscard]] bool get_isEnabled() const;

  /**
   * @brief Sets if the gizmo is enabled.
   */
  void set_isEnabled(bool value);

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  std::unique_ptr<PointerDragBehavior> dragBehavior;

  /**
   * Drag distance in babylon units that the gizmo will snap to when dragged (Default: 0)
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
  Property<AxisDragGizmo, bool> isEnabled;

private:
  Observer<PointerInfo>::Ptr _pointerObserver;
  bool _isEnabled;
  PositionGizmo* _parent;

  TransformNodePtr _arrow;
  StandardMaterialPtr _coloredMaterial;
  StandardMaterialPtr _hoverMaterial;

  float _currentSnapDragDistance;
  Vector3 _tmpVector;
  SnapEvent _tmpSnapEvent;
  Vector3 _localDelta;
  Matrix _tmpMatrix;

}; // end of class PositionGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_DRAG_GIZMO_H
