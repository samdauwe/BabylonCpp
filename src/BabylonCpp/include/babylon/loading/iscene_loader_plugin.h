#ifndef BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
#define BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneLoaderPlugin {
  std::pair<std::string, bool> extensions; // Extension name, isBinary
  virtual bool importMesh(const std::vector<std::string>& meshesNames,
                          Scene* scene, const std::string& data,
                          const std::string& rootUrl,
                          std::vector<AbstractMesh*>& meshes,
                          std::vector<ParticleSystem*>& particleSystems,
                          std::vector<Skeleton*>& skeletons)
    = 0;
  virtual bool load(Scene* scene, const std::string& data,
                    const std::string& rootUrl)
    = 0;
}; // end of struct ISceneLoaderPlugin

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_PLUGIN_H
