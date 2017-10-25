#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_2_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_2_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulation 256 agents trying to reach there opposite
 * position.
 */
class CrowdSimulationScene2 : public IRenderableScene {

public:
  CrowdSimulationScene2(ICanvas* iCanvas);
  ~CrowdSimulationScene2();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 256 agents try to reach there opposite position.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene2

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_2_H
