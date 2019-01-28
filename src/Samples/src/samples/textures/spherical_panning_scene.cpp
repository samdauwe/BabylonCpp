#include <babylon/samples/textures/spherical_panning_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/helpers/photo_dome.h>
#include <babylon/helpers/photo_dome_options.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace Samples {

SphericalPanningScene::SphericalPanningScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _camera{nullptr}
    , _inertialPanObserver{nullptr}
    , _sphericalPanObserver{nullptr}
    , _ptrX{0}
    , _ptrY{0}
    , _inertiaX{0}
    , _inertiaY{0}
    , _pointersDown{0}
    , _rotationAngle{0.f}
    , _matrix{Matrix::Identity()}
{
}

SphericalPanningScene::~SphericalPanningScene()
{
}

const char* SphericalPanningScene::getName()
{
  return "Spherical Panning Scene";
}

void SphericalPanningScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  PhotoDome::New("testdome", "./textures/GatonaParkWalkway1_Panorama_4Kx2K.jpg",
                 PhotoDomeOptions{
                   32u,         // resolution
                   1000u,       // size
                   std::nullopt // useDirectMapping
                 },
                 scene);

  _addSphericalPanningCameraToScene(scene, canvas);
}

void SphericalPanningScene::_addSphericalPanningCameraToScene(
  Scene* scene, ICanvas* /*canvas*/)
{
  // Set cursor to grab.
  scene->defaultCursor = "grab";

  // Add the actual camera to the scene.  Since we are going to be controlling
  // it manually, we don't attach any inputs directly to it. NOTE: We position
  // the camera at origin in this case, but it doesn't have to be there.
  // Spherical panning should work just fine regardless of the camera's
  // position.
  _camera = FreeCamera::New("camera", Vector3::Zero(), scene);

  // Ensure the camera's rotation quaternion is initialized correctly.
  _camera->rotationQuaternion
    = std::make_unique<Quaternion>(Quaternion::Identity());

  // Local state variables which will be used in the spherical pan method;
  // declared outside because they must persist from frame to frame.
  _ptrX     = 0;
  _ptrY     = 0;
  _inertiaX = 0;
  _inertiaY = 0;

  // Variables internal to spherical pan, declared here just to avoid
  // reallocating them when running.
  _priorDir      = Vector3();
  _currentDir    = Vector3();
  _rotationAxis  = Vector3();
  _rotationAngle = 0.f;
  _rotation      = Quaternion();
  _newForward    = Vector3();
  _newRight      = Vector3();
  _newUp         = Vector3();
  _matrix        = Matrix::Identity();

  // Enable/disable spherical panning depending on click state.  Note that this
  // is an extremely simplistic way to do this, so it gets a little janky on
  // multi-touch.
  _sphericalPanObserver = nullptr;
  _pointersDown         = 0;
  scene->onPointerDown  = [this](const PointerEvent& /*evt*/,
                                const std::optional<PickingInfo>& /*pickInfo*/,
                                PointerEventTypes /*type*/) {
    _pointersDown += 1;
    if (_pointersDown != 1) {
      return;
    }

    // Disable inertial panning.
    _scene->onBeforeRenderObservable.remove(_inertialPanObserver);

    // Switch cursor to grabbing.
    _scene->defaultCursor = "grabbing";

    // Store the current pointer position to clean out whatever values were left
    // in there from prior iterations.
    _ptrX = static_cast<float>(_scene->pointerX);
    _ptrY = static_cast<float>(_scene->pointerY);

    // Enable spherical panning.
    _sphericalPanObserver = _scene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) { sphericalPan(); });
  };
  scene->onPointerUp = [this](const PointerEvent& /*evt*/,
                              const std::optional<PickingInfo>& /*pickInfo*/,
                              PointerEventTypes /*type*/) {
    _pointersDown -= 1;
    if (_pointersDown != 0) {
      return;
    }

    // Switch cursor to grab.
    _scene->defaultCursor = "grab";

    // Disable spherical panning.
    _scene->onBeforeRenderObservable.remove(_sphericalPanObserver);

    // Enable inertial panning.
    _inertialPanObserver = _scene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) { inertialPan(); });
  };
}

bool SphericalPanningScene::_isNewForwardVectorTooCloseToSingularity(
  const Vector3& v) const
{
  static const auto TOO_CLOSE_TO_UP_THRESHOLD = 0.99f;
  return std::abs(Vector3::Dot(v, Vector3::Up())) > TOO_CLOSE_TO_UP_THRESHOLD;
}

