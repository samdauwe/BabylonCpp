#ifndef BABYLON_HELPERS_ENVIRONMENT_HELPER_H
#define BABYLON_HELPERS_ENVIRONMENT_HELPER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/helpers/ienvironment_helper_options.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class AbstractMesh;
class BackgroundMaterial;
struct ISceneSize;
class Mesh;
class MirrorTexture;
class RenderTargetTexture;
class Scene;
using AbstractMeshPtr        = std::shared_ptr<AbstractMesh>;
using BackgroundMaterialPtr  = std::shared_ptr<BackgroundMaterial>;
using MeshPtr                = std::shared_ptr<Mesh>;
using MirrorTexturePtr       = std::shared_ptr<MirrorTexture>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief The Environment helper class can be used to add a fully featuread none
 * expensive background to your scene. It includes by default a skybox and a
 * ground relying on the BackgroundMaterial. It also helps with the default
 * setup of your imageProcessing configuration.
 */
class BABYLON_SHARED_EXPORT EnvironmentHelper {

public:
  /**
   * @brief constructor
   * @param options
   * @param scene The scene to add the material to
   */
  EnvironmentHelper(Scene* scene);

  /**
   * @brief constructor
   * @param options
   * @param scene The scene to add the material to
   */
  EnvironmentHelper(const IEnvironmentHelperOptions& options, Scene* scene);

  ~EnvironmentHelper();

  /**
   * Updates the background according to the new options.
   * @param color the main color to affect to the ground and the background
   */
  void updateOptions(const IEnvironmentHelperOptions& options);

  /**
   * Sets the primary color of all the available elements.
   * @param color
   */
  void setMainColor(const Color3& color);

  /**
   * @brief Gets the root mesh created by the helper.
   */
  MeshPtr& rootMesh();

  /**
   * @brief Gets the skybox created by the helper.
   */
  MeshPtr& skybox();

  /**
   * @brief Gets the skybox texture created by the helper.
   */
  BaseTexturePtr skyboxTexture();

  /**
   * @brief Gets the skybox material created by the helper.
   */
  BackgroundMaterialPtr& skyboxMaterial();

  /**
   * @brief Gets the ground mesh created by the helper.
   */
  MeshPtr& ground();

  /**
   * @brief Gets the ground texture created by the helper.
   */
  BaseTexture* groundTexture();

  /**
   * @brief Gets the ground mirror created by the helper.
   */
  MirrorTexturePtr& groundMirror();

  /**
   * @brief Gets the ground mirror render list to helps pushing the meshes
   * you wish in the ground reflection.
   */
  std::vector<AbstractMesh*>& groundMirrorRenderList();

  /**
   * @brief Gets the ground material created by the helper.
   */
  BackgroundMaterialPtr& groundMaterial();

  /**
   * @brief Dispose all the elements created by the Helper.
   */
  void dispose();

private:
  /**
   * @brief Creates the default options for the helper.
   */
  static IEnvironmentHelperOptions _getDefaultOptions();

  /**
   * @brief Setup the image processing according to the specified options.
   */
  void _setupImageProcessing();

  /**
   * @brief Setup the environment texture according to the specified options.
   */
  void _setupEnvironmentTexture();

  /**
   * @brief Setup the background according to the specified options.
   */
  void _setupBackground();

  /**
   * @brief Get the scene sizes according to the setup.
   */
  ISceneSize _getSceneSize();

  /**
   * @brief Setup the ground according to the specified options.
   */
  void _setupGround(const ISceneSize& sceneSize);

  /**
   * @brief Setup the ground material according to the specified options.
   */
  void _setupGroundMaterial();

  /**
   * @brief Setup the ground diffuse texture according to the specified options.
   */
  void _setupGroundDiffuseTexture();

  /**
   * @brief Setup the ground mirror texture according to the specified options.
   */
  void _setupGroundMirrorTexture(ISceneSize* sceneSize);

  /**
   * @brief Setup the ground to receive the mirror texture.
   */
  void _setupMirrorInGroundMaterial();

  /**
   * @brief Setup the skybox according to the specified options.
   */
  void _setupSkybox(const ISceneSize& sceneSize);

  /**
   * @brief Setup the skybox material according to the specified options.
   */
  void _setupSkyboxMaterial();

  /**
   * @brief Setup the skybox reflection texture according to the specified
   * options.
   */
  void _setupSkyboxReflectionTexture();

  void _errorHandler(const std::string& message      = "",
                     const std::exception& exception = std::exception());

public:
  /**
   * This observable will be notified with any error during the creation of
   * the environment, mainly texture creation errors.
   */
  Observable<Exception> onErrorObservable;

private:
  /**
   * Default ground texture URL.
   */
  static constexpr const char* _groundTextureCDNUrl
    = "https://assets.babylonjs.com/environments/backgroundGround.png";

  /**
   * Default skybox texture URL.
   */
  static constexpr const char* _skyboxTextureCDNUrl
    = "https://assets.babylonjs.com/environments/backgroundSkybox.dds";

  /**
   * Default environment texture URL.
   */
  static constexpr const char* _environmentTextureCDNUrl
    = "https://assets.babylonjs.com/environments/environmentSpecular.env";

private:
  MeshPtr _rootMesh;
  MeshPtr _skybox;
  RenderTargetTexturePtr _skyboxTexture;
  BackgroundMaterialPtr _skyboxMaterial;
  MeshPtr _ground;
  BaseTexture* _groundTexture;
  MirrorTexturePtr _groundMirror;
  BackgroundMaterialPtr _groundMaterial;
  std::vector<AbstractMesh*> _emptyGroundMirrorRenderList;

  /**
   * Stores the creation options.
   */
  Scene* _scene;
  IEnvironmentHelperOptions _options;

}; // end of class EnvironmentHelper

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_ENVIRONMENT_HELPER_H
