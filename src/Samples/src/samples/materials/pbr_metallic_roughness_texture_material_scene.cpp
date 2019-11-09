#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Metallic Roughness Texture Material Scene. Example demonstrating how to create
 * physically based rendering metallic roughness materials.
 * @see https://www.babylonjs-playground.com/#2FDQT5#13
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PbrMetallicRoughnessTextureMaterialScene : public IRenderableScene {

  PbrMetallicRoughnessTextureMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrMetallicRoughnessTextureMaterialScene() override = default;

  const char* getName() override
  {
    return "PBR Metallic Roughness Texture Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f, Vector3::Zero(), scene);

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
    pbr->roughness = 1.f;
    pbr->environmentTexture
      = CubeTexture::CreateFromPrefilteredData("/textures/environment.dds", scene);
    pbr->metallicRoughnessTexture = Texture::New("/textures/mr.jpg", scene);
  }

}; // end of struct PBRMetallicRoughnessTextureMaterialScene

BABYLON_REGISTER_SAMPLE("Materials", PbrMetallicRoughnessTextureMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
