#ifndef BABYLON_SAMPLES_TEXTURES_SPHERICAL_PANNING_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_SPHERICAL_PANNING_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class FreeCamera;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

namespace Samples {

/**
 * @brief Spherical Panning Scene. Example demonstrating how to add spherical
 * panning behavior to a scene. The behavior is similar to the interactions from
 * viewing 3D images on Facebook, Google Maps, etc.
 * @see https://www.babylonjs-playground.com/#21UPR3#30
 * @see https://doc.babylonjs.com/snippets/sphericalpanning
 */
class SphericalPanningScene : public IRenderableScene {

public:
  SphericalPanningScene(ICanvas* iCanvas);
  ~SphericalPanningScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

  /**
   * @brief The core pan method.
   * Intuition: there exists a rotation of the camera that brings priorDir to
   * currentDir. By concatenating this rotation with the existing rotation of
   * the camera, we can move the camera so that the cursor appears to remain
   * over the same point in the scene, creating the feeling of smooth and
   * responsive 1-to-1 motion.
   */
  void pan(float currX, float currY);

  /**
   * @brief The main panning loop, to be run while the pointer is down.
   */
  void sphericalPan();

  /**
   * @brief The inertial panning loop, to be run after the pointer is released
   * until inertia runs out, or until the pointer goes down again, whichever
   * happens first.  Essentially just pretends to provide a decreasing amount of
   * input based on the last observed input, removing itself once the input
   * becomes negligible.
   */
  void inertialPan();

private:
  void _addSphericalPanningCameraToScene(Scene* scene, ICanvas* canvas);

  /**
   * @brief Add the actual camera to the scene.  Since we are going to be
   * controlling it manually, we don't attach any inputs directly to it. NOTE:
   * We position the camera at origin in this case, but it doesn't have to be
   * there. Spherical panning should work just fine regardless of the camera's
   * position.
   * @param v
   * @return
   */
  bool _isNewForwardVectorTooCloseToSingularity(const Vector3& v) const;

  /**
   * @brief Helper method to convert a screen point (in pixels) to a direction
   * in view space.
   */
  void _getPointerViewSpaceDirectionToRef(float x, float y, Vector3& ref);

  /**
   * @brief Helper method that computes the new forward direction.  This was
   * split into its own function because, near the singularity, we may to do
   * this twice in a single frame in order to reject inputs that would bring the
   * forward vector too close to vertical.
   */
  bool _computeNewForward(float x, float y);

private:
  // Scene camera
  FreeCameraPtr _camera;

  // Observers
  Observer<Scene>::Ptr _inertialPanObserver;
  Observer<Scene>::Ptr _sphericalPanObserver;

  // Local state variables which will be used in the spherical pan method;
  // declared outside because they must persist from frame to frame.
  float _ptrX;
  float _ptrY;
  float _inertiaX;
  float _inertiaY;
  int _pointersDown;

  // Variables internal to spherical pan, declared here just to avoid
  // reallocating them when running.
  Vector3 _priorDir;
  Vector3 _currentDir;
  Vector3 _rotationAxis;
  float _rotationAngle;
  Quaternion _rotation;
  Vector3 _newForward;
  Vector3 _newRight;
  Vector3 _newUp;
  Matrix _matrix;

}; // end of class SphericalPanningScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_SPHERICAL_PANNING_SCENE_H
