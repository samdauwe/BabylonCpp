#ifndef BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_CAMERAS_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_CAMERAS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cameras Scene (glTF). A sample with two different camera objects.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Cameras
 */
struct ImportCamerasScene : public IRenderableScene {

  ImportCamerasScene(ICanvas* iCanvas);
  ~ImportCamerasScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportCamerasScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_IMPORT_CAMERAS_SCENE_H
