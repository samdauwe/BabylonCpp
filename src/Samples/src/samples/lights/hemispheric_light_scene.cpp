#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the hemispheric light. This light simulate an ambient
 * environment light.
 * @see https://www.babylonjs-playground.com/#20OAV9#5
 * @see https://doc.babylonjs.com/babylon101/lights#the-hemispheric-light
 */
struct HemisphericLightScene : public IRenderableScene {

  HemisphericLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~HemisphericLightScene() override = default;

  const char* getName() override
  {
    return "Hemispheric Light Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is up and left
    auto light         = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
    light->diffuse     = Color3(1.f, 0.f, 0.f);
    light->specular    = Color3(0.f, 1.f, 0.f);
    light->groundColor = Color3(0.f, 1.f, 0.f);

    SphereOptions options;
    options.diameter = 1.5f;
    MeshBuilder::CreateSphere("sphere", options, scene);
  }

}; // end of struct HemisphericLightScene

BABYLON_REGISTER_SAMPLE("Ligths", HemisphericLightScene)

} // end of namespace Samples
} // end of namespace BABYLON
