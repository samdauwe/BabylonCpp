#ifndef BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/loading/iscene_loader_plugin.h>

using json = nlohmann::json;

namespace BABYLON {

class Material;
using MaterialPtr = std::shared_ptr<Material>;

struct BABYLON_SHARED_EXPORT BabylonFileLoader : public ISceneLoaderPlugin {

  BabylonFileLoader();
  virtual ~BabylonFileLoader(); // = default

  MaterialPtr parseMaterialById(const std::string& id, const json& parsedData,
                                Scene* scene, const std::string& rootUrl) const;
  bool isDescendantOf(const json& mesh, const std::vector<std::string>& names,
                      std::vector<std::string>& hierarchyIds) const;
  std::string logOperation(const std::string& operation) const;
  std::string logOperation(const std::string& operation,
                           const json& producer) const;
  void loadDetailLevels(Scene* scene, const AbstractMeshPtr& mesh) const;

  bool
  importMesh(const std::vector<std::string>& meshesNames, Scene* scene,
             const std::string& data, const std::string& rootUrl,
             std::vector<AbstractMeshPtr>& meshes,
             std::vector<IParticleSystemPtr>& particleSystems,
             std::vector<SkeletonPtr>& skeletons,
             const std::function<void(const std::string& message,
                                      const std::string& exception)>& onError
             = nullptr) const override;
  bool load(Scene* scene, const std::string& data, const std::string& rootUrl,
            const std::function<void(const std::string& message,
                                     const std::string& exception)>& onError
            = nullptr) const override;
  AssetContainerPtr loadAssetContainer(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr,
    bool addToScene = false) const override;
  void finally(const std::string& producer, const std::ostringstream& log,
               const json& parsedData) const;

}; // end of struct BabylonFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
