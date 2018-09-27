#ifndef BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H
#define BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

enum class GLTFLoaderCoordinateSystemMode {
  // Automatically convert the glTF right-handed data to the appropriate system
  // based on the current coordinate system mode of the scene
  // (scene.useRightHandedSystem).
  // NOTE: When scene.useRightHandedSystem is false, an additional transform
  // will be added to the root to transform the data from right-handed to
  // left-handed.
  AUTO,
  // The glTF right-handed data is not transformed in any form and is loaded
  // directly.
  PASS_THROUGH,
  // Sets the useRightHandedSystem flag on the scene.
  FORCE_RIGHT_HANDED,
}; // end of enum class GLTFLoaderCoordinateSystemMode

struct Version {
  unsigned int major;
  unsigned int minor;
}; // end of struct version

class BABYLON_SHARED_EXPORT GLTFFileLoader : public IDisposable,
                                             public ISceneLoaderPluginAsync {

public:
  void dispose();
  void importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const Variant<std::string, ArrayBuffer>& data, const std::string& rootUrl,
    const std::function<void(const std::vector<AbstractMesh*>& meshes,
                               const std::vector<ParticleSystem*>& particleSystems,
                               const std::vector<Skeleton*>& skeletons)>&
      onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    const std::function<void(const std::string& message)>& onError);
  void
  loadAsync(Scene* scene, const Variant<std::string, ArrayBuffer>& data,
            const std::string& rootUrl, const std::function<void()>& onSuccess,
            const std::function<void(const ProgressEvent& event)>& onProgress,
            const std::function<void(const std::string& message)>& onError);

private:
  bool canDirectLoad(const std::string& data);
  IGLTFLoader _getLoader(const IGLTFLoaderData& loaderData);

private:
  static IGLTFLoaderData _parse(const Variant<std::string, ArrayBuffer>& data);
  static IGLTFLoaderData _parseBinary(const ArrayBuffer& data);
  static IGLTFLoaderData _parseV1(const BinaryReader& binaryReader);
  static IGLTFLoaderData _parseV2(const BinaryReader& binaryReader);
  static Nullable<Version> _parseVersion(const std::string& version);
  static int _compareVersion(const Version& a, const Version& b);
  static std::string _decodeBufferToText(const Uint8Array& buffer);

public:
  static std::function<IGLTFLoader(const GLTFFileLoader& parent)>
    CreateGLTFLoaderV1;
  static std::function<IGLTFLoader(const GLTFFileLoader& parent)>
    CreateGLTFLoaderV2;

  // Common options
  std::function<void(const IGLTFLoaderData& data)> onParsed;

  // V1 options
  static bool HomogeneousCoordinates;
  static bool IncrementalLoading;

  // V2 options
  GLTFLoaderCoordinateSystemMode coordinateSystemMode;
  std::function<void(BaseTexture* texture)> onTextureLoaded;
  std::function<void(Material* material)> onMaterialLoaded;

  /**
   * Let the user decides if he needs to process the material (like
   * precompilation) before affecting it to meshes
   */
  std::function<void(Material* material, AbstractMesh* targetMesh, bool isLOD,
                       const std::function<void()>& callback)>
    onBeforeMaterialReadyAsync;

  /**
   * Raised when the asset is completely loaded, just before the loader is
   * disposed.
   * For assets with LODs, raised when all of the LODs are complete.
   * For assets without LODs, raised when the model is complete just after
   * onSuccess.
   */
  std::function<void()> onComplete;

  std::string name;

private:
  IGLTFLoader _loader;

}; // end of class GLTFFileLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H