#ifndef BABYLON_SAMPLES_TEXTURES_CYLINDER_WITH_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_CYLINDER_WITH_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cylinder With Texture Scene. Example demonstrating how to wrap an
 * image image around a cylinder.
 * @see https://www.babylonjs-playground.com/#VA2AC#3
 * @see https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
 */
struct CylinderWithTextureScene : public IRenderableScene {

  CylinderWithTextureScene(ICanvas* iCanvas);
  ~CylinderWithTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CylinderWithTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_CYLINDER_WITH_TEXTURE_SCENE_H
