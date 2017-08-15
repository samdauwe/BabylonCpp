#ifndef BABYLON_SAMPLES_MESH_TILED_GROUND_SCENE_H
#define BABYLON_SAMPLES_MESH_TILED_GROUND_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct TiledGroundScene : public IRenderableScene {

  TiledGroundScene(ICanvas* iCanvas);
  ~TiledGroundScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TiledGroundScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_TILED_GROUND_SCENE_H
