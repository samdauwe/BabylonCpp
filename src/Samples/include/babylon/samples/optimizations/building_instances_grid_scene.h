#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_BULDING_INSTANCES_GRID_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_BULDING_INSTANCES_GRID_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class DirectionalLight;
class Mesh;
using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
using MeshPtr             = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Building Instances Grid Scene. Example demonstrating how to create a
 * large grid of boxes using instanced based rendering.
 * @see https://www.babylonjs-playground.com/#9PK6N#4
 */
class BuildingInstancesGridScene : public IRenderableScene {

public:
  BuildingInstancesGridScene(ICanvas* iCanvas);
  ~BuildingInstancesGridScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sun;
  DirectionalLightPtr _light2;
  float _alpha;

}; // end of class BuildingInstancesGridScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_BULDING_INSTANCES_GRID_SCENE_H
