#ifndef BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_TWO_SIDED_PLANE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_TWO_SIDED_PLANE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Two Sided Plane Scene (glTF). A plane having the two sided material
 * parameter enabled.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see
 * https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/TwoSidedPlane
 */
struct TwoSidedPlaneScene : public IRenderableScene {

  TwoSidedPlaneScene(ICanvas* iCanvas);
  ~TwoSidedPlaneScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct TwoSidedPlaneScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_LOADERS_GLTF_FURTHER_PBR_MODELS_TWO_SIDED_PLANE_SCENE_H
