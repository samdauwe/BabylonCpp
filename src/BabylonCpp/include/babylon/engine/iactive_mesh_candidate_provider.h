#ifndef BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER
#define BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER

#include <babylon/babylon_global.h>

namespace BABYLON {

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
  virtual vector_t<AbstractMesh*> getMeshes(Scene* scene) = 0;

  /**
   * @brief Indicates if the meshes have been checked to make sure they are
   * isEnabled()
   */
  bool checksIsEnabled;

}; // end of struct IActiveMeshCandidateProvider

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER
