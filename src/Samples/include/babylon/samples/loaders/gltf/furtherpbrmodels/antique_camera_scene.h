#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANTIQUE_CAMERA_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANTIQUE_CAMERA_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Antique Camera Scene (glTF). Old camera model having various
 * materials.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AntiqueCamera
 */
struct AntiqueCameraScene : public IRenderableScene {

  AntiqueCameraScene(ICanvas* iCanvas);
  ~AntiqueCameraScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct AntiqueCameraScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_ANTIQUE_CAMERA_SCENE_H
