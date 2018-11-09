#ifndef BABYLON_SAMPLES_TEXTURES_EQUIRECTANGULAR_MAP_AS_REFLECTION_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_EQUIRECTANGULAR_MAP_AS_REFLECTION_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Equirectangular map as reflection texture scene. Example demonstrating
 * how to use equirectangular maps as a reflection texture.
 * @see https://www.babylonjs-playground.com/#23IQHK#2
 */
struct EquirectangularMapAsReflectionTextureScene : public IRenderableScene {

  EquirectangularMapAsReflectionTextureScene(ICanvas* iCanvas);
  ~EquirectangularMapAsReflectionTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct EquirectangularMapAsReflectionTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_TEXTURES_EQUIRECTANGULAR_MAP_AS_REFLECTION_TEXTURE_SCENE_H
