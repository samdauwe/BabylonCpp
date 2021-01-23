#ifndef BABYLON_GIZMOS_GIZMO_MANAGER_H
#define BABYLON_GIZMOS_GIZMO_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class BoundingBoxGizmo;
class Mesh;
class PointerInfo;
class PositionGizmo;
class RotationGizmo;
class ScaleGizmo;
class Scene;
class SixDofDragBehavior;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Node)
FWD_CLASS_SPTR(UtilityLayerRenderer)

struct Gizmos {
  std::unique_ptr<PositionGizmo> positionGizmo       = nullptr;
  std::unique_ptr<RotationGizmo> rotationGizmo       = nullptr;
  std::unique_ptr<ScaleGizmo> scaleGizmo             = nullptr;
  std::unique_ptr<BoundingBoxGizmo> boundingBoxGizmo = nullptr;
}; // end of struct Gizmos

struct GizmosEnabledSettings {
  bool positionGizmo    = false;
  bool rotationGizmo    = false;
  bool scaleGizmo       = false;
  bool boundingBoxGizmo = false;
}; // end of struct GizmosEnabledSettings

/**
 * @brief Helps setup gizmo's in the scene to rotate/scale/position nodes.
 */
class BABYLON_SHARED_EXPORT GizmoManager : public IDisposable {

public:
  /**
   * @brief Instantiates a gizmo manager.
   * @param scene the scene to overlay the gizmos on top of
   * @param thickness display gizmo axis thickness
   * @param utilityLayer the layer where gizmos are rendered
   * @param keepDepthUtilityLayer the layer where occluded gizmos are rendered
   */
  GizmoManager(Scene* scene, float thickness = 1,
               const UtilityLayerRendererPtr& utilityLayer          = nullptr,
               const UtilityLayerRendererPtr& keepDepthUtilityLayer = nullptr);
  ~GizmoManager() override; // = default

  /**
   * @brief Attaches a set of gizmos to the specified mesh.
   * @param mesh The mesh the gizmo's should be attached to
   */
  void attachToMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Attaches a set of gizmos to the specified node.
   * @param node The node the gizmo's should be attached to
   */
  void attachToNode(const NodePtr& node);

  /**
   * @brief Builds Gizmo Axis Cache to enable features such as hover state preservation and graying
   * out other axis during manipulation.
   * @param gizmoAxisCache Gizmo axis definition used for reactive gizmo UI
   */
  void addToAxisCache(const std::unordered_map<Mesh*, GizmoAxisCache>& gizmoAxisCache);

  /**
   * @brief Disposes of the gizmo manager.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Gets the utility layer that the bounding box gizmo belongs to.
   */
  UtilityLayerRendererPtr& get_keepDepthUtilityLayer();

  /**
   * @brief Gets the utility layer that all gizmos besides bounding box belong
   * to.
   */
  UtilityLayerRendererPtr& get_utilityLayer();

  /**
   * @brief Returns True when the mouse pointer is hovering a gizmo mesh.
   */
  bool get_isHovered() const;

  /**
   * @brief Sets if the position gizmo is enabled.
   */
  void set_positionGizmoEnabled(bool value);

  /**
   * @brief Gets if the position gizmo is enabled.
   */
  [[nodiscard]] bool get_positionGizmoEnabled() const;

  /**
   * @brief Sets if the rotation gizmo is enabled.
   */
  void set_rotationGizmoEnabled(bool value);

  /**
   * @brief Gets if the rotation gizmo is enabled.
   */
  [[nodiscard]] bool get_rotationGizmoEnabled() const;

  /**
   * @brief Sets if the scale gizmo is enabled.
   */
  void set_scaleGizmoEnabled(bool value);

  /**
   * @brief Gets if the scale gizmo is enabled.
   */
  [[nodiscard]] bool get_scaleGizmoEnabled() const;

  /**
   * @brief Sets if the boundingBox gizmo is enabled.
   */
  void set_boundingBoxGizmoEnabled(bool value);

  /**
   * @brief Gets if the boundingBox gizmo is enabled.
   */
  [[nodiscard]] bool get_boundingBoxGizmoEnabled() const;

private:
  /**
   * @brief Subscribes to pointer down events, for attaching and detaching mesh
   * @param scene The scene layer the observer will be added to
   */
  Observer<PointerInfo>::Ptr _attachToMeshPointerObserver(Scene* scene);

public:
  /**
   * Gizmo's created by the gizmo manager, gizmo will be null until gizmo has been enabled for the
   * first time
   */
  Gizmos gizmos;

  /**
   * When true, the gizmo will be detached from the current object when a pointer down occurs with
   * an empty picked mesh
   */
  bool clearGizmoOnEmptyPointerEvent;

  /**
   * Fires an event when the manager is attached to a mesh
   */
  Observable<AbstractMesh> onAttachedToMeshObservable;

  /**
   * Fires an event when the manager is attached to a node
   */
  Observable<Node> onAttachedToNodeObservable;

  /**
   * When bounding box gizmo is enabled, this can be used to track drag/end events
   */
  std::unique_ptr<SixDofDragBehavior> boundingBoxDragBehavior;

  /**
   * Array of meshes which will have the gizmo attached when a pointer selected them. If null, all
   * meshes are attachable. (Default: null)
   */
  std::optional<std::vector<AbstractMeshPtr>> attachableMeshes;

  /**
   * Array of nodes which will have the gizmo attached when a pointer selected them. If null, all
   * nodes are attachable. (Default: null)
   */
  std::optional<std::vector<NodePtr>> attachableNodes;

  /**
   * If pointer events should perform attaching/detaching a gizmo, if false this can be done
   * manually via attachToMesh. (Default: true)
   */
  bool usePointerToAttachGizmos;

  /**
   * Utility layer that the bounding box gizmo belongs to
   */
  ReadOnlyProperty<GizmoManager, UtilityLayerRendererPtr> keepDepthUtilityLayer;

  /**
   * Utility layer that all gizmos besides bounding box belong to
   */
  ReadOnlyProperty<GizmoManager, UtilityLayerRendererPtr> utilityLayer;

  /**
   * True when the mouse pointer is hovering a gizmo mesh
   */
  ReadOnlyProperty<GizmoManager, bool> isHovered;

  /**
   * If the position gizmo is enabled
   */
  Property<GizmoManager, bool> positionGizmoEnabled;

  /**
   * If the rotation gizmo is enabled
   */
  Property<GizmoManager, bool> rotationGizmoEnabled;

  /**
   * If the scale gizmo is enabled
   */
  Property<GizmoManager, bool> scaleGizmoEnabled;

  /**
   * If the boundingBox gizmo is enabled
   */
  Property<GizmoManager, bool> boundingBoxGizmoEnabled;

private:
  Scene* _scene;
  GizmosEnabledSettings _gizmosEnabled;
  std::vector<Observer<PointerInfo>::Ptr> _pointerObservers;
  AbstractMeshPtr _attachedMesh;
  NodePtr _attachedNode;
  Color3 _boundingBoxColor;
  UtilityLayerRendererPtr _defaultUtilityLayer;
  UtilityLayerRendererPtr _defaultKeepDepthUtilityLayer;
  float _thickness;
  /** Node Caching for quick lookup */
  std::unordered_map<Mesh*, GizmoAxisCache> _gizmoAxisCache;

}; // end of class GizmoManager

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_MANAGER_H
