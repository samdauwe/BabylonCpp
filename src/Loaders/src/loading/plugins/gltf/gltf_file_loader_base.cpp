#include <babylon/loading/plugins/gltf/gltf_file_loader_base.h>

#include <babylon/core/json.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader.h>

namespace BABYLON {

IGLTFRuntime CreateRuntime(const Json::value& parsedData, Scene* scene,
                           const std::string& rootUrl)
{
  IGLTFRuntime gltfRuntime;
  gltfRuntime.buffersCount     = 0;
  gltfRuntime.shaderscount     = 0;
  gltfRuntime.scene            = scene;
  gltfRuntime.loadedBuffers    = 0;
  gltfRuntime.loadedShaders    = 0;
  gltfRuntime.rootUrl          = rootUrl;
  gltfRuntime.importOnlyMeshes = false;

  // Parse
  // 1. Parse buffers
  if (parsedData.contains("buffers")
      && parsedData.get("buffers").is<Json::object>()) {
    GLTFFileLoader::parseBuffers(parsedData.get("buffers").get<Json::object>(),
                                 gltfRuntime);
  }

  // 2. Parse bufferViews
  if (parsedData.contains("bufferViews")
      && parsedData.get("bufferViews").is<Json::object>()) {
    GLTFFileLoader::parseObject(
      parsedData.get("bufferViews").get<Json::object>(), "bufferViews",
      gltfRuntime);
  }

  // 3. Parse accessors
  if (parsedData.contains("accessors")
      && parsedData.get("accessors").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("accessors").get<Json::object>(),
                                "accessors", gltfRuntime);
  }

  // 4. Parse meshes
  if (parsedData.contains("meshes")
      && parsedData.get("meshes").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("meshes").get<Json::object>(),
                                "meshes", gltfRuntime);
  }

  // 5. Parse lights
  if (parsedData.contains("lights")
      && parsedData.get("lights").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("lights").get<Json::object>(),
                                "lights", gltfRuntime);
  }

  // 6. Parse cameras
  if (parsedData.contains("cameras")
      && parsedData.get("cameras").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("cameras").get<Json::object>(),
                                "cameras", gltfRuntime);
  }

  // 6. Parse nodes
  if (parsedData.contains("nodes")
      && parsedData.get("nodes").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("nodes").get<Json::object>(),
                                "nodes", gltfRuntime);
  }

  // 7. Parse images
  if (parsedData.contains("images")
      && parsedData.get("images").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("images").get<Json::object>(),
                                "images", gltfRuntime);
  }

  // 8. Parse textures
  if (parsedData.contains("textures")
      && parsedData.get("textures").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("textures").get<Json::object>(),
                                "textures", gltfRuntime);
  }

  // 9. Parse shaders
  if (parsedData.contains("shaders")
      && parsedData.get("shaders").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("shaders").get<Json::object>(),
                                "shaders", gltfRuntime);
  }

  // 10. Parse programs
  if (parsedData.contains("programs")
      && parsedData.get("programs").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("programs").get<Json::object>(),
                                "programs", gltfRuntime);
  }

  // 11. Parse samplers
  if (parsedData.contains("samplers")
      && parsedData.get("samplers").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("samplers").get<Json::object>(),
                                "samplers", gltfRuntime);
  }

  // 12. Parse techniques
  if (parsedData.contains("techniques")
      && parsedData.get("techniques").is<Json::object>()) {
    GLTFFileLoader::parseObject(
      parsedData.get("techniques").get<Json::object>(), "techniques",
      gltfRuntime);
  }

  // 14. Parse materials
  if (parsedData.contains("materials")
      && parsedData.get("materials").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("materials").get<Json::object>(),
                                "materials", gltfRuntime);
  }

  // 15. Parse animations
  if (parsedData.contains("animations")
      && parsedData.get("animations").is<Json::object>()) {
    GLTFFileLoader::parseObject(
      parsedData.get("animations").get<Json::object>(), "animations",
      gltfRuntime);
  }

  // 16. Parse skins
  if (parsedData.contains("skins")
      && parsedData.get("skins").is<Json::object>()) {
    GLTFFileLoader::parseObject(parsedData.get("skins").get<Json::object>(),
                                "skins", gltfRuntime);
  }

  // 17. Parse scene
  if (parsedData.contains("scene") && parsedData.contains("scenes")) {
    // gltfRuntime.currentScene = parsedData.scenes[parsedData.scene];
  }

  return gltfRuntime;
}

void LoadBufferAsync(
  const IGLTFRuntime& gltfRuntime, const std::string& id,
  const std::function<void(const std::string& buffer)>& onSuccess,
  std::function<void()>& onError)
{
}

void LoadTextureBufferAsync(
  const IGLTFRuntime& gltfRuntime, const std::string& id,
  const std::function<void(const std::string& buffer)>& onSuccess,
  const std::function<void()>& onError)
{
}

void CreateTextureAsync(const IGLTFRuntime& gltfRuntime, const std::string& id,
                        const std::string& buffer,
                        const std::function<void(Texture* texture)>& onSuccess,
                        std::function<void()>& onError)
{
}

void LoadShaderStringAsync(
  const IGLTFRuntime& gltfRuntime, const std::string& id,
  const std::function<void(const std::string& shaderString)>& onSuccess,
  const std::function<void()>& onError)
{
}

void LoadMaterialAsync(const IGLTFRuntime& gltfRuntime, const std::string& id,
                       const std::function<void(Material* material)>& onSuccess,
                       const std::function<void()>& onError)
{
}

} // end of namespace BABYLON
