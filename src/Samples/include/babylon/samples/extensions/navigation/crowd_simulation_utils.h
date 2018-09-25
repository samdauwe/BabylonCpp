#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H

#include <memory>
#include <vector>

namespace BABYLON {

class AbstractMesh;
class Scene;
class Vector2;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

namespace Samples {

struct CrowdSimulationUtils {

  static std::vector<AbstractMeshPtr> createAgentMeshes(Scene* scene,
                                                        size_t totalNoAgents);
  static AbstractMeshPtr createWayPointMesh(Scene* scene,
                                            const Vector2& position);

}; // end of struct CrowdSimulationScene1

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H
