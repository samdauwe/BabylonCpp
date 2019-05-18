#include <babylon/samples/materials/pbr_specular_glossiness_texture_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

PBRSpecularGlossinessTextureMaterialScene::
  PBRSpecularGlossinessTextureMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRSpecularGlossinessTextureMaterialScene::
  ~PBRSpecularGlossinessTextureMaterialScene()
{
}

const char* PBRSpecularGlossinessTextureMaterialScene::getName()
{
  return "PBR Specular Glossiness Texture Material Scene";
}

void PBRSpecularGlossinessTextureMaterialScene::initializeScene(ICanvas* canvas,
                                                                Scene* scene)
{
  auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);

  camera->lowerRadiusLimit = 2.f;
  camera->upperRadiusLimit = 10.f;

  camera->attachControl(canvas, true);

  auto light       = PointLight::New("light1", Vector3(0.f, 30.f, 0.f), scene);
  light->intensity = 0.97f;

  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  auto pbr           = PBRSpecularGlossinessMaterial::New("pbr", scene);
  sphere->material   = pbr;
  pbr->diffuseColor  = Color3(1.f, 0.766f, 0.336f);
  pbr->specularColor = Color3(1.f, 0.766f, 0.336f);
  // Let the texture controls the value
  pbr->glossiness         = 1.f;
  pbr->environmentTexture = CubeTexture::CreateFromPrefilteredData(
    "/textures/environment.dds", scene);
  pbr->specularGlossinessTexture = Texture::New("/textures/sg.png", scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
