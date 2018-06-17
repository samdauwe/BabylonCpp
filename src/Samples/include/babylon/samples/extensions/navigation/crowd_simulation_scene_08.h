#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_08_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_08_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 200 agents. Two groups of agents walk toward
 * each other and intersect. Precision high.
 */
class CrowdSimulationScene08 : public IRenderableScene {

public:
  CrowdSimulationScene08(ICanvas* iCanvas);
  ~CrowdSimulationScene08() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 200 agents. Two groups of agents walk toward each other and
   * intersect. Precision high.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene08

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_08_H
