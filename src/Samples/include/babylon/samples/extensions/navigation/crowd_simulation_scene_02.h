#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_02_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_02_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 256 agents trying to reach there opposite
 * position.
 */
class CrowdSimulationScene02 : public IRenderableScene {

public:
  CrowdSimulationScene02(ICanvas* iCanvas);
  ~CrowdSimulationScene02() override;

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

}; // end of class CrowdSimulationScene02

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_02_H
