#ifndef BABYLON_GIZMOS_GIZMO_MANAGER_H
#define BABYLON_GIZMOS_GIZMO_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

struct Gizmos {
  unique_ptr_t<PositionGizmo> positionGizmo       = nullptr;
  unique_ptr_t<RotationGizmo> rotationGizmo       = nullptr;
  unique_ptr_t<ScaleGizmo> scaleGizmo             = nullptr;
  unique_ptr_t<BoundingBoxGizmo> boundingBoxGizmo = nullptr;
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
  void attachToMesh(const AbstractMeshPtr& mesh);

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
   * Array of meshes which will have the gizmo attached when a pointer selected
   * them. If null, all meshes are attachable. (Default: null)
   */
  nullable_t<vector_t<AbstractMeshPtr>> attachableMeshes;

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
  AbstractMeshPtr _attachedMesh;
  Color3 _boundingBoxColor;
  unique_ptr_t<SixDofDragBehavior> _dragBehavior;

}; // end of class GizmoManager

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_MANAGER_H
