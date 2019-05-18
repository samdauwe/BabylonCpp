#include <babylon/samples/materials/pbr_material_checker_orm_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

PBRMaterialCheckerORMScene::PBRMaterialCheckerORMScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRMaterialCheckerORMScene::~PBRMaterialCheckerORMScene()
{
}

const char* PBRMaterialCheckerORMScene::getName()
{
  return "PBR Material Checker ORM Scene";
}

void PBRMaterialCheckerORMScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("camera", -1.f, 1.f, 6.f, Vector3::Zero(), scene);
  camera->wheelPrecision = 100.f;
  camera->attachControl(canvas, true);

  auto hdrTexture
    = CubeTexture::CreateFromPrefilteredData("textures/environment.dds", scene);
  hdrTexture->gammaSpace    = false;
  scene->environmentTexture = hdrTexture;

  auto sphere          = Mesh::CreateSphere("sphere1", 16, 1.f, scene);
  sphere->position().y = 0.5f;
  auto sphereMat       = PBRMaterial::New("sphereMat", scene);
  sphere->material     = sphereMat;

  auto cube          = Mesh::CreateBox("cube", 1.f, scene);
  cube->position().y = 0.5f;
  cube->position().x = -2.f;
  cube->position().z = -2.f;
  cube->material     = sphereMat;

  auto ground      = Mesh::CreateGround("ground1", 6, 6, 10, scene);
  ground->material = sphereMat;

  /* materials tweaking */

  auto checkerORM = Texture::New("textures/checkerMetallic.png", scene);
  sphereMat->metallicTexture                           = checkerORM;
  auto uvScale                                         = 2.f;
  checkerORM->uScale                                   = uvScale;
  checkerORM->vScale                                   = uvScale;
  sphereMat->useMetallnessFromMetallicTextureBlue      = true;
  sphereMat->useRoughnessFromMetallicTextureGreen      = true;
  sphereMat->useRoughnessFromMetallicTextureAlpha      = false;
  sphereMat->useAmbientOcclusionFromMetallicTextureRed = true;

  /* end of materials tweaking */
}

} // end of namespace Samples
} // end of namespace BABYLON
