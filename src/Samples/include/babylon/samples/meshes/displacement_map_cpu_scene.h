#ifndef BABYLON_SAMPLES_MESHES_DISPLACEMENT_MAP_CPU_SCENE_H
#define BABYLON_SAMPLES_MESHES_DISPLACEMENT_MAP_CPU_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Displacement map (CPU) Scene. Example demonstrating how to update mesh
 * geometry using a displacement map (CPU).
 * @see https://www.babylonjs-playground.com/#04JDPF#0
 */
struct DisplacementMapCPUScene : public IRenderableScene {

  DisplacementMapCPUScene(ICanvas* iCanvas);
  ~DisplacementMapCPUScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct DisplacementMapCPUScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_DISPLACEMENT_MAP_CPU_SCENE_H
