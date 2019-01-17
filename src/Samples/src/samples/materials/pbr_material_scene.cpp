#include <babylon/samples/materials/pbr_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

PBRMaterialScene::PBRMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

PBRMaterialScene::~PBRMaterialScene()
{
}

const char* PBRMaterialScene::getName()
{
  return "PBR Material Scene";
}

void PBRMaterialScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f,
                                     200, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);
  camera->minZ = 0.1f;

  // Environment Texture
  auto hdrTexture
    = CubeTexture::CreateFromPrefilteredData("textures/environments/environmentSpecular.env", scene);

  scene->imageProcessingConfiguration()->exposure = 0.6f;
  scene->imageProcessingConfiguration()->contrast = 1.6f;

  // Skybox
  auto hdrSkybox         = Mesh::CreateBox("hdrSkyBox", 1000.f, scene);
  auto hdrSkyboxMaterial = PBRMaterial::New("skyBox", scene);
  hdrSkyboxMaterial->backFaceCulling   = false;
  hdrSkyboxMaterial->reflectionTexture = hdrTexture->clone();
  hdrSkyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  hdrSkyboxMaterial->microSurface    = 1.f;
  hdrSkyboxMaterial->disableLighting = true;
  hdrSkybox->material                = hdrSkyboxMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
