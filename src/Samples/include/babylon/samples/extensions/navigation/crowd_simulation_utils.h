#ifndef BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H
#define BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Samples {

struct CrowdSimulationUtils {

  static vector_t<AbstractMesh*> createAgentMeshes(Scene* scene,
                                                   size_t totalNoAgents);

}; // end of struct CrowdSimulationScene1

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CROWD_SIMULATION_UTILS_H
