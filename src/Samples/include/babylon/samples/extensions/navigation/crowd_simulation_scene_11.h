#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_11_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_11_H

#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 256 agents. More complex scene with Dijkstra
 * activated and containing obstacles.
 */
class CrowdSimulationScene11 : public IRenderableScene {

public:
  CrowdSimulationScene11(ICanvas* iCanvas);
  ~CrowdSimulationScene11() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 256 agents. More complex scene with Dijkstra activated and
   * containing obstacles.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene11

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_11_H
