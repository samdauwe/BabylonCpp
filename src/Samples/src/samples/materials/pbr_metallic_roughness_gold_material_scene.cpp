#include <babylon/samples/materials/pbr_metallic_roughness_gold_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

PBRMetallicRoughnessGoldMaterialScene::PBRMetallicRoughnessGoldMaterialScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRMetallicRoughnessGoldMaterialScene::~PBRMetallicRoughnessGoldMaterialScene()
{
}

const char* PBRMetallicRoughnessGoldMaterialScene::getName()
{
  return "PBR Metallic Roughness Gold Material Scene";
}

void PBRMetallicRoughnessGoldMaterialScene::initializeScene(ICanvas* canvas,
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

  pbr->baseColor          = Color3(1.f, 0.766f, 0.336f);
  pbr->metallic           = 1.f;
  pbr->roughness          = 0.4f;
  pbr->environmentTexture = CubeTexture::CreateFromPrefilteredData(
    "/textures/environment.dds", scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
