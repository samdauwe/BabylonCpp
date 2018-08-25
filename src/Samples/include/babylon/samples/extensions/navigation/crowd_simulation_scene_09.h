#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_09_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_09_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 196 agents. Four groups of agents walk
 * toward each other and intersect. Precision low.
 */
class CrowdSimulationScene09 : public IRenderableScene {

public:
  CrowdSimulationScene09(ICanvas* iCanvas);
  ~CrowdSimulationScene09() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 196 agents. Four groups of agents walk toward each other and
   * intersect. Precision low.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene09

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_09_H