void SphericalPanningScene::pan(float currX, float currY)
{
  // Compute the new forward vector first using the actual input, both X and Y.
  // If this results in a forward vector that would be too close to the
  // singularity, recompute using only the new X input, repeating the Y input
  // from the prior frame.  If either of these computations succeeds, construct
  // the new rotation matrix using the result.
  if (_computeNewForward(currX, currY) || _computeNewForward(currX, _ptrY)) {
    // We manually compute the new right and up vectors to ensure that the
    // camera only has pitch and yaw, never roll.  This dependency on the
    // world-space vertical axis is what causes the singularity described above.
    Vector3::CrossToRef(Vector3::Up(), _newForward, _newRight);
    Vector3::CrossToRef(_newForward, _newRight, _newUp);

    // Create the new world-space rotation matrix from the computed forward,
    // right, and up vectors.
    _matrix.setRowFromFloats(0, _newRight.x, _newRight.y, _newRight.z, 0.f);
    _matrix.setRowFromFloats(1, _newUp.x, _newUp.y, _newUp.z, 0.f);
    _matrix.setRowFromFloats(2, _newForward.x, _newForward.y, _newForward.z,
                             0.f);

    Quaternion::FromRotationMatrixToRef(_matrix.getRotationMatrix(),
                                        *_camera->rotationQuaternion);
  }
}

void SphericalPanningScene::_getPointerViewSpaceDirectionToRef(float x, float y,
                                                               Vector3& ref)
{
  auto identityMatrix1 = Matrix::Identity();
  auto identityMatrix2 = Matrix::Identity();

  Vector3::UnprojectToRef(Vector3(x, y, 0.f),                  //
                          static_cast<float>(_canvas->width),  //
                          static_cast<float>(_canvas->height), //
                          identityMatrix1,                     //
                          identityMatrix2,                     //
                          _camera->getProjectionMatrix(),      //
                          ref);
  ref.normalize();
}

bool SphericalPanningScene::_computeNewForward(float x, float y)
{
  _getPointerViewSpaceDirectionToRef(_ptrX, _ptrY, _priorDir);
  _getPointerViewSpaceDirectionToRef(x, y, _currentDir);

  Vector3::CrossToRef(_priorDir, _currentDir, _rotationAxis);

  // If the magnitude of the cross-product is zero, then the cursor has not
  // moved since the prior frame and there is no need to do anything.
  if (_rotationAxis.lengthSquared() > 0.f) {
    _rotationAngle
      = Vector3::GetAngleBetweenVectors(_priorDir, _currentDir, _rotationAxis);
    Quaternion::RotationAxisToRef(_rotationAxis, -_rotationAngle, _rotation);

    // Order matters here.  We create the new forward vector by applying the new
    // rotation first, then apply the camera's existing rotation.  This is
    // because, since the new rotation is computed in view space, it only makes
    // sense for a camera that is facing forward.
    _newForward.set(0.f, 0.f, 1.f);
    _newForward.rotateByQuaternionToRef(_rotation, _newForward);
    _newForward.rotateByQuaternionToRef(*_camera->rotationQuaternion,
                                        _newForward);

    return !_isNewForwardVectorTooCloseToSingularity(_newForward);
  }

  return false;
}

void SphericalPanningScene::sphericalPan()
{
  pan(static_cast<float>(_scene->pointerX),
      static_cast<float>(_scene->pointerY));

  // Store the state variables for use in the next frame.
  _inertiaX = _scene->pointerX - _ptrX;
  _inertiaY = _scene->pointerY - _ptrY;
  _ptrX     = static_cast<float>(_scene->pointerX);
  _ptrY     = static_cast<float>(_scene->pointerY);
}

void SphericalPanningScene::inertialPan()
{
  static const auto INERTIA_DECAY_FACTOR         = 0.9f;
  static const auto INERTIA_NEGLIGIBLE_THRESHOLD = 0.5f;

  if (std::abs(_inertiaX) > INERTIA_NEGLIGIBLE_THRESHOLD
      || std::abs(_inertiaY) > INERTIA_NEGLIGIBLE_THRESHOLD) {
    pan(_ptrX + _inertiaX, _ptrY + _inertiaY);

    _inertiaX *= INERTIA_DECAY_FACTOR;
    _inertiaY *= INERTIA_DECAY_FACTOR;
  }
  else {
    _scene->onBeforeRenderObservable.remove(_inertialPanObserver);
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
