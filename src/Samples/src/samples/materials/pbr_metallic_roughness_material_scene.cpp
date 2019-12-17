#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Metallic Roughness Material Scene. Example demonstrating how to create physically
 * based rendering metallic roughness materials.
 * @see https://www.babylonjs-playground.com/#2FDQT5#0
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PbrMetallicRoughnessMaterialScene : public IRenderableScene {

  PbrMetallicRoughnessMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrMetallicRoughnessMaterialScene() override = default;

  const char* getName() override
  {
    return "PBR Metallic Roughness Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f, Vector3::Zero(), scene);

    camera->lowerRadiusLimit = 2.f;
    camera->upperRadiusLimit = 10.f;

    camera->attachControl(canvas, true);

    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.97f;

    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    auto pbr         = PBRMetallicRoughnessMaterial::New("pbr", scene);
    sphere->material = pbr;

    pbr->baseColor = Color3(1.f, 0.766f, 0.336f);
    pbr->metallic  = 0.f;
    pbr->roughness = 1.f;
  }

}; // end of struct PBRMetallicRoughnessMaterialScene

BABYLON_REGISTER_SAMPLE("Materials", PbrMetallicRoughnessMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
