#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the point light. This light is emitting from a single
 * point.
 * @see https://www.babylonjs-playground.com/#20OAV9#0
 * @see https://doc.babylonjs.com/babylon101/lights#the-point-light
 */
struct PointLightScene : public IRenderableScene {

  PointLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PointLightScene() override = default;

  const char* getName() override
  {
    return "Point Light Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is directly down
    auto light      = PointLight::New("light", Vector3(0.f, 1.f, 0.f), scene);
    light->diffuse  = Color3(1.f, 0.f, 0.f);
    light->specular = Color3(0.f, 1.f, 0.f);

    SphereOptions options;
    options.diameter     = 1.5f;
    auto sphere          = MeshBuilder::CreateSphere("sphere", options, scene);
    sphere->position().z = 1.25f;
  }

}; // end of struct PointLightScene

BABYLON_REGISTER_SAMPLE("Ligths", PointLightScene)

} // end of namespace Samples
} // end of namespace BABYLON
