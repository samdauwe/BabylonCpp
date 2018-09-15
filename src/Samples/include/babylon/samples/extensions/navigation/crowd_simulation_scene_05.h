#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_05_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_05_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 196 agents trying to reach there opposite
 * position, 4 obstacles. Agents don't know the world where They evolve. So they
 * discover the obstacles and try to avoid them.
 */
class CrowdSimulationScene05 : public IRenderableScene {

public:
  CrowdSimulationScene05(ICanvas* iCanvas);
  ~CrowdSimulationScene05() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 196 agents try to reach there opposite position, 4 obstacles. Agents
   * don't know the world where They evolve. So They discover the obstacles and
   * try to avoid them.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene05

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_05_H
