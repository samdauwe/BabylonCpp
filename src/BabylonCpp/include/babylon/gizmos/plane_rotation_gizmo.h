#ifndef BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
#define BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class PointerDragBehavior;
class RotationGizmo;

/**
 * @brief Single axis scale gizmo.
 */
class BABYLON_SHARED_EXPORT PlaneRotationGizmo : public Gizmo {

public:
  /**
   * @brief Creates a PlaneRotationGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param planeNormal The normal of the plane which the gizmo will be able to rotate on
   * @param color The color of the gizmo
   * @param tessellation Amount of tessellation to be used when creating rotation circles
   * @param useEulerRotation Use and update Euler angle instead of quaternion
   * @param thickness display gizmo axis thickness
   */
  PlaneRotationGizmo(const Vector3& planeNormal, const Color3& color = Color3::Gray(),
                     const std::shared_ptr<UtilityLayerRenderer>& gizmoLayer
                     = UtilityLayerRenderer::DefaultUtilityLayer(),
                     unsigned int tessellation = 32, RotationGizmo* parent = nullptr,
                     bool useEulerRotation = false, float thickness = 1.f);
  ~PlaneRotationGizmo() override; // = default

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  void _attachedNodeChanged(const NodePtr& value) override;

  /**
   * @brief Gets If the gizmo is enabled.
   */
  [[nodiscard]] bool get_isEnabled() const;

  /**
   * @brief Sets if the gizmo is enabled.
   */
  void set_isEnabled(bool value);

private:
  /** Create Geometry for Gizmo */
  std::tuple<MeshPtr, MeshPtr> _createGizmoMesh(const AbstractMeshPtr& parentMesh, float thickness,
                                                unsigned int tessellation);
  MeshPtr setupRotationCircle(std::vector<std::vector<Vector3>>& paths,
                              const AbstractMeshPtr& parentMesh);
  void updateRotationPath(std::vector<std::vector<Vector3>>& pathArr, float newFill);
  void updateRotationCircle(const MeshPtr& mesh, std::vector<std::vector<Vector3>>& paths,
                            float newFill, const Vector3& dragPlanePoint);

public:
  /**
   * Drag behavior responsible for the gizmos dragging interactions
   */
  std::unique_ptr<PointerDragBehavior> dragBehavior;

  /**
   * Rotation distance in radians that the gizmo will snap to (Default: 0)
   */
  float snapDistance;

  /**
   * Event that fires each time the gizmo snaps to a new location.
   * * snapDistance is the the change in distance
   */
  Observable<SnapEvent> onSnapObservable;

  /**
   * The maximum angle between the camera and the rotation allowed for interaction
   * If a rotation plane appears 'flat', a lower value allows interaction.
   */
  static float MaxDragAngle;

  /**
   * Acumulated relative angle value for rotation on the axis. Reset to 0 when a dragStart occurs
   */
  float angle;

  /**
   * If the gizmo is enabled
   */
  Property<PlaneRotationGizmo, bool> isEnabled;

private:
  Observer<PointerInfo>::Ptr _pointerObserver;
  bool _isEnabled;
  RotationGizmo* _parent;
  StandardMaterialPtr _coloredMaterial;
  StandardMaterialPtr _hoverMaterial;
  StandardMaterialPtr _disableMaterial;
  MeshPtr _gizmoMesh;
  MeshPtr _rotationCircle;
  MeshPtr _rotationMesh;
  MeshPtr _collider;
  bool _dragging;
  bool _useEulerRotation;
  GizmoAxisCache _cache;

  struct CircleConstants {
    float radius;
    float pi2;
    unsigned int tessellation;
    unsigned int rotationCircleRange;
  }; // end of struct
  static CircleConstants _CircleConstants;

  // Closures for drag logic
  float _dragDistance;
  Vector3 _lastDragPosition;
  Vector3 _dragPlanePoint;
  Matrix _rotationMatrix;
  Vector3 _planeNormalTowardsCamera;
  Vector3 _localPlaneNormalTowardsCamera;

  SnapEvent _tmpSnapEvent;
  float _currentSnapDragDistance;
  Matrix _tmpMatrix;
  Vector3 _tmpVector;
  Quaternion _amountToRotate;

}; // end of class PlaneRotationGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_PLANE_ROTATION_GIZMO_H
