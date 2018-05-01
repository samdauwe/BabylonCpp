#ifndef BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H

#include <babylon/babylon_stl.h>

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/icamera_pov.h>

namespace BABYLON {

class Camera;

class CameraAdapter : public Adapter, public ICameraPOV {

public:
  CameraAdapter(Camera* camera);
  ~CameraAdapter() override;

  /**
   * @brief Returns the name displayed in the tree.
   */
  string_t id() override;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  string_t type() override;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  unique_ptr_t<PropertiesView>& getProperties() override;

  /**
   * @brief Returns the list of tools available for this adapter.
   */
  vector_t<AbstractTreeToolUPtr>& getTools() override;

  /**
   * @brief Set the point of view of the chosen camera.
   */
  void setPOV() override;

  string_t getCurrentActiveCamera() const override;

  string_t cameraId() const override;

private:
  Camera* _camera;
  unique_ptr_t<PropertiesView> _properties;
  vector_t<unique_ptr_t<AbstractTreeTool>> _tools;

}; // end of class CameraAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_CAMERA_ADAPTER_H
