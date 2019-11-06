#ifndef BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H
#define BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/loading/glTF/igltf_loader.h>
#include <babylon/loading/iscene_loader_plugin_async.h>
#include <babylon/loading/iscene_loader_plugin_factory.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class AbstractMesh;
class BaseTexture;
class Camera;
struct ISceneLoaderPluginAsync;
class Material;
using AbstractMeshPtr            = std::shared_ptr<AbstractMesh>;
using BaseTexturePtr             = std::shared_ptr<BaseTexture>;
using CameraPtr                  = std::shared_ptr<Camera>;
using MaterialPtr                = std::shared_ptr<Material>;
using ISceneLoaderPluginAsyncPtr = std::shared_ptr<ISceneLoaderPluginAsync>;

namespace GLTF2 {

class BinaryReader;
struct IGLTFLoader;
struct IGLTFLoaderData;
struct IGLTFLoaderExtension;
struct IGLTFValidationResults;
using IGLTFLoaderPtr = std::shared_ptr<IGLTFLoader>;

struct UnpackedBinary {
  std::string json                   = "";
  std::optional<ArrayBufferView> bin = std::nullopt;
}; // end of struct UnpackedBinary

struct Version {
  unsigned int major;
  unsigned int minor;
}; // end of struct Version

class BABYLON_SHARED_EXPORT GLTFFileLoader : public IDisposable,
                                             public ISceneLoaderPluginAsync,
                                             public ISceneLoaderPluginFactory {

public:
  static void RegisterAsSceneLoaderPlugin();
  /** Hidden */
  static IGLTFLoaderPtr _CreateGLTF2Loader(GLTFFileLoader& parent);

private:
  static const std::string _logSpaces;

public:
  GLTFFileLoader();
  virtual ~GLTFFileLoader() override;

  /**
   * @brief Disposes the loader, releases resources during load, and cancels any
   * outstanding requests.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Hidden
   */
  void _clear();

  /**
   * @brief Imports one or more meshes from the loaded glTF data and adds them
   * to the scene.
   * @param meshesNames a string or array of strings of the mesh names that
   * should be loaded from the file
   * @param scene the scene the meshes should be added to
   * @param data the glTF data to load
   * @param rootUrl root url to load from
   * @param onProgress event that fires when loading progress has occured
   * @param fileName Defines the name of the file to load
   * @returns a promise containg the loaded meshes, particles, skeletons and
   * animations
   */
  ImportedMeshes importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "") override;

  /**
   * @brief Imports all objects from the loaded glTF data and adds them to the
   * scene.
   * @param scene the scene the objects should be added to
   * @param data the glTF data to load
   * @param rootUrl root url to load from
   * @param onProgress event that fires when loading progress has occured
   * @param fileName Defines the name of the file to load
   * @returns a promise which completes when objects have been loaded to the
   * scene
   */
  void loadAsync(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "") override;

  /**
   * @brief Load into an asset container.
   * @param scene The scene to load into
   * @param data The data to import
   * @param rootUrl The root url for scene and resources
   * @param onProgress The callback when the load progresses
   * @param fileName Defines the name of the file to load
   * @returns The loaded asset container
   */
  AssetContainerPtr loadAssetContainerAsync(
    Scene* scene, const std::string& data, const std::string& rootUrl,
    const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
    = nullptr,
    const std::string& fileName = "") override;

  /**
   * @brief Instantiates a glTF file loader plugin.
   * @returns the created plugin
   */
  std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>
  createPlugin() override;

  /**
   * @brief The loader state or null if the loader is not active.
   */
  std::optional<GLTFLoaderState> loaderState();

  /**
   * @brief Returns a promise that resolves when the asset is completely loaded.
   * @returns a promise that resolves when the asset is completely loaded.
   */
  void whenCompleteAsync();

  /**
   * @brief Hidden
   */
  std::function<void(const std::string& message)> _log;

  /**
   * @brief Hidden
   */
  void _logOpen(const std::string& message);

  /**
   * @brief Hidden
   */
  void _logClose();

  /**
   * @brief Hidden
   */
  std::function<void(const std::string& counterName)> _startPerformanceCounter;

  /**
   * @brief Hidden
   */
  std::function<void(const std::string& counterName)> _endPerformanceCounter;

protected:
  /**
   * @brief Raised when the asset has been parsed.
   */
  void set_onParsed(const std::function<void(IGLTFLoaderData* loaderData,
                                             EventState& es)>& callback);

  /**
   * @brief Callback raised when the loader creates a mesh after parsing the
   * glTF properties of the mesh.
   */
  void set_onMeshLoaded(const std::function<void(AbstractMesh* callback,
                                                 EventState& es)>& callback);

  /**
   * @brief Callback raised when the loader creates a texture after parsing the
   * glTF properties of the texture.
   */
  void set_onTextureLoaded(
    const std::function<void(BaseTexture* texture, EventState& es)>& callback);

