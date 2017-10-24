#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_1_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_1_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulation 128 agents trying to reach there opposite
 * position.
 */
class CrowdSimulationScene1 : public IRenderableScene {

public:
  CrowdSimulationScene1(ICanvas* iCanvas);
  ~CrowdSimulationScene1();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  Mesh* createAgentsMesh(size_t agentNo, size_t totalNoAgents);

  /**
   * @brief 128 agents try to reach there opposite position.
   */
  void setupScenario();

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene1

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_1_H
