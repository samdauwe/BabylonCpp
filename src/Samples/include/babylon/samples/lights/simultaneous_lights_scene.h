#ifndef BABYLON_SAMPLES_LIGHTS_SIMULTANEOUS_LIGHTS_SCENE_H
#define BABYLON_SAMPLES_LIGHTS_SIMULTANEOUS_LIGHTS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class DirectionalLight;
class Mesh;
class PointLight;
using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
using MeshPtr             = std::shared_ptr<Mesh>;
using PointLightPtr       = std::shared_ptr<PointLight>;

namespace Samples {

/**
 * @brief Scene demonstrating the use multiple lights on a single mesh.
 * @see https://www.babylonjs-playground.com/#ZU5TKG#0
 * @see https://doc.babylonjs.com/babylon101/lights
 */
class SimultaneousLightsScene : public IRenderableScene {

public:
  SimultaneousLightsScene(ICanvas* iCanvas);
  ~SimultaneousLightsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  PointLightPtr _light0, _light1, _light2;
  DirectionalLightPtr _light3;
  MeshPtr _lightSphere0, _lightSphere1, _lightSphere2;
  float _alpha;

}; // end of class SimultaneousLightsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_SIMULTANEOUS_LIGHTS_SCENE_H
