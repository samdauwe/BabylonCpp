#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SCI_FI_HELMET_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SCI_FI_HELMET_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Sci Fi Helmet Scene (glTF). Sci Fi Helmet having unsigned integer
 * indices.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SciFiHelmet
 */
struct SciFiHelmetScene : public IRenderableScene {

  SciFiHelmetScene(ICanvas* iCanvas);
  ~SciFiHelmetScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SciFiHelmetScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_SCI_FI_HELMET_SCENE_H
