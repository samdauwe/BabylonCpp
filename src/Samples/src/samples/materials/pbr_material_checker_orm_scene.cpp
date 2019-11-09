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
 * @brief PBRM material checker ORM Scene. Example demonstrating how to manually create an ORM
 * texture linked to the metallicTexture channel:
 *   - Red channel for Ambient Occlusion
 *   - Green channel for Roughness
 *   - Blue channel for Metallic
 * @see https://www.babylonjs-playground.com/#K4S3GU#64
 * @see https://forum.babylonjs.com/t/how-to-create-a-pbr-from-multiple-images/1433
 */
struct PbrMaterialCheckerOrmScene : public IRenderableScene {

  PbrMaterialCheckerOrmScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrMaterialCheckerOrmScene() override = default;

  const char* getName() override
  {
    return "PBR Material Checker ORM Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera            = ArcRotateCamera::New("camera", -1.f, 1.f, 6.f, Vector3::Zero(), scene);
    camera->wheelPrecision = 100.f;
    camera->attachControl(canvas, true);

    auto hdrTexture = CubeTexture::CreateFromPrefilteredData("textures/environment.dds", scene);
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

    auto checkerORM            = Texture::New("textures/checkerMetallic.png", scene);
    sphereMat->metallicTexture = checkerORM;
    auto uvScale               = 2.f;
    checkerORM->uScale         = uvScale;
    checkerORM->vScale         = uvScale;
    sphereMat->useMetallnessFromMetallicTextureBlue      = true;
    sphereMat->useRoughnessFromMetallicTextureGreen      = true;
    sphereMat->useRoughnessFromMetallicTextureAlpha      = false;
    sphereMat->useAmbientOcclusionFromMetallicTextureRed = true;

    /* end of materials tweaking */
  }

}; // end of struct PBRMaterialCheckerORMScene

BABYLON_REGISTER_SAMPLE("Materials", PbrMaterialCheckerOrmScene)

} // end of namespace Samples
} // end of namespace BABYLON
