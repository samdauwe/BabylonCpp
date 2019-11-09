#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Metallic Roughness Gold Material Scene. Example demonstrating how to create physically
 * based rendering metallic roughness materials and add more roughness to give it a more golden
 * look.
 * @see https://www.babylonjs-playground.com/#2FDQT5#12
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PbrMetallicRoughnessGoldMaterialScene : public IRenderableScene {

  PbrMetallicRoughnessGoldMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrMetallicRoughnessGoldMaterialScene() override = default;

  const char* getName() override
  {
    return "PBR Metallic Roughness Gold Material Scene";
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
    pbr->metallic  = 1.f;
    pbr->roughness = 0.4f;
    pbr->environmentTexture
      = CubeTexture::CreateFromPrefilteredData("/textures/environment.dds", scene);
  }

}; // end of struct PBRMetallicRoughnessGoldMaterialScene

BABYLON_REGISTER_SAMPLE("Materials", PbrMetallicRoughnessGoldMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
