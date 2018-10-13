#ifndef BABYLON_SAMPLES_MESHES_RAYCAST_ON_HEIGHT_MAP_SCENE_H
#define BABYLON_SAMPLES_MESHES_RAYCAST_ON_HEIGHT_MAP_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Raycast on height map scene. Example demonstrating how to raycast to
 * find positions on a heightmap.
 * @see https://www.babylonjs-playground.com/#QM57B#0
 */
class RaycastOnHeightMapScene : public IRenderableScene {

public:
  RaycastOnHeightMapScene(ICanvas* iCanvas);
  ~RaycastOnHeightMapScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _angle;
  MeshPtr _ground, _sphere;

}; // end of class RaycastOnHeightMapScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_RAYCAST_ON_HEIGHT_MAP_SCENE_H
