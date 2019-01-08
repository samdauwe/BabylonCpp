#ifndef BABYLON_SAMPLES_TEXTURES_REFLECTING_ONE_FACE_SPRITE_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_REFLECTING_ONE_FACE_SPRITE_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

/**
 * @brief Reflecting One Face Sprite Texture Scene. Example demonstrating how to
 * orientate a sprite on a face with faceUV.
 * @see https://www.babylonjs-playground.com/#ICZEXW#4
 * @see https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
 */
class ReflectingOneFaceSpriteTextureScene : public IRenderableScene {

public:
  ReflectingOneFaceSpriteTextureScene(ICanvas* iCanvas);
  ~ReflectingOneFaceSpriteTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;

}; // end of class ReflectingOneFaceSpriteTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_TEXTURES_REFLECTING_ONE_FACE_SPRITE_TEXTURE_SCENE_H
