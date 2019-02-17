#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H

#include <memory>
#include <optional>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BaseTexture;
class Scene;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

struct IScenePropertyGridComponentProps {
  Scene* scene;
}; // end of struct IScenePropertyGridComponentProps

struct PhysicsEngineSettings {
  Vector3 gravity;
  float timeStep;
}; // end of struct PhysicsEngineSettings

class BABYLON_SHARED_EXPORT ScenePropertyGridComponent {

public:
  ScenePropertyGridComponent(const IScenePropertyGridComponentProps& props);
  ~ScenePropertyGridComponent();

  void setRenderingModes(bool point, bool wireframe);
  void switchIBL();
  void updateEnvironmentTexture(const std::string& fileLocation);
  void updateGravity(const Vector3& newValue);
  void updateTimeStep(float newValue);
  void render();

public:
  IScenePropertyGridComponentProps props;

private:
  BaseTexturePtr _storedEnvironmentTexture;
  std::optional<PhysicsEngineSettings> _dummy;

}; // end of class ScenePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H
