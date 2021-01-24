#include <babylon/cameras/inputs/free_camera_mouse_wheel_input.h>

#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

FreeCameraMouseWheelInput::FreeCameraMouseWheelInput()
    : wheelXMoveRelative{this, &FreeCameraMouseWheelInput::get_wheelXMoveRelative,
                         &FreeCameraMouseWheelInput::set_wheelXMoveRelative}
    , wheelYMoveRelative{this, &FreeCameraMouseWheelInput::get_wheelYMoveRelative,
                         &FreeCameraMouseWheelInput::set_wheelYMoveRelative}
    , wheelZMoveRelative{this, &FreeCameraMouseWheelInput::get_wheelZMoveRelative,
                         &FreeCameraMouseWheelInput::set_wheelZMoveRelative}
    , wheelXRotateRelative{this, &FreeCameraMouseWheelInput::get_wheelXRotateRelative,
                           &FreeCameraMouseWheelInput::set_wheelXRotateRelative}
    , wheelYRotateRelative{this, &FreeCameraMouseWheelInput::get_wheelYRotateRelative,
                           &FreeCameraMouseWheelInput::set_wheelYRotateRelative}
    , wheelZRotateRelative{this, &FreeCameraMouseWheelInput::get_wheelZRotateRelative,
                           &FreeCameraMouseWheelInput::set_wheelZRotateRelative}
    , wheelXMoveScene{this, &FreeCameraMouseWheelInput::get_wheelXMoveScene,
                      &FreeCameraMouseWheelInput::set_wheelXMoveScene}
    , wheelYMoveScene{this, &FreeCameraMouseWheelInput::get_wheelYMoveScene,
                      &FreeCameraMouseWheelInput::set_wheelYMoveScene}
    , wheelZMoveScene{this, &FreeCameraMouseWheelInput::get_wheelZMoveScene,
                      &FreeCameraMouseWheelInput::set_wheelZMoveScene}
    , _moveRelative{Vector3::Zero()}
    , _rotateRelative{Vector3::Zero()}
    , _moveScene{Vector3::Zero()}
    , _wheelXAction{_CameraProperty::MoveRelative}
    , _wheelXActionCoordinate{Coordinate::X}
    , _wheelYAction{_CameraProperty::MoveRelative}
    , _wheelYActionCoordinate{Coordinate::Z}
    , _wheelZAction{std::nullopt}
    , _wheelZActionCoordinate{std::nullopt}
{
}

FreeCameraMouseWheelInput::~FreeCameraMouseWheelInput() = default;

Camera* FreeCameraMouseWheelInput::get_camera() const
{
  return camera;
}

std::string FreeCameraMouseWheelInput::getClassName() const
{
  return "FreeCameraMouseWheelInput";
}

std::string FreeCameraMouseWheelInput::getSimpleName() const
{
  return BaseCameraMouseWheelInput::getSimpleName();
}

void FreeCameraMouseWheelInput::set_wheelXMoveRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::MoveRelative)) {
    // Attempting to clear different _wheelXAction.
    return;
  }
  _wheelXAction           = _CameraProperty::MoveRelative;
  _wheelXActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelXMoveRelative()
{
  if (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::MoveRelative) {
    return _nullCoordinate;
  }
  return _wheelXActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelYMoveRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::MoveRelative)) {
    // Attempting to clear different _wheelYAction.
    return;
  }
  _wheelYAction           = _CameraProperty::MoveRelative;
  _wheelYActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelYMoveRelative()
{
  if (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::MoveRelative) {
    return _nullCoordinate;
  }
  return _wheelYActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelZMoveRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::MoveRelative)) {
    // Attempting to clear different _wheelZAction.
    return;
  }
  _wheelZAction           = _CameraProperty::MoveRelative;
  _wheelZActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelZMoveRelative()
{
  if (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::MoveRelative) {
    return _nullCoordinate;
  }
  return _wheelZActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelXRotateRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::RotateRelative)) {
    // Attempting to clear different _wheelXAction.
    return;
  }
  _wheelXAction           = _CameraProperty::RotateRelative;
  _wheelXActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelXRotateRelative()
{
  if (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::RotateRelative) {
    return _nullCoordinate;
  }
  return _wheelXActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelYRotateRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::RotateRelative)) {
    // Attempting to clear different _wheelYAction.
    return;
  }
  _wheelYAction           = _CameraProperty::RotateRelative;
  _wheelYActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelYRotateRelative()
{
  if (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::RotateRelative) {
    return _nullCoordinate;
  }
  return _wheelYActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelZRotateRelative(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::RotateRelative)) {
    // Attempting to clear different _wheelZAction.
    return;
  }
  _wheelZAction           = _CameraProperty::RotateRelative;
  _wheelZActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelZRotateRelative()
{
  if (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::RotateRelative) {
    return _nullCoordinate;
  }
  return _wheelZActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelXMoveScene(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::MoveScene)) {
    // Attempting to clear different _wheelXAction.
    return;
  }
  _wheelXAction           = _CameraProperty::MoveScene;
  _wheelXActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelXMoveScene()
{
  if (!_wheelXAction.has_value() || *_wheelXAction != _CameraProperty::MoveScene) {
    return _nullCoordinate;
  }
  return _wheelXActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelYMoveScene(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::MoveScene)) {
    // Attempting to clear different _wheelYAction.
    return;
  }
  _wheelYAction           = _CameraProperty::MoveScene;
  _wheelYActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelYMoveScene()
{
  if (!_wheelYAction.has_value() || *_wheelYAction != _CameraProperty::MoveScene) {
    return _nullCoordinate;
  }
  return _wheelYActionCoordinate;
}

