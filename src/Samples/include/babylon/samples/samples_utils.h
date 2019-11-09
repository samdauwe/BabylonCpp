#ifndef BABYLON_SAMPLES_UTILS_H
#define BABYLON_SAMPLES_UTILS_H

#include <memory>
#include <vector>

namespace BABYLON {

class AbstractMesh;
class Mesh;
class Scene;
class Vector2;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Class containing crowd simulation helper functions.
 */
struct CrowdSimulationUtils {

  static std::vector<AbstractMeshPtr> createAgentMeshes(Scene* scene, size_t totalNoAgents);
  static AbstractMeshPtr createWayPointMesh(Scene* scene, const Vector2& position);

}; // end of struct SamplesUtils

/**
 * @brief Class containing mesh helper functions.
 */
struct MeshUtils {

  /**
   * @brief Creates a 12 Pointed Star.
   */
  static MeshPtr CreateTwelvePointedStar(size_t len, Scene* scene);

}; // end of struct MeshUtils

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_UTILS_H
