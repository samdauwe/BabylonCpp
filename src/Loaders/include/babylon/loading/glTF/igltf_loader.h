#ifndef BABYLON_LOADING_GLTF_IGLTF_LOADER_H
#define BABYLON_LOADING_GLTF_IGLTF_LOADER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

struct BABYLON_SHARED_EXPORT IGLTFLoader : public IDisposable {

  virtual void importMeshAsync(
    const vector_t<string_t>& meshesNames, Scene* scene,
    const IGLTFLoaderData& data, const string_t& rootUrl,
    const ::std::function<void(const vector_t<AbstractMesh*>& meshes,
                               const vector_t<ParticleSystem*>& particleSystems,
                               const vector_t<Skeleton*>& skeletons)>&
      onSuccess,
    const ::std::function<void(const ProgressEvent& event)>& onProgress,
    const ::std::function<void(const string_t& message)>& onError)
    = 0;

  virtual void
  loadAsync(Scene* scene, const IGLTFLoaderData& data, const string_t& rootUrl,
            const ::std::function<void()>& onSuccess,
            const ::std::function<void(const ProgressEvent& event)>& onProgress,
            const ::std::function<void(const string_t& message)>& onError)
    = 0;

}; // end of struct IGLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_IGLTF_LOADER_H