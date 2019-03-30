#include <babylon/samples/materials/pbr_specular_glossiness_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

PBRSpecularGlossinessMaterialScene::PBRSpecularGlossinessMaterialScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRSpecularGlossinessMaterialScene::~PBRSpecularGlossinessMaterialScene()
{
}

const char* PBRSpecularGlossinessMaterialScene::getName()
{
  return "PBR Specular Glossiness Material Scene";
}

void PBRSpecularGlossinessMaterialScene::initializeScene(ICanvas* canvas,
                                                         Scene* scene)
{
  auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);

  camera->lowerRadiusLimit = 2.f;
  camera->upperRadiusLimit = 10.f;

  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.97f;

  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  auto pbr                = PBRSpecularGlossinessMaterial::New("pbr", scene);
  sphere->material        = pbr;
  pbr->diffuseColor       = Color3(1.f, 0.766f, 0.336f);
  pbr->specularColor      = Color3(1.f, 0.766f, 0.336f);
  pbr->glossiness         = 0.4f;
  pbr->environmentTexture = CubeTexture::CreateFromPrefilteredData(
    "/textures/environment.dds", scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
