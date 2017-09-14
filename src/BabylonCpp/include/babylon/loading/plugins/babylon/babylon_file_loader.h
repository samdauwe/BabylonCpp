#ifndef BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BabylonFileLoader : public ISceneLoaderPlugin {

  BabylonFileLoader();
  virtual ~BabylonFileLoader();

  Material* parseMaterialById(const string_t& id, const Json::value& parsedData,
                              Scene* scene, const string_t& rootUrl) const;
  bool isDescendantOf(const Json::value& mesh, const vector_t<string_t>& names,
                      vector_t<string_t>& hierarchyIds);
  string_t logOperation(const string_t& operation) const;
  string_t logOperation(const string_t& operation,
                        const Json::value& producer) const;

  bool importMesh(const vector_t<string_t>& meshesNames, Scene* scene,
                  const string_t& data, const string_t& rootUrl,
                  vector_t<AbstractMesh*>& meshes,
                  vector_t<ParticleSystem*>& particleSystems,
                  vector_t<Skeleton*>& skeletons) override;
  bool load(Scene* scene, const string_t& data,
            const string_t& rootUrl) override;

}; // end of struct BabylonFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
