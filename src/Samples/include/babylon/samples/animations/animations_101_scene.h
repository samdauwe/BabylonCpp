#ifndef BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_101_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_101_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class StandardMaterial;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Samples {

/**
 * @brief Animations 101 scene. Example demonstrating how to add animations.
 * @see https://www.babylonjs-playground.com/#QYFDDP#1
 * @see https://doc.babylonjs.com/babylon101/animations
 */
struct Animations101Scene : public IRenderableScene {

public:
  Animations101Scene(ICanvas* iCanvas);
  ~Animations101Scene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  high_res_time_point_t _previousTimestamp;
  StandardMaterialPtr _materialBox2;

}; // end of struct Animations101Scene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_101_SCENE_H
