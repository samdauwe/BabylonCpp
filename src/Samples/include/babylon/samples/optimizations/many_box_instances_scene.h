#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_MANY_BOX_INSTANCES_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_MANY_BOX_INSTANCES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Many box Instances Scene. Example demonstrating how to create
 * instances.
 */
struct ManyBoxInstancesScene : public IRenderableScene {

  ManyBoxInstancesScene(ICanvas* iCanvas);
  ~ManyBoxInstancesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ManyBoxInstancesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_MANY_BOX_INSTANCES_SCENE_H
