#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_BOX_INSTANCES_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_BOX_INSTANCES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Instances Scene. Example demonstrating how to create instances.
 */
struct BoxInstancesScene : public IRenderableScene {

  BoxInstancesScene(ICanvas* iCanvas);
  ~BoxInstancesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct BoxInstancesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_BOX_INSTANCES_SCENE_H
