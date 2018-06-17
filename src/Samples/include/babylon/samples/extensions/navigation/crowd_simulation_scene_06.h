#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_06_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_06_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class CrowdSimulation;
}

namespace Samples {

/**
 * @brief The scene class simulates 196 agents trying to reach there opposite
 * position, 4 obstacles. This simulation uses Dijkstra algorithm (user define
 * waypoints, here gray spheres) to help agents to find their way. It's more
 * fluid than the last demo, but it costs more computation time.
 */
class CrowdSimulationScene06 : public IRenderableScene {

public:
  CrowdSimulationScene06(ICanvas* iCanvas);
  ~CrowdSimulationScene06() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief 196 agents try to reach there opposite position, 4 obstacles. This
   * simulation uses Dijkstra algorithm (user define waypoints, here gray
   * spheres) to help agents to find their way. It's more fluid than the last
   * demo, but it costs more computation time.
   */
  void setupScenario(Scene* scene);

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMesh*> _agentMeshes;

}; // end of class CrowdSimulationScene06

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_SCENE_06_H
