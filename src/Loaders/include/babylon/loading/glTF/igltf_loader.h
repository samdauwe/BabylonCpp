#ifndef BABYLON_LOADING_GLTF_IGLTF_LOADER_H
#define BABYLON_LOADING_GLTF_IGLTF_LOADER_H

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
struct ArrayBufferView;
struct ImportedMeshes;
struct IParticleSystem;
class ProgressEvent;
class Scene;
class SceneLoaderProgressEvent;
class Skeleton;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr  = std::shared_ptr<AnimationGroup>;
using IParticleSystemPtr = std::shared_ptr<IParticleSystem>;
using SkeletonPtr        = std::shared_ptr<Skeleton>;

namespace GLTF2 {

/**
 * @brief Mode that determines the coordinate system to use.
 */
enum class GLTFLoaderCoordinateSystemMode {
  /**
   * Invalid mode
   */
  INVALID,

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
   * Invalid mode
   */
  INVALID,

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
  json jsonObject;

  /**
   * The BIN chunk of a binary glTF.
   */
  std::optional<ArrayBufferView> bin = std::nullopt;
}; // end of struct IGLTFLoaderData

/**
 * @brief Interface for extending the loader.
 */
struct IGLTFBaseLoaderExtension {
  /**
   * The name of this extension.
   */
  std::string name;

  /**
   * Defines whether this extension is enabled.
   */
  bool enabled;
}; // end of struct IGLTFBaseLoaderExtension

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

  std::optional<GLTFLoaderState> state = std::nullopt;

  virtual ImportedMeshes importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const IGLTFLoaderData& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "")
    = 0;

  virtual void loadAsync(
    Scene* scene, const IGLTFLoaderData& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "")
    = 0;

}; // end of struct IGLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_IGLTF_LOADER_H
