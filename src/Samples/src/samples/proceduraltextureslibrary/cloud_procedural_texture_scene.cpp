#include <babylon/samples/proceduraltextureslibrary/cloud_procedural_texture_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture.h>

namespace BABYLON {
namespace Samples {

CloudProceduralTextureScene::CloudProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CloudProceduralTextureScene::~CloudProceduralTextureScene()
{
}

const char* CloudProceduralTextureScene::getName()
{
  return "Cloud Procedural Texture Scene";
}

void CloudProceduralTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates a basic Babylon Scene object (non-mesh)
  auto camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -10.f), scene);

  // This targets the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // This attaches the camera to the canvas
  camera->attachControl(canvas, true);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto boxCloud      = Mesh::CreateSphere("boxCloud", 100, 1000, scene);
  boxCloud->position = Vector3(0.f, 0.f, 240.f);
  auto cloudMaterial = StandardMaterial::New("cloudMat", scene);
  auto cloudProcText = ProceduralTexturesLibrary::CloudProceduralTexture::New(
    "cloud", 1024, scene);
  cloudMaterial->setEmissiveTexture(cloudProcText);
  cloudMaterial->backFaceCulling = false;
  cloudMaterial->emissiveTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  boxCloud->material = cloudMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
