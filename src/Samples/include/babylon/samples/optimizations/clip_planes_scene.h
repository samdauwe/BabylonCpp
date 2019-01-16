#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_CLIP_PLANES_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_CLIP_PLANES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

struct Slider {
  float minimum;
  float maximum;
  float value;
}; // end of struct

/**
 * @brief ClipPlanesScene. Example demonstrating how to create instances.
 * @see https://www.babylonjs-playground.com/#Y6W087#0
 * @see https://doc.babylonjs.com/how_to/clip_planes
 */
class ClipPlanesScene : public IRenderableScene {

public:
  ClipPlanesScene(ICanvas* iCanvas);
  ~ClipPlanesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  PointLightPtr _light;
  ArcRotateCameraPtr _camera;

  Slider verticalSlider;
  Slider horizontalSlider;

}; // end of class ClipPlanesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_CLIP_PLANES_SCENE_H