void FreeCameraMouseWheelInput::set_wheelZMoveScene(const std::optional<Coordinate>& axis)
{
  if (!axis.has_value()
      && (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::MoveScene)) {
    // Attempting to clear different _wheelZAction.
    return;
  }
  _wheelZAction           = _CameraProperty::MoveScene;
  _wheelZActionCoordinate = axis;
}

std::optional<Coordinate>& FreeCameraMouseWheelInput::get_wheelZMoveScene()
{
  if (!_wheelZAction.has_value() || *_wheelZAction != _CameraProperty::MoveScene) {
    return _nullCoordinate;
  }
  return _wheelZActionCoordinate;
}

void FreeCameraMouseWheelInput::attachControl(bool noPreventDefault)
{
  BaseCameraMouseWheelInput::attachControl(noPreventDefault);
}

void FreeCameraMouseWheelInput::detachControl(ICanvas* ignored)
{
  BaseCameraMouseWheelInput::detachControl(ignored);
}

void FreeCameraMouseWheelInput::checkInputs()
{
  if (_wheelDeltaX == 0.f && _wheelDeltaY == 0.f && _wheelDeltaZ == 0.f) {
    return;
  }

  // Clear the camera properties that we might be updating.
  _moveRelative.setAll(0.f);
  _rotateRelative.setAll(0.f);
  _moveScene.setAll(0.f);

  // Set the camera properties that are to be updated.
  _updateCamera();

  if (camera->getScene()->useRightHandedSystem()) {
    // TODO: Does this need done for worldUpdate too?
    _moveRelative.z *= -1.f;
  }

  // Convert updates relative to camera to world position update.
  auto cameraTransformMatrix = Matrix::Zero();
  camera->getViewMatrix().invertToRef(cameraTransformMatrix);

  auto transformedDirection = Vector3::Zero();
  Vector3::TransformNormalToRef(_moveRelative, cameraTransformMatrix, transformedDirection);

  // Apply updates to camera position.
  const auto freeCamera = static_cast<FreeCamera*>(camera);
  freeCamera->cameraRotation->x += _rotateRelative.x / 200.f;
  freeCamera->cameraRotation->y += _rotateRelative.y / 200.f;
  freeCamera->cameraDirection->addInPlace(transformedDirection);
  freeCamera->cameraDirection->addInPlace(_moveScene);

  // Call the base class implementation to handle observers and do cleanup.
  BaseCameraMouseWheelInput::checkInputs();
}

void FreeCameraMouseWheelInput::_updateCamera()
{
  // Do the camera updates for each of the 3 touch-wheel axis.
  _updateCameraProperty(_wheelDeltaX, _wheelXAction, _wheelXActionCoordinate);
  _updateCameraProperty(_wheelDeltaY, _wheelYAction, _wheelYActionCoordinate);
  _updateCameraProperty(_wheelDeltaZ, _wheelZAction, _wheelZActionCoordinate);
}

void FreeCameraMouseWheelInput::_updateCameraProperty(
  float value, const std::optional<_CameraProperty>& cameraProperty,
  const std::optional<Coordinate>& coordinate)
{
  if (value == 0.f) {
    // Mouse wheel has not moved.
    return;
  }
  if (!cameraProperty.has_value() || !coordinate.has_value()) {
    // Mouse wheel axis not configured.
    return;
  }

  Vector3 action;
  switch (*cameraProperty) {
    case _CameraProperty::MoveRelative:
      action = _moveRelative;
      break;
    case _CameraProperty::RotateRelative:
      action = _rotateRelative;
      break;
    case _CameraProperty::MoveScene:
      action = _moveScene;
      break;
  }

  switch (*coordinate) {
    case Coordinate::X:
      action.set(value, 0.f, 0.f);
      break;
    case Coordinate::Y:
      action.set(0.f, value, 0.f);
      break;
    case Coordinate::Z:
      action.set(0.f, 0.f, value);
      break;
  }
}

} // end of namespace BABYLON
