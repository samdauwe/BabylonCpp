#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_01_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_01_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 128 agents trying to reach there opposite
 * position.
 */
class CrowdSimulationScene01 : public IRenderableScene {

public:
  CrowdSimulationScene01(ICanvas* iCanvas);
  ~CrowdSimulationScene01() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 128 agents try to reach there opposite position.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene01

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_01_H
