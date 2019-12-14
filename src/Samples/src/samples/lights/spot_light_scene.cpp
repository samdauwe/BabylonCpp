#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/spot_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the spot light. This light defines a cone of light pointing
 * in a direction.
 * @see https://www.babylonjs-playground.com/#20OAV9#3
 * @see https://doc.babylonjs.com/babylon101/lights#the-spot-light
 */
struct SpotLightScene : public IRenderableScene {

  SpotLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~SpotLightScene() override = default;

  const char* getName() override
  {
    return "Spot Light Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is directly down from a position one unit up, slow decay
    auto light      = SpotLight::New("spotLight", Vector3(-1.f, 1.f, -1.f), Vector3(0.f, -1.f, 0.f),
                                Math::PI_2, 10.f, scene);
    light->diffuse  = Color3(1.f, 0.f, 0.f);
    light->specular = Color3(0.f, 1.f, 0.f);

    // Light direction is directly down from a position one unit up, fast decay
    auto light1      = SpotLight::New("spotLight1", Vector3(1.f, 1.f, 1.f), Vector3(0.f, -1.f, 0.f),
                                 Math::PI_2, 50.f, scene);
    light1->diffuse  = Color3(0.f, 1.f, 0.f);
    light1->specular = Color3(0.f, 1.f, 0.f);

    GroundOptions options;
    options.width  = 4;
    options.height = 4;
    MeshBuilder::CreateGround("ground", options, scene);
  }

}; // end of struct SpotLightScene

BABYLON_REGISTER_SAMPLE("Ligths", SpotLightScene)

} // end of namespace Samples
} // end of namespace BABYLON
