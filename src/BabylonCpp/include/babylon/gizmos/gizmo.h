#ifndef BABYLON_GIZMOS_GIZMO_H
#define BABYLON_GIZMOS_GIZMO_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/events/pointer_info.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Mesh)
FWD_CLASS_SPTR(Node)
FWD_CLASS_SPTR(StandardMaterial)
FWD_CLASS_SPTR(UtilityLayerRenderer)

/**
 * @brief Cache built by each axis. Used for managing state between all elements of gizmo for
 * enhanced UI.
 */
struct BABYLON_SHARED_EXPORT GizmoAxisCache {
  /** Mesh used to render the Gizmo */
  std::vector<MeshPtr> gizmoMeshes;
  /** Mesh used to detect user interaction with Gizmo */
  std::vector<MeshPtr> colliderMeshes;
  /** Material used to indicate color of gizmo mesh */
  StandardMaterialPtr material = nullptr;
  /** Material used to indicate hover state of the Gizmo */
  StandardMaterialPtr hoverMaterial = nullptr;
  /** Material used to indicate disabled state of the Gizmo */
  StandardMaterialPtr disableMaterial = nullptr;
  /** Used to indicate Active state of the Gizmo */
  bool active = false;
}; // end of struct GizmoAxisCache

/**
 * @brief Renders gizmos on top of an existing scene which provide controls for position, rotation,
 * etc.
 */
class BABYLON_SHARED_EXPORT Gizmo : public IDisposable {

public:
  /**
   * @brief Creates a gizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  Gizmo(const UtilityLayerRendererPtr& gizmoLayer);
  ~Gizmo() override; // = default

  /**
   * @brief Disposes and replaces the current meshes in the gizmo with the specified mesh.
   * @param mesh The mesh to replace the default mesh of the gizmo
   * @param useGizmoMaterial If the gizmo's default material should be used (default: false)
   */
  virtual void setCustomMesh(const MeshPtr& mesh, bool useGizmoMaterial = false);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Subscribes to pointer up, down, and hover events. Used for responsive gizmos.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param gizmoAxisCache Gizmo axis definition used for reactive gizmo UI
   * @returns {Observer<PointerInfo>} pointerObserver
   */
  static Observer<PointerInfo>::Ptr
  GizmoAxisPointerObserver(const UtilityLayerRendererPtr& gizmoLayer,
                           std::unordered_map<Mesh*, GizmoAxisCache>& gizmoAxisCache);

protected:
  /**
   * @brief Sets the ratio for the scale of the gizmo (Default: 1).
   */
  virtual void set_scaleRatio(float value);

  /**
   * @brief Sets the ratio for the scale of the gizmo (Default: 1).
   */
  [[nodiscard]] virtual float get_scaleRatio() const;

  /**
   * @brief Returns True when the mouse pointer is hovered a gizmo mesh.
   */
  virtual bool get_isHovered() const;

  /**
   * @brief Gets the mesh that the gizmo will be attached to. (eg. on a drag gizmo the mesh that
   * will be dragged).
   */
  virtual AbstractMeshPtr& get_attachedMesh();

  /**
   * @brief Sets the mesh that the gizmo will be attached to. (eg. on a drag gizmo the mesh that
   * will be dragged).
   */
  virtual void set_attachedMesh(const AbstractMeshPtr& value);

  /**
   * @brief Gets the node that the gizmo will be attached to. (eg. on a drag gizmo the mesh, bone or
   * NodeTransform that will be dragged)
   * * When set, interactions will be enabled
   */
  virtual NodePtr& get_attachedNode();

  /**
   * @brief Sets the node that the gizmo will be attached to. (eg. on a drag gizmo the mesh, bone or
   * NodeTransform that will be dragged)
   * * When set, interactions will be enabled
   */
  virtual void set_attachedNode(const NodePtr& value);

  /**
   * @brief Sets if set the gizmo's position will be updated to match the attached mesh each frame
   * (Default: true).
   */
  virtual void set_updateGizmoRotationToMatchAttachedMesh(bool value);

  /**
   * @brief Gets if set the gizmo's position will be updated to match the attached mesh each frame
   * (Default: true).
   */
  [[nodiscard]] virtual bool get_updateGizmoRotationToMatchAttachedMesh() const;

  /**
   * @brief Hidden
   */
  virtual void _attachedNodeChanged(const NodePtr& value);

  /**
   * Hidden
   * @brief Updates the gizmo to match the attached mesh's position/rotation.
   */
  void _update();

  /**
   * @brief computes the rotation/scaling/position of the transform once the Node world matrix has
   * changed.
   * @param value Node, TransformNode or mesh
   */
  void _matrixChanged();

public:
  /**
   * The root mesh of the gizmo
   */
  MeshPtr _rootMesh;

  /**
   * Ratio for the scale of the gizmo (Default: 1)
   */
  Property<Gizmo, float> scaleRatio;

  /**
   * True when the mouse pointer is hovered a gizmo mesh
   */
  ReadOnlyProperty<Gizmo, bool> isHovered;

  /**
   * Mesh that the gizmo will be attached to. (eg. on a drag gizmo the mesh that will be dragged)
   * * When set, interactions will be enabled
   */
  Property<Gizmo, AbstractMeshPtr> attachedMesh;

  /**
   * Node that the gizmo will be attached to. (eg. on a drag gizmo the mesh, bone or NodeTransform
   * that will be dragged)
   * * When set, interactions will be enabled
   */
  Property<Gizmo, NodePtr> attachedNode;

  /**
   * The utility layer the gizmo will be added to
   */
  UtilityLayerRendererPtr gizmoLayer;

  /**
   * If set the gizmo's rotation will be updated to match the attached mesh each frame (Default:
   * true)
   */
  Property<Gizmo, bool> updateGizmoRotationToMatchAttachedMesh;

  /**
   * If set the gizmo's position will be updated to match the attached mesh each frame (Default:
   * true)
   */
  bool updateGizmoPositionToMatchAttachedMesh;

  /**
   * When set, the gizmo will always appear the same size no matter where the camera is (default:
   * true)
   */
  bool updateScale;

protected:
  /**
   * Ratio for the scale of the gizmo (Default: 1)
   */
  float _scaleRatio;

  /**
   * boolean updated by pointermove when a gizmo mesh is hovered
   */
  bool _isHovered;

  /**
   * If a custom mesh has been set (Default: false)
   */
  bool _customMeshSet;

  /**
   * If set the gizmo's rotation will be updated to match the attached mesh each frame (Default:
   * true)
   */
  bool _updateGizmoRotationToMatchAttachedMesh;

  bool _interactionsEnabled;

private:
  AbstractMeshPtr _attachedMesh;
  NodePtr _attachedNode;
  Matrix _tmpMatrix;
  Observer<Scene>::Ptr _beforeRenderObserver;
  std::optional<Quaternion> _tempQuaternion;
  std::optional<Vector3> _tempVector;
  std::optional<Vector3> _tempVector2;
  Matrix _tempMatrix1;
  Matrix _tempMatrix2;
  Matrix _rightHandtoLeftHandMatrix;

}; // end of class Gizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_H
