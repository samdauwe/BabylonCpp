#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_CUBE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_CUBE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cube Scene (glTF). A cube with non-smoothed faces.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Cube
 */
struct CubeScene : public IRenderableScene {

  CubeScene(ICanvas* iCanvas);
  ~CubeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CubeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_CUBE_SCENE_H
