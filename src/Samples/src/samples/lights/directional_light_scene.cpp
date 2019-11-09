#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the directional light. The light is emitted from everywhere
 * in the specified direction, and has an infinite range.
 * @see https://www.babylonjs-playground.com/#20OAV9#1
 * @see https://doc.babylonjs.com/babylon101/lights#the-directional-light
 */
struct DirectionalLightScene : public IRenderableScene {

  DirectionalLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~DirectionalLightScene() override = default;

  const char* getName() override
  {
    return "Directional Light Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is directly down
    auto light      = DirectionalLight::New("DirectionalLight", Vector3(0.f, -1.f, 0.f), scene);
    light->diffuse  = Color3(1.f, 0.f, 0.f);
    light->specular = Color3(0.f, 1.f, 0.f);

    SphereOptions options;
    options.diameter = 1.5f;
    MeshBuilder::CreateSphere("sphere", options, scene);
  }

}; // end of struct DirectionalLightScene

BABYLON_REGISTER_SAMPLE("Ligths", DirectionalLightScene)

} // end of namespace Samples
} // end of namespace BABYLON
