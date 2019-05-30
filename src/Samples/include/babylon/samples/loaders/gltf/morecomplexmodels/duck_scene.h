#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_DUCK_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_DUCK_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Duck Scene (glTF). The COLLADA duck. One texture.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Duck
 */
struct DuckScene : public IRenderableScene {

  DuckScene(ICanvas* iCanvas);
  ~DuckScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct DuckScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_DUCK_SCENE_H
