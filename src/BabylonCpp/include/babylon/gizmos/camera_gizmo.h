#ifndef BABYLON_GIZMOS_CAMERA_GIZMO_H
#define BABYLON_GIZMOS_CAMERA_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class StandardMaterial;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

/**
 * @brief Gizmo that enables viewing a camera.
 */
class BABYLON_SHARED_EXPORT CameraGizmo : public Gizmo {

public:
  /**
   * @brief Creates a CameraGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  CameraGizmo(const UtilityLayerRendererPtr& gizmoLayer
              = UtilityLayerRenderer::DefaultUtilityLayer());
  ~CameraGizmo() override; // = default

  /**
   * @brief Disposes of the light gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Updates the gizmo to match the attached mesh's position/rotation.
   * Hidden
   */
  void _update();

protected:
  /**
   * @brief Gets a boolean indicating if frustum lines must be rendered (true by default)).
   */
  bool get_displayFrustum() const;

  /**
   * @brief Sets a boolean indicating if frustum lines must be rendered (true by default)).
   */
  void set_displayFrustum(bool value);

  /**
   * @brief Gets the camera that the gizmo is attached to.
   */
  CameraPtr& get_camera();

  /**
   * @brief Sets the camera that the gizmo is attached to.
   */
  void set_camera(const CameraPtr& camera);

  /**
   * @brief Gets the material used to render the light gizmo.
   */
  StandardMaterialPtr& get_material();

private:
  static MeshPtr _CreateCameraMesh(Scene* scene);
  static MeshPtr _CreateCameraFrustum(Scene* scene);

public:
  /**
   * Event that fires each time the gizmo is clicked
   */
  Observable<Camera> onClickedObservable;

  /**
   * Gets or sets a boolean indicating if frustum lines must be rendered (true by default))
   */
  Property<CameraGizmo, bool> displayFrustum;

  /**
   * The camera that the gizmo is attached to
   */
  Property<CameraGizmo, CameraPtr> camera;

  /**
   * Gets the material used to render the camera gizmo
   */
  ReadOnlyProperty<CameraGizmo, StandardMaterialPtr> material;

private:
  MeshPtr _cameraMesh;
  MeshPtr _cameraLinesMesh;
  StandardMaterialPtr _material;
  Observer<PointerInfo>::Ptr _pointerObserver;
  CameraPtr _camera;

  // Static helper methods
  static float _Scale;
  Matrix _invProjection;

}; // end of class CameraGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_CAMERA_GIZMO_H
