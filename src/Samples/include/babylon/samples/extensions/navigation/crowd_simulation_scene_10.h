#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_10_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_10_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 196 agents. Four groups of agents walk
 * toward each other and intersect. Precision medium.
 */
class CrowdSimulationScene10 : public IRenderableScene {

public:
  CrowdSimulationScene10(ICanvas* iCanvas);
  ~CrowdSimulationScene10() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 196 agents. Four groups of agents walk toward each other and
   * intersect. Precision medium.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene10

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_10_H
