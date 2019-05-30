#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Scene (glTF). Example demonstrating how to easily load a glTF
 * model.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://www.babylonjs-playground.com/#FY6DP6#0
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Box
 */
struct BoxScene : public IRenderableScene {

  BoxScene(ICanvas* iCanvas);
  ~BoxScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_SCENE_H
