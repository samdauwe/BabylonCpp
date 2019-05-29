#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MORPH_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MORPH_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Simple Morph Scene (glTF). A triangle with a morph animation
 * applied.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleMorph
 */
struct ImportSimpleMorphScene : public IRenderableScene {

  ImportSimpleMorphScene(ICanvas* iCanvas);
  ~ImportSimpleMorphScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportSimpleMorphScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_SIMPLE_MORPH_SCENE_H
