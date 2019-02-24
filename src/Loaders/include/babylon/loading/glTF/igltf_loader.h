#ifndef BABYLON_LOADING_GLTF_IGLTF_LOADER_H
#define BABYLON_LOADING_GLTF_IGLTF_LOADER_H

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>

using json = nlohmann::json;

namespace BABYLON {

struct ArrayBufferView;

namespace GLTF2 {

/**
 * @brief Mode that determines the coordinate system to use.
 */
enum class GLTFLoaderCoordinateSystemMode {
  /**
   * Automatically convert the glTF right-handed data to the appropriate system
   * based on the current coordinate system mode of the scene.
   */
  AUTO,

  /**
   * Sets the useRightHandedSystem flag on the scene.
   */
  FORCE_RIGHT_HANDED,
}; // end of enum class GLTFLoaderCoordinateSystemMode

/**
 * @brief Mode that determines what animations will start.
 */
enum class GLTFLoaderAnimationStartMode {
  /**
   * No animation will start.
   */
  NONE,

  /**
   * The first animation will start.
   */
  FIRST,

  /**
   * All animations will start.
   */
  ALL,
}; // end of enum class GLTFLoaderAnimationStartMode

/**
 * @brief Interface that contains the data for the glTF asset.
 */
struct IGLTFLoaderData {
  /**
   * Object that represents the glTF JSON.
   */
  json object;

  /**
   * The BIN chunk of a binary glTF.
   */
  std::optional<ArrayBufferView> bin;
}; // end of struct IGLTFLoaderData

/**
 * @brief Interface for extending the loader.
 */
struct IGLTFLoaderExtension {
  /**
   * The name of this extension.
   */
  std::string name;

  /**
   * Defines whether this extension is enabled.
   */
  bool enabled;
}; // end of struct IGLTFLoaderExtension

/**
 * @brief Loader state.
 */
enum class GLTFLoaderState {
  /**
   * The asset is loading.
   */
  LOADING,

  /**
   * The asset is ready for rendering.
   */
  READY,

  /**
   * The asset is completely loaded.
   */
  COMPLETE
}; // end of enum class GLTFLoaderState

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IGLTFLoader : public IDisposable {

  virtual void importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const IGLTFLoaderData& data, const std::string& rootUrl,
    const std::function<
      void(const std::vector<AbstractMesh*>& meshes,
           const std::vector<ParticleSystem*>& particleSystems,
           const std::vector<Skeleton*>& skeletons)>& onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    const std::function<void(const std::string& message)>& onError)
    = 0;

  virtual void
  loadAsync(Scene* scene, const IGLTFLoaderData& data,
            const std::string& rootUrl, const std::function<void()>& onSuccess,
            const std::function<void(const ProgressEvent& event)>& onProgress,
            const std::function<void(const std::string& message)>& onError)
    = 0;

}; // end of struct IGLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_IGLTF_LOADER_H
