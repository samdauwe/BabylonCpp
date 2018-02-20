#ifndef BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER
#define BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IActiveMeshCandidateProvider {
  virtual vector_t<AbstractMesh*> getMeshes(Scene* scene) = 0;
  bool checksIsEnabled;
}; // end of struct IActiveMeshCandidateProvider

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_IACTIVE_MESH_CANDIDATE_PROVIDER