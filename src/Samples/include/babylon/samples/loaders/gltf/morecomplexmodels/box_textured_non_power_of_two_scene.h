#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_NON_POWER_OF_TWO_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_NON_POWER_OF_TWO_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Textured Non Power Of Two Scene (glTF). Box with a non-power-of-2
 * (NPOT) texture. Not all implementations support NPOT textures.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxTexturedNonPowerOfTwo
 */
struct BoxTexturedNonPowerOfTwoScene : public IRenderableScene {

  BoxTexturedNonPowerOfTwoScene(ICanvas* iCanvas);
  ~BoxTexturedNonPowerOfTwoScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxTexturedNonPowerOfTwoScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_NON_POWER_OF_TWO_SCENE_H
