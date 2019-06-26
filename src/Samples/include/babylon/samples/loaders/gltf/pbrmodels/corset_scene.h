#ifndef BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_CORSET_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_CORSET_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Corset Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Corset
 */
struct CorsetScene : public IRenderableScene {

  CorsetScene(ICanvas* iCanvas);
  ~CorsetScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CorsetScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_CORSET_SCENE_H
