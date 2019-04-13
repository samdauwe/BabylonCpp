#include <babylon/samples/materials/pbr_metallic_roughness_texture_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

PBRMetallicRoughnessTextureMaterialScene::
  PBRMetallicRoughnessTextureMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRMetallicRoughnessTextureMaterialScene::
  ~PBRMetallicRoughnessTextureMaterialScene()
{
}

const char* PBRMetallicRoughnessTextureMaterialScene::getName()
{
  return "PBR Metallic Roughness Texture Material Scene";
}

void PBRMetallicRoughnessTextureMaterialScene::initializeScene(ICanvas* canvas,
                                                               Scene* scene)
{
  auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);

  camera->lowerRadiusLimit = 2.f;
  camera->upperRadiusLimit = 10.f;

  camera->attachControl(canvas, true);

  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  auto pbr         = PBRMetallicRoughnessMaterial::New("pbr", scene);
  sphere->material = pbr;

  pbr->baseColor = Color3(1.f, 0.766f, 0.336f);
  // set to 1 to only use it from the metallicRoughnessTexture
  pbr->metallic = 1.f;
  // set to 1 to only use it from the metallicRoughnessTexture
  pbr->roughness          = 1.f;
  pbr->environmentTexture = CubeTexture::CreateFromPrefilteredData(
    "/textures/environment.dds", scene);
  pbr->metallicRoughnessTexture = Texture::New("/textures/mr.jpg", scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
