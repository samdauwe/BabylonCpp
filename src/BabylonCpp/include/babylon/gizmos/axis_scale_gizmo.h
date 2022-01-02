#ifndef BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
#define BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class PointerDragBehavior;
class ScaleGizmo;
FWD_CLASS_SPTR(StandardMaterial)

/**
 * @brief Single axis scale gizmo.
 */
class BABYLON_SHARED_EXPORT AxisScaleGizmo : public Gizmo {

public:
  /**
   * @brief Creates an AxisScaleGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param dragAxis The axis which the gizmo will be able to scale on
   * @param color The color of the gizmo
   * @param thickness display gizmo axis thickness
   */
  AxisScaleGizmo(const Vector3& dragAxis, const Color3& color = Color3::Gray(),
                 const UtilityLayerRendererPtr& gizmoLayer
                 = UtilityLayerRenderer::DefaultUtilityLayer(),
                 ScaleGizmo* parent = nullptr, float thickness = 1.f);
  ~AxisScaleGizmo() override; // = default

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
  void _attachedNodeChanged(const NodePtr& value) override;

  /**
   * @brief Gets if the gizmo is enabled.
   */
  [[nodiscard]] bool get_isEnabled() const;

  /**
   * @brief Sets if the gizmo is enabled.
   */
  void set_isEnabled(bool value);

private:
  /**
   * @brief Create Geometry for Gizmo.
   */
  std::tuple<MeshPtr, MeshPtr> _createGizmoMesh(const AbstractMeshPtr& parentMesh, float thickness,
                                                bool isCollider = false);

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  std::unique_ptr<PointerDragBehavior> dragBehavior;

  /**
   * Scale distance in babylon units that the gizmo will snap to when dragged (Default: 0)
   */
  float snapDistance;

  /**
   * Event that fires each time the gizmo snaps to a new location.
   * * snapDistance is the the change in distance
   */
  Observable<SnapEvent> onSnapObservable;

  /**
   * If the scaling operation should be done on all axis (default: false)
   */
  bool uniformScaling;

  /**
   * Custom sensitivity value for the drag strength
   */
  float sensitivity;

  /**
   * If the gizmo is enabled
   */
  Property<AxisScaleGizmo, bool> isEnabled;

private:
  Observer<PointerInfo>::Ptr _pointerObserver;

  bool _isEnabled;
  ScaleGizmo* _parent;

  AbstractMeshPtr _gizmoMesh;
  MeshPtr _arrowMesh;
  MeshPtr _arrowTail;
  MeshPtr _colliderArrowMesh;
  MeshPtr _colliderArrowTail;
  StandardMaterialPtr _coloredMaterial;
  StandardMaterialPtr _hoverMaterial;
  StandardMaterialPtr _disableMaterial;
  bool _dragging;
  Vector3 _tmpVector;
  Vector3 __tmpVector;
  Matrix _tmpMatrix;
  Matrix _tmpMatrix2;

  float _currentSnapDragDistance;
  SnapEvent _tmpSnapEvent;

  Vector3 _nodePosition;
  Vector3 _linePosition;
  Vector3 _lineScale;

  std::function<void(float dragDistance)> _increaseGizmoMesh;
  std::function<void()> _resetGizmoMesh;
  GizmoAxisCache _cache;

}; // end of class AxisScaleGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_AXIS_SCALE_GIZMO_H
