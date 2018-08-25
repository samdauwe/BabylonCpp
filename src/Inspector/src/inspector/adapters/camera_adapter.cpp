#include <babylon/inspector/adapters/camera_adapter.h>

#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>

#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/camera_pov.h>
#include <babylon/inspector/treetools/checkbox.h>

namespace BABYLON {

CameraAdapter::CameraAdapter(const CameraPtr& camera)
    : _camera{camera}, _freeCamera{nullptr}, _properties{nullptr}
{
  _tools.emplace_back(::std::make_unique<CameraPOV>(this));
  // Cast camera
  if (_camera->type() == IReflect::Type::FREECAMERA) {
    _freeCamera = ::std::static_pointer_cast<FreeCamera>(_camera);
  }
  // Build properties view
  _properties = ::std::make_unique<PropertiesView>();
  _buildPropertiesView();
}

CameraAdapter::~CameraAdapter()
{
}

string_t CameraAdapter::id()
{
  return _camera->name;
}

string_t CameraAdapter::type()
{
  return _camera->getClassName();
}

unique_ptr_t<PropertiesView>& CameraAdapter::getProperties()
{
  return _properties;
}

vector_t<unique_ptr_t<AbstractTreeTool>>& CameraAdapter::getTools()
{
  return _tools;
}

void CameraAdapter::setPOV()
{
  _camera->getScene()->switchActiveCamera(_camera);
}

string_t CameraAdapter::getCurrentActiveCamera() const
{
  const auto& activeCamera = _camera->getScene()->activeCamera;
  if (activeCamera != nullptr) {
    return activeCamera->name;
  }
  else {
    return "0";
  }
}

string_t CameraAdapter::cameraId() const
{
  return _camera->name;
}

void CameraAdapter::_buildPropertiesView()
{
  // -- Add properties -- //
  auto& view = *_properties;
  /** Node properties **/
  // - id
  view.addStringProperty("id", [&]() -> string_t& { return _camera->id; },
                         [&](const string_t& value) { _camera->id = value; });
  // - name
  view.addStringProperty("name", [&]() -> string_t& { return _camera->name; },
                         [&](const string_t& value) { _camera->name = value; });
  // - doNotSerialize
  view.addBoolProperty(
    "doNotSerialize", [&]() -> bool { return _camera->doNotSerialize; },
    [&](const bool& value) { _camera->doNotSerialize = value; });
  // - uniqueId
  view.addSizeTProperty(
    "uniqueId", [&]() -> size_t { return _camera->uniqueId; },
    [&](const size_t& value) { _camera->uniqueId = value; });
  /** Camera properties **/
  // - cameraRigMode
  view.addSizeTProperty("cameraRigMode",
                        [&]() -> size_t { return _camera->cameraRigMode; },
                        [&](const size_t& value) {
                          _camera->cameraRigMode = static_cast<unsigned>(value);
                        });
  // - fov
  view.addFloatProperty("fov", [&]() -> float { return _camera->fov; },
                        [&](const float& value) { _camera->fov = value; });
  // - fovMode
  view.addSizeTProperty("fovMode", [&]() -> size_t { return _camera->fovMode; },
                        [&](const size_t& value) {
                          _camera->fovMode = static_cast<unsigned>(value);
                        });
  // - globalPosition
  view.addVector3Property(
    "globalPosition",
    [&]() -> Vector3 const& { return _camera->globalPosition(); },
    [&](const Vector3& /*value*/) {});
  // - inertia
  view.addFloatProperty("inertia", [&]() -> float { return _camera->inertia; },
                        [&](const float& value) { _camera->inertia = value; });
  // - isIntermediate
  view.addBoolProperty(
    "isIntermediate", [&]() -> bool { return _camera->isIntermediate; },
    [&](const bool& value) { _camera->isIntermediate = value; });
  // - layerMask
  view.addSizeTProperty("layerMask",
                        [&]() -> size_t { return _camera->layerMask; },
                        [&](const size_t& value) {
                          _camera->layerMask = static_cast<unsigned>(value);
                        });
  // - maxZ
  view.addFloatProperty("maxZ", [&]() -> float { return _camera->maxZ; },
                        [&](const float& value) { _camera->maxZ = value; });
  // - minZ
  view.addFloatProperty("minZ", [&]() -> float { return _camera->minZ; },
                        [&](const float& value) { _camera->minZ = value; });
  // - mode
  view.addSizeTProperty(
    "mode", [&]() -> size_t { return _camera->mode; },
    [&](const size_t& value) { _camera->mode = static_cast<unsigned>(value); });
  // position
  view.addVector3Property(
    "position", [&]() -> Vector3 const& { return _camera->position; },
    [&](const Vector3& value) { _camera->position = value; });
  // upVector
  view.addVector3Property(
    "upVector", [&]() -> Vector3 const& { return _camera->upVector; },
    [&](const Vector3& value) { _camera->upVector = value; });
  /** HemisphericLight properties **/
  if (_freeCamera) {
    /** TargetCamera properties **/
    // cameraDirection
    view.addVector3Property(
      "cameraDirection",
      [&]() -> Vector3 const& { return *_freeCamera->cameraDirection; },
      [&](const Vector3& value) { *_freeCamera->cameraDirection = value; });
    // cameraRotation
    view.addVector2Property(
      "cameraRotation",
      [&]() -> Vector2 const& { return *_freeCamera->cameraRotation; },
      [&](const Vector2& value) { *_freeCamera->cameraRotation = value; });
    // - noRotationConstraint
    view.addBoolProperty(
      "noRotationConstraint",
      [&]() -> bool { return _freeCamera->noRotationConstraint; },
      [&](const bool& value) { _freeCamera->noRotationConstraint = value; });
    // rotation
    view.addVector3Property(
      "rotation", [&]() -> Vector3 const& { return *_freeCamera->rotation; },
      [&](const Vector3& value) { *_freeCamera->rotation = value; });
    // - speed
    view.addFloatProperty(
      "speed", [&]() -> float { return _freeCamera->speed; },
      [&](const float& value) { _freeCamera->speed = value; });
    /** HemisphericLight properties **/
    // - applyGravity
    view.addBoolProperty(
      "applyGravity", [&]() -> bool { return _freeCamera->applyGravity; },
      [&](const bool& value) { _freeCamera->applyGravity = value; });
    // - checkCollisions
    view.addBoolProperty(
      "checkCollisions", [&]() -> bool { return _freeCamera->checkCollisions; },
      [&](const bool& value) { _freeCamera->checkCollisions = value; });
    // - collisionMask
    view.addIntProperty(
      "collisionMask", [&]() -> int { return _freeCamera->collisionMask(); },
      [&](const int& value) { _freeCamera->collisionMask = value; });
    // ellipsoid
    view.addVector3Property(
      "ellipsoid", [&]() -> Vector3 const& { return _freeCamera->ellipsoid; },
      [&](const Vector3& value) { _freeCamera->ellipsoid = value; });
    // ellipsoidOffset
    view.addVector3Property(
      "ellipsoidOffset",
      [&]() -> Vector3 const& { return _freeCamera->ellipsoidOffset; },
      [&](const Vector3& value) { _freeCamera->ellipsoidOffset = value; });
  }
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

} // end of namespace BABYLON
