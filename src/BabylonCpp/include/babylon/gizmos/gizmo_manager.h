#ifndef BABYLON_GIZMOS_GIZMO_MANAGER_H
#define BABYLON_GIZMOS_GIZMO_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class AbstractMesh;
class BoundingBoxGizmo;
class PointerInfo;
class PositionGizmo;
class RotationGizmo;
class ScaleGizmo;
class Scene;
class SixDofDragBehavior;
class UtilityLayerRenderer;
using AbstractMeshPtr         = std::shared_ptr<AbstractMesh>;
using UtilityLayerRendererPtr = std::shared_ptr<UtilityLayerRenderer>;

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
 * @brief Helps setup gizmo's in the scene to rotate/scale/position meshes.
 */
class BABYLON_SHARED_EXPORT GizmoManager : public IDisposable {

public:
  /**
   * @brief Instatiates a gizmo manager.
   * @param scene the scene to overlay the gizmos on top of
   */
  GizmoManager(Scene* scene);
  virtual ~GizmoManager();

  /**
   * @brief Attaches a set of gizmos to the specified mesh.
   * @param mesh The mesh the gizmo's should be attached to
   */
  void attachToMesh(AbstractMesh* mesh);

  /**
   * @brief Disposes of the gizmo manager.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief If the position gizmo is enabled.
   */
  void set_positionGizmoEnabled(bool value);
  bool get_positionGizmoEnabled() const;

  /**
   * @brief If the rotation gizmo is enabled.
   */
  void set_rotationGizmoEnabled(bool value);
  bool get_rotationGizmoEnabled() const;

  /**
   * @brief If the scale gizmo is enabled.
   */
  void set_scaleGizmoEnabled(bool value);
  bool get_scaleGizmoEnabled() const;

  /**
   * @brief If the boundingBox gizmo is enabled.
   */
  void set_boundingBoxGizmoEnabled(bool value);
  bool get_boundingBoxGizmoEnabled() const;

public:
  /**
   * Gizmo's created by the gizmo manager, gizmo will be null until gizmo has
   * been enabled for the first time
   */
  Gizmos gizmos;

  /**
   * When bounding box gizmo is enabled, this can be used to track drag/end
   * events
   */
  std::unique_ptr<SixDofDragBehavior> boundingBoxDragBehavior;

  /**
   * Array of meshes which will have the gizmo attached when a pointer selected
   * them. If null, all meshes are attachable. (Default: null)
   */
  std::optional<std::vector<AbstractMeshPtr>> attachableMeshes;

  /**
   * If pointer events should perform attaching/detaching a gizmo, if false this
   * can be done manually via attachToMesh. (Default: true)
   */
  bool usePointerToAttachGizmos;

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
  Observer<PointerInfo>::Ptr _pointerObserver;
  AbstractMesh* _attachedMesh;
  Color3 _boundingBoxColor;
  UtilityLayerRendererPtr _defaultUtilityLayer;
  UtilityLayerRendererPtr _defaultKeepDepthUtilityLayer;

}; // end of class GizmoManager

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_MANAGER_H
