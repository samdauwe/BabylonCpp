#ifndef BABYLON_SAMPLES_MESHES_HEIGHT_MAP_SCENE_H
#define BABYLON_SAMPLES_MESHES_HEIGHT_MAP_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
class PointLight;
using MeshPtr       = std::shared_ptr<Mesh>;
using PointLightPtr = std::shared_ptr<PointLight>;

namespace Samples {

/**
 * @brief Height map scene. Example demonstrating how to use a height map to
 * extrude a plane to create mountains.
 * @see https://www.babylonjs-playground.com/#95PXRY#0
 */
class HeightMapScene : public IRenderableScene {

public:
  HeightMapScene(ICanvas* iCanvas);
  ~HeightMapScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _positionStepX;
  bool reverseSun;
  MeshPtr _sun;
  PointLightPtr _spot;

}; // end of class HeightMapScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_HEIGHT_MAP_SCENE_H
