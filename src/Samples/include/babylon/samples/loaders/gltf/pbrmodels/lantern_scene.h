#ifndef BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_LANTERN_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_LANTERN_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Lantern Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Lantern
 */
struct LanternScene : public IRenderableScene {

  LanternScene(ICanvas* iCanvas);
  ~LanternScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct LanternScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_LANTERN_SCENE_H
