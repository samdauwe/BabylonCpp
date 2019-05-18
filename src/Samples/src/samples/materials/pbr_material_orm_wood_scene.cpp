#include <babylon/samples/materials/pbr_material_orm_wood_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

PBRMaterialORMWoodScene::PBRMaterialORMWoodScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRMaterialORMWoodScene::~PBRMaterialORMWoodScene()
{
}

const char* PBRMaterialORMWoodScene::getName()
{
  return "PBR Material ORM Wood Scene";
}

void PBRMaterialORMWoodScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("camera", 1.f, 1.f, 6.f, Vector3::Zero(), scene);
  camera->wheelPrecision = 100.f;
  camera->attachControl(canvas, true);

  auto hdrTexture = CubeTexture::CreateFromPrefilteredData(
    "textures/Studio_Softbox_2Umbrellas_cube_specular.dds", scene);
  hdrTexture->gammaSpace    = false;
  scene->environmentTexture = hdrTexture;

  auto sphere          = Mesh::CreateSphere("sphere1", 16, 1.f, scene);
  sphere->position().y = 0.5f;
  auto sphereMat       = PBRMaterial::New("sphereMat", scene);
  sphere->material     = sphereMat;

  auto cube          = Mesh::CreateBox("cube", 1.f, scene);
  cube->position().y = 0.5f;
  cube->position().x = -1.f;
  cube->position().z = -1.f;

  auto ground    = Mesh::CreateGround("ground1", 6, 6, 10, scene);
  auto groundMat = PBRMaterial::New("groundMat", scene);

  ground->material = groundMat;
  cube->material   = groundMat;

  groundMat->albedoTexture
    = Texture::New("textures/stripParquetHardwoodFloorAlbedo.jpg", scene);
  groundMat->bumpTexture
    = Texture::New("textures/stripParquetHardwoodFloorNormal.png", scene);
  groundMat->invertNormalMapX = true;
  groundMat->invertNormalMapY = true;
  groundMat->metallicTexture
    = Texture::New("textures/stripParquetHardwoodFloorMetallic.jpg", scene);
  groundMat->useAmbientOcclusionFromMetallicTextureRed = true;
  groundMat->useMetallnessFromMetallicTextureBlue      = true;
  groundMat->useRoughnessFromMetallicTextureGreen      = true;
  groundMat->useRoughnessFromMetallicTextureAlpha      = false;

  sphereMat->metallic  = 1.f;
  sphereMat->roughness = 0.2f;
}

} // end of namespace Samples
} // end of namespace BABYLON
