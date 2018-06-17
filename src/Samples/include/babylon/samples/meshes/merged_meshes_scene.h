#ifndef BABYLON_SAMPLES_MESHES_MERGED_MESHES_SCENE_H
#define BABYLON_SAMPLES_MESHES_MERGED_MESHES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct Block {
  float r;
  float g;
  float b;
  float a;
}; // end of struct Block

struct MergedMeshesScene : public IRenderableScene {

  // json representation of the world
  static const char* json;

  MergedMeshesScene(ICanvas* iCanvas);
  ~MergedMeshesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct MergedMeshesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_MERGED_MESHES_SCENE_H
