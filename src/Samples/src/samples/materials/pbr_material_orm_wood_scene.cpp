#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBRM material ORM Wood Scene. Example demonstrating how to manually create an ORM texture
 * linked to the metallicTexture channel:
 *   - Red channel for Ambient Occlusion
 *   - Green channel for Roughness
 *   - Blue channel for Metallic
 * @see https://www.babylonjs-playground.com/#K4S3GU#58
 * @see https://forum.babylonjs.com/t/how-to-create-a-pbr-from-multiple-images/1433
 */
struct PbrMaterialOrmWoodScene : public IRenderableScene {

  PbrMaterialOrmWoodScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrMaterialOrmWoodScene() override = default;

  const char* getName() override
  {
    return "PBR Material ORM Wood Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera            = ArcRotateCamera::New("camera", 1.f, 1.f, 6.f, Vector3::Zero(), scene);
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

    groundMat->albedoTexture = Texture::New("textures/stripParquetHardwoodFloorAlbedo.jpg", scene);
    groundMat->bumpTexture   = Texture::New("textures/stripParquetHardwoodFloorNormal.png", scene);
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

}; // end of struct PBRMaterialORMWoodScene

BABYLON_REGISTER_SAMPLE("Materials", PbrMaterialOrmWoodScene)

} // end of namespace Samples
} // end of namespace BABYLON
