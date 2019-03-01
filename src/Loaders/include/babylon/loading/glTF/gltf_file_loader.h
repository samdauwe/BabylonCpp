#ifndef BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H
#define BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/loading/iscene_loader_plugin_async.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class AbstractMesh;
class BaseTexture;
class Camera;
class Material;
using AbstractMeshPtr = std::function<AbstractMesh>;
using BaseTexturePtr  = std::function<BaseTexture>;
using CameraPtr       = std::function<Camera>;
using MaterialPtr     = std::function<Material>;

namespace GLTF2 {

struct IGLTFLoaderData;
struct IGLTFLoaderExtension;

class BABYLON_SHARED_EXPORT GLTFFileLoader : public IDisposable,
                                             public ISceneLoaderPluginAsync {

protected:
  /**
   * @brief Raised when the asset has been parsed.
   */
  void set_onParsed(
    const std::function<void(const IGLTFLoaderData& loaderData)>& callback);

  /**
   * @brief Callback raised when the loader creates a mesh after parsing the
   * glTF properties of the mesh.
   */
  void set_onMeshLoaded(
    const std::function<void(const AbstractMeshPtr& callback)>& callback);

  /**
   * @brief Callback raised when the loader creates a texture after parsing the
   * glTF properties of the texture.
   */
  void set_onTextureLoaded(
    const std::function<void(const BaseTexturePtr& texture)>& callback);

  /**
   * @brief Callback raised when the loader creates a material after parsing the
   * glTF properties of the material.
   */
  void set_onMaterialLoaded(
    const std::function<void(const MaterialPtr& material)>& callback);

  /**
   * @brief Callback raised when the loader creates a camera after parsing the
   * glTF properties of the camera.
   */
  void set_onCameraLoaded(
    const std::function<void(const CameraPtr& camera)>& callback);

  /**
   * @brief Callback raised when the asset is completely loaded, immediately
   * before the loader is disposed. For assets with LODs, raised when all of the
   * LODs are complete. For assets without LODs, raised when the model is
   * complete, immediately after the loader resolves the returned promise.
   */
  void set_onComplete(const std::function<void()>& callback);

  /**
   * @brief Callback raised when an error occurs.
   */
  void
  set_onError(const std::function<void(const std::string& reason)>& callback);

  /**
   * @brief Callback raised after the loader is disposed.
   */
  void set_onDispose(const std::function<void()>& callback);

  /**
   * @brief Callback raised after a loader extension is created.
   */
  void set_onExtensionLoaded(
    const std::function<void(const IGLTFLoaderExtension& extension)>& callback);

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
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const IGLTFLoaderData& loaderData)>>
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
  unsigned int coordinateSystemMode;

  /**
   * The animation start mode. Defaults to FIRST.
   */
  unsigned int animationStartMode;

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
  std::function<void(const std::string& url)> preprocessUrlAsync;

  /**
   * Observable raised when the loader creates a mesh after parsing the glTF
   * properties of the mesh.
   */
  Observable<AbstractMeshPtr> onMeshLoadedObservable;

  /**
   * Callback raised when the loader creates a mesh after parsing the glTF
   * properties of the mesh.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const AbstractMeshPtr& mesh)>>
    onMeshLoaded;

  /**
   * Observable raised when the loader creates a texture after parsing the glTF
   * properties of the texture.
   */
  Observable<BaseTexturePtr> onTextureLoadedObservable;

  /**
   * Callback raised when the loader creates a texture after parsing the
   * glTF properties of the texture.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const BaseTexturePtr& texture)>>
    onTextureLoaded;

  /**
   * Observable raised when the loader creates a material after parsing the glTF
   * properties of the material.
   */
  Observable<MaterialPtr> onMaterialLoadedObservable;

  /**
   * Callback raised when the loader creates a material after parsing the
   * glTF properties of the material.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const MaterialPtr& material)>>
    onMaterialLoaded;

  /**
   * Observable raised when the loader creates a camera after parsing the glTF
   * properties of the camera.
   */
  Observable<CameraPtr> onCameraLoadedObservable;

  /**
   * Callback raised when the loader creates a camera after parsing the glTF
   * properties of the camera.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const CameraPtr& camera)>>
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
  WriteOnlyProperty<GLTFFileLoader, std::function<void()>> onComplete;

  /**
   * Observable raised when an error occurs.
   */
  Observable<std::string> onErrorObservable;

  /**
   * Callback raised when an error occurs.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const std::string& reason)>>
    onError;

  /**
   * Observable raised after the loader is disposed.
   */
  Observable<void> onDisposeObservable;

  /**
   * Callback raised after the loader is disposed.
   */
  WriteOnlyProperty<GLTFFileLoader, std::function<void()>> onDispose;

  /**
   * Observable raised after a loader extension is created.
   * Set additional options for a loader extension in this event.
   */
  Observable<IGLTFLoaderExtension> onExtensionLoadedObservable;

  /**
   * Callback raised after a loader extension is created.
   */
  WriteOnlyProperty<GLTFFileLoader,
                    std::function<void(const IGLTFLoaderExtension& extension)>>
    onExtensionLoaded;

private:
  Observer<IGLTFLoaderData>::Ptr _onParsedObserver;
  Observer<AbstractMeshPtr>::Ptr _onMeshLoadedObserver;
  Observer<BaseTexture>::Ptr _onTextureLoadedObserver;
  Observer<Material>::Ptr _onMaterialLoadedObserver;
  Observer<Camera>::Ptr _onCameraLoadedObserver;
  Observer<void>::Ptr _onCompleteObserver;
  Observer<std::string>::Ptr _onErrorObserver;
  Observer<void>::Ptr _onDisposeObserver;
  Observer<IGLTFLoaderExtension>::Ptr _onExtensionLoadedObserver;

}; // end of class GLTFFileLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_GLTF_FILE_LOADER_H
