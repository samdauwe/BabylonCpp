#ifndef BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BOOM_BOX_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BOOM_BOX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Boom Box Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoomBox
 */
struct BoomBoxScene : public IRenderableScene {

  BoomBoxScene(ICanvas* iCanvas);
  ~BoomBoxScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoomBoxScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_PBR_MODELS_BOOM_BOX_SCENE_H
