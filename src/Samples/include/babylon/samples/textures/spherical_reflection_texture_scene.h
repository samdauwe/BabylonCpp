#ifndef BABYLON_SAMPLES_TEXTURES_SPHERICAL_REFLECTION_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_SPHERICAL_REFLECTION_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Spherical reflection texture scene. Example demonstrating how to
 * apply an image to each of four spheres, one as a diffuse texture and the
 * other three with reflectionTexture but different coordinatesMode.
 * @see https://www.babylonjs-playground.com/#20OAV9#26
 */
struct SphericalReflectionTextureScene : public IRenderableScene {

  SphericalReflectionTextureScene(ICanvas* iCanvas);
  ~SphericalReflectionTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SphericalReflectionTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_SPHERICAL_REFLECTION_TEXTURE_SCENE_H
