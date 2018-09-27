#ifndef BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H

#include <memory>

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/icamera_pov.h>

namespace BABYLON {

class Camera;
class FreeCamera;
using CameraPtr     = std::shared_ptr<Camera>;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

class CameraAdapter : public Adapter, public ICameraPOV {

public:
  CameraAdapter(const CameraPtr& camera);
  ~CameraAdapter() override;

  /**
   * @brief Returns the name displayed in the tree.
   */
  std::string id() override;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  std::string type() override;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  std::unique_ptr<PropertiesView>& getProperties() override;

  /**
   * @brief Returns the list of tools available for this adapter.
   */
  std::vector<AbstractTreeToolUPtr>& getTools() override;

  /**
   * @brief Set the point of view of the chosen camera.
   */
  void setPOV() override;

  std::string getCurrentActiveCamera() const override;

  std::string cameraId() const override;

private:
  void _buildPropertiesView();

private:
  CameraPtr _camera;
  FreeCameraPtr _freeCamera;
  std::unique_ptr<PropertiesView> _properties;
  std::vector<std::unique_ptr<AbstractTreeTool>> _tools;

}; // end of class CameraAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H
