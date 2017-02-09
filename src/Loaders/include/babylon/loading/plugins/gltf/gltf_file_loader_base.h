#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_BASE_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_BASE_H

#include <babylon/babylon_global.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_interfaces.h>

namespace BABYLON {

/**
 * @brief Implementation of the base glTF spec.
 */
struct BABYLON_SHARED_EXPORT GLTFFileLoaderBase {

  static IGLTFRuntime CreateRuntime(const picojson::value& parsedData,
                                    Scene* scene, const std::string& rootUrl);

  static void LoadBufferAsync(
    const IGLTFRuntime& gltfRuntime, const std::string& id,
    const std::function<void(const std::string& buffer)>& onSuccess,
    std::function<void()>& onError);

  static void LoadTextureBufferAsync(
    const IGLTFRuntime& gltfRuntime, const std::string& id,
    const std::function<void(const std::string& buffer)>& onSuccess,
    const std::function<void()>& onError);

  static void
  CreateTextureAsync(const IGLTFRuntime& gltfRuntime, const std::string& id,
                     const std::string& buffer,
                     const std::function<void(Texture* texture)>& onSuccess,
                     std::function<void()>& onError);

  static void LoadShaderStringAsync(
    const IGLTFRuntime& gltfRuntime, const std::string& id,
    const std::function<void(const std::string& shaderString)>& onSuccess,
    const std::function<void()>& onError);

  static void
  LoadMaterialAsync(const IGLTFRuntime& gltfRuntime, const std::string& id,
                    const std::function<void(Material* material)>& onSuccess,
                    const std::function<void()>& onError);

}; // end of struct GLTFFileLoaderBase

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_BASE_H