  /**
   * @brief Callback raised when the loader creates a material after parsing the
   * glTF properties of the material.
   */
  void set_onMaterialLoaded(
    const std::function<void(Material* material, EventState& es)>& callback);

  /**
   * @brief Callback raised when the loader creates a camera after parsing the
   * glTF properties of the camera.
   */
  void set_onCameraLoaded(
    const std::function<void(Camera* camera, EventState& es)>& callback);

  /**
   * @brief Callback raised when the asset is completely loaded, immediately
   * before the loader is disposed. For assets with LODs, raised when all of the
   * LODs are complete. For assets without LODs, raised when the model is
   * complete, immediately after the loader resolves the returned promise.
   */
  void
  set_onComplete(const std::function<void(void*, EventState& es)>& callback);

  /**
   * @brief Callback raised when an error occurs.
   */
  void set_onError(
    const std::function<void(std::string* reason, EventState& es)>& callback);

  /**
   * @brief Callback raised after the loader is disposed.
   */
  void
  set_onDispose(const std::function<void(void*, EventState& es)>& callback);

  /**
   * @brief Callback raised after a loader extension is created.
   */
  void set_onExtensionLoaded(
    const std::function<void(IGLTFLoaderExtension* extension, EventState& es)>&
      callback);

  /**
   * @brief Defines if the loader logging is enabled.
   */
  bool get_loggingEnabled() const;
  void set_loggingEnabled(bool value);

  /**
   * @brief Defines if the loader should capture performance counters.
   */
  bool get_capturePerformanceCounters() const;
  void set_capturePerformanceCounters(bool value);

  /**
   * @brief Callback raised after a loader extension is created.
   */
  void set_onValidated(const std::function<void(IGLTFValidationResults* results,
                                                EventState& es)>& callback);

private:
  IGLTFLoaderData
  _parseAsync(Scene* scene, const std::variant<std::string, ArrayBuffer>& data,
              const std::string& rootUrl, const std::string& fileName = "");
  void _validateAsync(Scene* scene, const std::string& json,
                      const std::string& rootUrl,
                      const std::string& fileName = "");
  IGLTFLoaderPtr _getLoader(const IGLTFLoaderData& loaderData);
  UnpackedBinary _unpackBinary(const ArrayBuffer& data);
  UnpackedBinary _unpackBinaryV1(BinaryReader& binaryReader) const;
  UnpackedBinary _unpackBinaryV2(BinaryReader& binaryReader) const;
  static std::optional<Version> _parseVersion(const std::string& version);
  static int _compareVersion(const Version& a, const Version& b);
  static std::string _decodeBufferToText(const Uint8Array& buffer);
  void _logEnabled(const std::string& message);
  void _logDisabled(const std::string& message);
  void _startPerformanceCounterEnabled(const std::string& counterName);
  void _startPerformanceCounterDisabled(const std::string& counterName);
  void _endPerformanceCounterEnabled(const std::string& counterName);
  void _endPerformanceCounterDisabled(const std::string& counterName);

public:
  // --------------
  // Common options
  // --------------

  /**
   * Raised when the asset has been parsed
   */
  Observable<IGLTFLoaderData> onParsedObservable;

  /**
   * Raised when the asset has been parsed
   */
  WriteOnlyProperty<
    GLTFFileLoader,
    std::function<void(IGLTFLoaderData* loaderData, EventState& es)>>
    onParsed;

  // ----------
  // V1 options
  // ----------

  /**
   * Set this property to false to disable incremental loading which delays the
   * loader from calling the success callback until after loading the meshes and
   * shaders. Textures always loads asynchronously. For example, the success
   * callback can compute the bounding information of the loaded meshes when
   * incremental loading is disabled. Defaults to true.
   * @hidden
   */
  static bool IncrementalLoading;

  /**
   * Set this property to true in order to work with homogeneous coordinates,
   * available with some converters and exporters. Defaults to false. See
   * https://en.wikipedia.org/wiki/Homogeneous_coordinates.
   * @hidden
   */
  static bool HomogeneousCoordinates;

  // ----------
  // V2 options
  // ----------

  /**
   * The coordinate system mode. Defaults to AUTO.
   */
  GLTFLoaderCoordinateSystemMode coordinateSystemMode;

  /**
   * The animation start mode. Defaults to FIRST.
   */
  GLTFLoaderAnimationStartMode animationStartMode;

  /**
   * Defines if the loader should compile materials before raising the success
   * callback. Defaults to false.
   */
  bool compileMaterials;

  /**
   * Defines if the loader should also compile materials with clip planes.
   * Defaults to false.
   */
  bool useClipPlane;

  /**
   * Defines if the loader should compile shadow generators before raising the
   * success callback. Defaults to false.
   */
  bool compileShadowGenerators;

