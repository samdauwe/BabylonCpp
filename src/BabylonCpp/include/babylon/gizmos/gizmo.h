#ifndef BABYLON_GIZMOS_GIZMO_H
#define BABYLON_GIZMOS_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
class Node;
class Scene;
class UtilityLayerRenderer;
using AbstractMeshPtr         = std::shared_ptr<AbstractMesh>;
using MeshPtr                 = std::shared_ptr<Mesh>;
using NodePtr                 = std::shared_ptr<Node>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

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

}; // end of class Gizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_H
