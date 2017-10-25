#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_3_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_3_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulation 128 agents trying to reach there opposite
 * position. Agents don't know the world where They evolve. So They discover the
 * obstabcle and try to avoid it.
 */
class CrowdSimulationScene3 : public IRenderableScene {

public:
  CrowdSimulationScene3(ICanvas* iCanvas);
  ~CrowdSimulationScene3();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 128 agents trying to reach there opposite position. Agents don't
   * know the world where They evolve. So They discover the obstabcle and try to
   * avoid it.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene3

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_3_H
