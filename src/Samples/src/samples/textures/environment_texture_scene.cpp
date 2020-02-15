#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Environment texture scene. Example demonstrating how to create a skybox using and
 * environment texture.
 * @see https://www.babylonjs-playground.com/#09R6ZA#3
 */
struct EnvironmentTextureScene : public IRenderableScene {

  EnvironmentTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~EnvironmentTextureScene() override = default;

  const char* getName() override
  {
    return "Environment Texture Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    // Create default environment while loading the model
    scene->createDefaultCameraOrLight(true, true, true);

    // Default camera setup.
    scene->imageProcessingConfiguration()->exposure           = 1.4f;
    scene->imageProcessingConfiguration()->contrast           = 1.66f;
    scene->imageProcessingConfiguration()->toneMappingEnabled = true;

    // Define a general environment textue
    auto hdrTexture
      = CubeTexture::CreateFromPrefilteredData("textures/environmentSpecular.env", scene);
    hdrTexture->gammaSpace = false;
    scene->createDefaultSkybox(
      hdrTexture, true, (scene->activeCamera()->maxZ - scene->activeCamera()->minZ) / 2.f, 0.f);
  }

}; // end of struct EnvironmentTextureScene

BABYLON_REGISTER_SAMPLE("Textures", EnvironmentTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
