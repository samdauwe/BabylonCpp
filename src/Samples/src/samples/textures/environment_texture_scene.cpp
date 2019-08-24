#include <babylon/samples/textures/environment_texture_scene.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/textures/cube_texture.h>

namespace BABYLON {
namespace Samples {

EnvironmentTextureScene::EnvironmentTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

EnvironmentTextureScene::~EnvironmentTextureScene()
{
}

const char* EnvironmentTextureScene::getName()
{
  return "Environment Texture Scene";
}

void EnvironmentTextureScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  // Create default environment while loading the model
  scene->createDefaultCameraOrLight(true, true, true);

  // Default camera setup.
  scene->imageProcessingConfiguration()->exposure           = 1.4f;
  scene->imageProcessingConfiguration()->contrast           = 1.66f;
  scene->imageProcessingConfiguration()->toneMappingEnabled = true;

  // Define a general environment textue
  auto hdrTexture
    = CubeTexture::CreateFromPrefilteredData("textures/environment.env", scene);
  hdrTexture->gammaSpace = false;
  scene->createDefaultSkybox(
    hdrTexture, true,
    (scene->activeCamera()->maxZ - scene->activeCamera()->minZ) / 2.f, 0.f);
}

} // end of namespace Samples
} // end of namespace BABYLON
