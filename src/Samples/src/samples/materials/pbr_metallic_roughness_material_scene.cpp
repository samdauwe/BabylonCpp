#include <babylon/samples/materials/pbr_metallic_roughness_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

PBRMetallicRoughnessMaterialScene::PBRMetallicRoughnessMaterialScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRMetallicRoughnessMaterialScene::~PBRMetallicRoughnessMaterialScene()
{
}

const char* PBRMetallicRoughnessMaterialScene::getName()
{
  return "PBR Metallic Roughness Material Scene";
}

void PBRMetallicRoughnessMaterialScene::initializeScene(ICanvas* canvas,
                                                        Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("camera1", 0, Math::PI_2, 5, Vector3::Zero(), scene);

  camera->lowerRadiusLimit = 2.f;
  camera->upperRadiusLimit = 10.f;

  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.97f;

  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  auto pbr         = PBRMetallicRoughnessMaterial::New("pbr", scene);
  sphere->material = pbr;

  pbr->baseColor = Color3(1.f, 0.766f, 0.336f);
  pbr->metallic  = 0.f;
  pbr->roughness = 1.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
