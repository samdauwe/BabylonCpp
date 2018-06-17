#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_03_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_03_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 128 agents trying to reach there opposite
 * position. Agents don't know the world where they evolve. So they discover the
 * obstabcle and try to avoid it.
 */
class CrowdSimulationScene03 : public IRenderableScene {

public:
  CrowdSimulationScene03(ICanvas* iCanvas);
  ~CrowdSimulationScene03() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 128 agents trying to reach there opposite position. Agents don't
   * know the world where they evolve. So they discover the obstabcle and try to
   * avoid it.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene03

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_03_H
