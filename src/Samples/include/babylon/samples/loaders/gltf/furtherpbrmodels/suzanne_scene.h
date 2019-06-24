#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SUZANNE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SUZANNE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Suzanne Scene (glTF). Suzanne from Blender with smoothed faces.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Suzanne
 */
struct SuzanneScene : public IRenderableScene {

  SuzanneScene(ICanvas* iCanvas);
  ~SuzanneScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SuzanneScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SUZANNE_SCENE_H
