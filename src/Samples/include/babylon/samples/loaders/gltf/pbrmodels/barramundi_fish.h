#ifndef BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BARRAMUNDI_FISH_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BARRAMUNDI_FISH_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Barramundi Fish (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BarramundiFish
 */
struct BarramundiFishScene : public IRenderableScene {

  BarramundiFishScene(ICanvas* iCanvas);
  ~BarramundiFishScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BarramundiFishScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BARRAMUNDI_FISH_SCENE_H