  /**
   * Defines if the Alpha blended materials are only applied as coverage.
   * If false, (default) The luminance of each pixel will reduce its opacity to
   * simulate the behaviour of most physical materials. If true, no extra
   * effects are applied to transparent pixels.
   */
  bool transparencyAsCoverage;

  /**
   * Function called before loading a url referenced by the asset.
   */
  std::function<std::string(const std::string& url)> preprocessUrlAsync;

  /**
   * Observable raised when the loader creates a mesh after parsing the glTF
   * properties of the mesh.
   */
  Observable<AbstractMesh> onMeshLoadedObservable;

  /**
   * Callback raised when the loader creates a mesh after parsing the glTF
   * properties of the mesh.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(AbstractMesh* mesh, EventState& es)>>
    onMeshLoaded;

  /**
   * Observable raised when the loader creates a texture after parsing the glTF
   * properties of the texture.
   */
  Observable<BaseTexture> onTextureLoadedObservable;

  /**
   * Callback raised when the loader creates a texture after parsing the
   * glTF properties of the texture.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(BaseTexture* texture, EventState& es)>>
    onTextureLoaded;

  /**
   * Observable raised when the loader creates a material after parsing the glTF
   * properties of the material.
   */
  Observable<Material> onMaterialLoadedObservable;

  /**
   * Callback raised when the loader creates a material after parsing the
   * glTF properties of the material.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(Material* material, EventState& es)>>
    onMaterialLoaded;

  /**
   * Observable raised when the loader creates a camera after parsing the glTF
   * properties of the camera.
   */
  Observable<Camera> onCameraLoadedObservable;

  /**
   * Callback raised when the loader creates a camera after parsing the glTF
   * properties of the camera.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(Camera* camera, EventState& es)>>
    onCameraLoaded;

  /**
   * Observable raised when the asset is completely loaded, immediately before
   * the loader is disposed. For assets with LODs, raised when all of the LODs
   * are complete. For assets without LODs, raised when the model is complete,
   * immediately after the loader resolves the returned promise.
   */
  Observable<void> onCompleteObservable;

  /**
   * Callback raised when the asset is completely loaded, immediately
   * before the loader is disposed. For assets with LODs, raised when all of the
   * LODs are complete. For assets without LODs, raised when the model is
   * complete, immediately after the loader resolves the returned promise.
   */
  WriteOnlyProperty<GLTFFileLoader, std::function<void(void*, EventState& es)>>
    onComplete;

  /**
   * Observable raised when an error occurs.
   */
  Observable<std::string> onErrorObservable;

  /**
   * Callback raised when an error occurs.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(std::string* reason, EventState& es)>>
    onError;

  /**
   * Observable raised after the loader is disposed.
   */
  Observable<void> onDisposeObservable;

  /**
   * Callback raised after the loader is disposed.
   */
  WriteOnlyProperty<GLTFFileLoader, std::function<void(void*, EventState& es)>>
    onDispose;

  /**
   * Observable raised after a loader extension is created.
   * Set additional options for a loader extension in this event.
   */
  Observable<IGLTFLoaderExtension> onExtensionLoadedObservable;

  /**
   * Callback raised after a loader extension is created.
   */
  WriteOnlyProperty<
    GLTFFileLoader,
    std::function<void(IGLTFLoaderExtension* extension, EventState& es)>>
    onExtensionLoaded;

  /**
   * Defines if the loader logging is enabled.
   */
  Property<GLTFFileLoader, bool> loggingEnabled;

  /**
   * Defines if the loader should capture performance counters.
   */
  Property<GLTFFileLoader, bool> capturePerformanceCounters;

  /**
   * Defines if the loader should validate the asset.
   */
  bool validate;

  /**
   * Observable raised after validation when validate is set to true. The event
   * data is the result of the validation.
   */
  Observable<IGLTFValidationResults> onValidatedObservable;

  /**
   * Callback raised after a loader extension is created.
   */
  WriteOnlyProperty<
    GLTFFileLoader,
    std::function<void(IGLTFValidationResults* results, EventState& es)>>
    onValidated;

private:
  Observer<IGLTFLoaderData>::Ptr _onParsedObserver;
  Observer<AbstractMesh>::Ptr _onMeshLoadedObserver;
  Observer<BaseTexture>::Ptr _onTextureLoadedObserver;
  Observer<Material>::Ptr _onMaterialLoadedObserver;
  Observer<Camera>::Ptr _onCameraLoadedObserver;
  Observer<void>::Ptr _onCompleteObserver;
  Observer<std::string>::Ptr _onErrorObserver;
  Observer<void>::Ptr _onDisposeObserver;
  Observer<IGLTFLoaderExtension>::Ptr _onExtensionLoadedObserver;
  Observer<IGLTFValidationResults>::Ptr _onValidatedObserver;
  IGLTFLoaderPtr _loader;
  size_t _logIndentLevel;
  bool _loggingEnabled;
  bool _capturePerformanceCounters;

}; // end of class GLTFFileLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H
