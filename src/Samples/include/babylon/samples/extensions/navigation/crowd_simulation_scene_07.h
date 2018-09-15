#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_07_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_07_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 200 agents. Two groups of agents walk toward
 * each other and intersect. Precision low.
 */
class CrowdSimulationScene07 : public IRenderableScene {

public:
  CrowdSimulationScene07(ICanvas* iCanvas);
  ~CrowdSimulationScene07() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 200 agents. Two groups of agents walk toward each other and
   * intersect. Precision low.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene07

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_07_H
