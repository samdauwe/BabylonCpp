#ifndef BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BabylonFileLoader : public ISceneLoaderPlugin {

  BabylonFileLoader();
  virtual ~BabylonFileLoader();

  Material* parseMaterialById(const std::string& id,
                              const Json::value& parsedData, Scene* scene,
                              const std::string& rootUrl) const;
  bool isDescendantOf(const Json::value& mesh,
                      const std::vector<std::string>& names,
                      std::vector<std::string>& hierarchyIds);
  std::string logOperation(const std::string& operation) const;
  std::string logOperation(const std::string& operation,
                           const Json::value& producer) const;

  bool importMesh(const std::vector<std::string>& meshesNames, Scene* scene,
                  const std::string& data, const std::string& rootUrl,
                  std::vector<AbstractMesh*>& meshes,
                  std::vector<ParticleSystem*>& particleSystems,
                  std::vector<Skeleton*>& skeletons) override;
  bool load(Scene* scene, const std::string& data,
            const std::string& rootUrl) override;

}; // end of struct BabylonFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
