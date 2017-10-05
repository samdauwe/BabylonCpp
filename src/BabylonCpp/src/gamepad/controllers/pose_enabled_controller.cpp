#include <babylon/gamepad/controllers/pose_enabled_controller.h>

#include <babylon/culling/ray.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/ibrowser_gamepad.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

PoseEnabledController::PoseEnabledController(
  const shared_ptr_t<IBrowserGamepad>& browserGamepad)
    : Gamepad(browserGamepad->id, browserGamepad->index, browserGamepad)
    , devicePosition{Vector3::Zero()}
    , deviceScaleFactor{1.f}
    , position{Vector3::Zero()}
    , controllerType{PoseEnabledControllerType::GENERIC}
    , _mesh{nullptr}
    , _calculatedPosition{Vector3::Zero()}

{
  type = Gamepad::POSE_ENABLED;
  Quaternion::RotationYawPitchRollToRef(Math::PI, 0, 0,
                                        _leftHandSystemQuaternion);
}

PoseEnabledController::~PoseEnabledController()
{
}

void PoseEnabledController::update()
{
  Gamepad::update();
  const auto& pose = _browserGamepad->pose;
  if (pose) {
    updateFromDevice(*pose);
  }

  if (_mesh) {
    _mesh->position().copyFrom(_calculatedPosition);
    _mesh->rotationQuaternion().copyFrom(_calculatedRotation);
  }
}

void PoseEnabledController::updateFromDevice(const DevicePose& poseData)
{
  rawPose = poseData;
  if (!poseData.position.empty()) {
    devicePosition.copyFromFloats(poseData.position[0], poseData.position[1],
                                  -poseData.position[2]);
    if (_mesh && _mesh->getScene()->useRightHandedSystem()) {
      devicePosition.z *= -1.f;
    }

    devicePosition.scaleToRef(deviceScaleFactor, _calculatedPosition);
    _calculatedPosition.addInPlace(position);
  }
  if (!poseData.orientation.empty()) {
    deviceRotationQuaternion.copyFromFloats(
      rawPose.orientation[0], rawPose.orientation[1], -rawPose.orientation[2],
      -rawPose.orientation[3]);
    if (_mesh) {
      if (_mesh->getScene()->useRightHandedSystem()) {
        deviceRotationQuaternion.z *= -1.f;
        deviceRotationQuaternion.w *= -1.f;
      }
      else {
        deviceRotationQuaternion.multiplyToRef(_leftHandSystemQuaternion,
                                               deviceRotationQuaternion);
      }
    }

    // if the camera is set, rotate to the camera's rotation
    deviceRotationQuaternion.multiplyToRef(rotationQuaternion,
                                           _calculatedRotation);
  }
}

void PoseEnabledController::attachToMesh(AbstractMesh* mesh)
{
  if (_mesh) {
    _mesh->setParent(nullptr);
  }
  _mesh = mesh;
  if (_poseControlledCamera) {
    // _mesh->setParent(_poseControlledCamera);
  }
  if (!_mesh->rotationQuaternionSet()) {
    _mesh->setRotationQuaternion(Quaternion());
  }
}

void PoseEnabledController::attachToPoseControlledCamera(TargetCamera* camera)
{
  _poseControlledCamera = camera;
  if (_mesh) {
    // _mesh->setParent(_poseControlledCamera);
  }
}

void PoseEnabledController::dispose()
{
  if (_mesh) {
    _mesh->dispose();
  }
  _mesh = nullptr;

  Gamepad::dispose();
}

AbstractMesh* PoseEnabledController::mesh()
{
  return _mesh;
}

Ray PoseEnabledController::getForwardRay(float length)
{
  if (!mesh()) {
    return Ray(Vector3::Zero(), Vector3(0.f, 0.f, 1.f), length);
  }

  auto m      = mesh()->getWorldMatrix();
  auto origin = m->getTranslation();

  Vector3 forward(0.f, 0.f, -1.f);
  auto forwardWorld = Vector3::TransformNormal(forward, *m);

  auto direction = Vector3::Normalize(forwardWorld);

  return Ray(origin, direction, length);
}

} // end of namespace BABYLON
