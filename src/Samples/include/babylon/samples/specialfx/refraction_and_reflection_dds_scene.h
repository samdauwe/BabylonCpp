#ifndef BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_DDS_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_DDS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Refraction and reflection dds scene. This example simulates how light
 * would reflect and refract with a sphere using a DDS texure as skybox.
 * @see https://www.babylonjs-playground.com/#XH85A9#0
 */
class RefractionAndReflectionDDSScene : public IRenderableScene {

public:
  RefractionAndReflectionDDSScene(ICanvas* iCanvas);
  ~RefractionAndReflectionDDSScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of class RefractionAndReflectionDDSScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_SPECIAL_FX_REFRACTION_AND_REFLECTION_DDS_SCENE_H
