#include <babylon/inspector/adapters/camera_adapter.h>

#include <babylon/cameras/camera.h>

#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/camera_pov.h>
#include <babylon/inspector/treetools/checkbox.h>

namespace BABYLON {

CameraAdapter::CameraAdapter(Camera* camera)
    : _camera{camera}, _properties{nullptr}
{
  _tools.emplace_back(::std::make_unique<CameraPOV>(this));
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

} // end of namespace BABYLON
