#ifndef BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H
#define BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/rendering/utility_layer_renderer.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Bounding box gizmo.
 */
class BABYLON_SHARED_EXPORT BoundingBoxGizmo : public Gizmo {

public:
  /**
   * @brief Creates an BoundingBoxGizmo.
   * @param color The color of the gizmo
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  BoundingBoxGizmo(const Color3& color = Color3::Gray(),
                   const shared_ptr_t<UtilityLayerRenderer>& gizmoLayer
                   = UtilityLayerRenderer::DefaultUtilityLayer());
  ~BoundingBoxGizmo() override;

  /**
   * @brief Updates the bounding box information for the Gizmo.
   */
  void updateBoundingBox();

  /**
   * @brief Enables rotation on the specified axis and disables rotation on the
   * others.
   * @param axis The list of axis that should be enabled (eg. "xy" or "xyz")
   */
  void setEnabledRotationAxis(const string_t axis);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Makes a mesh not pickable and wraps the mesh inside of a bounding
   * box mesh that is pickable. (This is useful to avoid picking within complex
   * geometry).
   * @param mesh the mesh to wrap in the bounding box mesh and make not pickable
   * @returns the bounding box mesh with the passed in mesh as a child
   */
  static MeshPtr MakeNotPickableAndWrapInBoundingBox(Mesh* mesh);

  /**
   * @brief CustomMeshes are not supported by this gizmo.
   * @param mesh The mesh to replace the default mesh of the gizmo
   */
  void setCustomMesh(Mesh* mesh) override;

protected:
  void _attachedMeshChanged(AbstractMesh* value) override;

private:
  void _selectNode(const MeshPtr& selectedMesh);
  void _recurseComputeWorld(AbstractMesh* mesh);

public:
  /**
   * The size of the rotation spheres attached to the bounding box (Default:
   * 0.1)
   */
  float rotationSphereSize;
  /**
   * The size of the scale boxes attached to the bounding box (Default: 0.1)
   */
  float scaleBoxSize;
  /**
   * If set, the rotation spheres and scale boxes will increase in size based on
   * the distance away from the camera to have a consistent screen size
   * (Default: false)
   */
  bool fixedDragMeshScreenSize;

  /**
   * The distance away from the object which the draggable meshes should appear
   * world sized when fixedDragMeshScreenSize is set to true (default: 10)
   */
  float fixedDragMeshScreenSizeDistanceFactor;
  /**
   * Fired when a rotation sphere or scale box is dragged
   */
  Observable<DragStartOrEndEvent> onDragStartObservable;
  /**
   * Fired when a rotation sphere or scale box drag is started
   */
  Observable<DragMoveEvent> onDragObservable;
  /**
   * Fired when a rotation sphere or scale box drag is needed
   */
  Observable<DragStartOrEndEvent> onDragEndObservable;

private:
  AbstractMeshPtr _lineBoundingBox;
  AbstractMeshPtr _rotateSpheresParent;
  AbstractMeshPtr _scaleBoxesParent;
  Vector3 _boundingDimensions;
  Observer<Scene>::Ptr _renderObserver;
  Observer<PointerInfo>::Ptr _pointerObserver;
  float _scaleDragSpeed;

  Quaternion _tmpQuaternion;
  Vector3 _tmpVector;

  AbstractMeshPtr _anchorMesh;
  Vector3 _existingMeshScale;

}; // end of class BoundingBoxGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H
