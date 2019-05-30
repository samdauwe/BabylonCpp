#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Textured Scene (glTF). Box with one texture. Start with this to
 * test textures.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxTextured
 */
struct BoxTexturedScene : public IRenderableScene {

  BoxTexturedScene(ICanvas* iCanvas);
  ~BoxTexturedScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxTexturedScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_TEXTURED_SCENE_H
