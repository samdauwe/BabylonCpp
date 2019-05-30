#ifndef BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_INTERLEAVED_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_INTERLEAVED_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Interleaved Scene (glTF). Box example with interleaved position
 * and normal attributes.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxInterleaved
 */
struct BoxInterleavedScene : public IRenderableScene {

  BoxInterleavedScene(ICanvas* iCanvas);
  ~BoxInterleavedScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxInterleavedScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_MORE_COMPLEX_MODELS_BOX_INTERLEAVED_SCENE_H
