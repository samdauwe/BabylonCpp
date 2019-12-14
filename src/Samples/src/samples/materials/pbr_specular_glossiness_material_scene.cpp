#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR Specular Glossiness Material Scene. Example demonstrating how to create physically
 * based rendering specular glossiness materials.
 * @see https://www.babylonjs-playground.com/#Z1VL3V#5
 * @see https://doc.babylonjs.com/how_to/physically_based_rendering
 */
struct PbrSpecularGlossinessMaterialScene : public IRenderableScene {

  PbrSpecularGlossinessMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PbrSpecularGlossinessMaterialScene() override = default;

  const char* getName() override
  {
    return "PBR Specular Glossiness Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f, Vector3::Zero(), scene);

    camera->lowerRadiusLimit = 2.f;
    camera->upperRadiusLimit = 10.f;

    camera->attachControl(canvas, true);

    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    auto pbr           = PBRSpecularGlossinessMaterial::New("pbr", scene);
    sphere->material   = pbr;
    pbr->diffuseColor  = Color3(1.f, 0.766f, 0.336f);
    pbr->specularColor = Color3(1.f, 0.766f, 0.336f);
    pbr->glossiness    = 0.4f;
    pbr->environmentTexture
      = CubeTexture::CreateFromPrefilteredData("/textures/environment.dds", scene);
  }

}; // end of struct PBRSpecularGlossinessMaterialScene

BABYLON_REGISTER_SAMPLE("Materials", PbrSpecularGlossinessMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
