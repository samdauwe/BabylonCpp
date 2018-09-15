#ifndef BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER
#define BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief Interface used to let developers provide their own mesh selection
 * mechanism.
 */
struct BABYLON_SHARED_EXPORT IActiveMeshCandidateProvider {

  /**
   * @brief Return the list of active meshes.
   * @param scene defines the current scene
   * @returns the list of active meshes
   */
  virtual std::vector<AbstractMeshPtr> getMeshes(Scene* scene) = 0;

  /**
   * @brief Indicates if the meshes have been checked to make sure they are
   * isEnabled()
   */
  bool checksIsEnabled;

}; // end of struct IActiveMeshCandidateProvider

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER
