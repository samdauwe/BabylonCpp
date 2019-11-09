#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/spot_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Intersecting Lights Colors Scene. Scene demonstrating intersecting lights colors using
 * intersecting spot lights.
 * @see http://www.babylonjs-playground.com/#20OAV9#9
 */
struct IntersectingLightsColorsScene : public IRenderableScene {

  IntersectingLightsColorsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~IntersectingLightsColorsScene() override = default;

  const char* getName() override
  {
    return "Intersecting Lights Colors Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Red light
    auto light     = SpotLight::New("spotLight",
                                Vector3(-std::cos(Math::PI / 6.f), 1.f, -std::sin(Math::PI / 6.f)),
                                Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
    light->diffuse = Color3::Red();

    // Green light
    auto light1 = SpotLight::New("spotLight1", Vector3(0.f, 1.f, 1.f - std::sin(Math::PI / 6.f)),
                                 Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
    light1->diffuse = Color3::Green();

    // Blue light
    auto light2     = SpotLight::New("spotLight2",
                                 Vector3(std::cos(Math::PI / 6), 1.f, -std::sin(Math::PI / 6.f)),
                                 Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
    light2->diffuse = Color3::Blue();

    GroundOptions groundOptions;
    groundOptions.width  = 4;
    groundOptions.height = 4;
    auto ground          = MeshBuilder::CreateGround("ground", groundOptions, scene);
  }

}; // end of struct IntersectingLightsColorsScene

BABYLON_REGISTER_SAMPLE("Ligths", IntersectingLightsColorsScene)

} // end of namespace Samples
} // end of namespace BABYLON
