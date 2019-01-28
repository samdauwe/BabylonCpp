#include <babylon/samples/lights/intersecting_lights_colors_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/spot_light.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

IntersectingLightsColorsScene::IntersectingLightsColorsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

IntersectingLightsColorsScene::~IntersectingLightsColorsScene()
{
}

const char* IntersectingLightsColorsScene::getName()
{
  return "Intersecting Lights Colors Scene";
}

void IntersectingLightsColorsScene::initializeScene(ICanvas* canvas,
                                                    Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Red light
  auto light = SpotLight::New(
    "spotLight",
    Vector3(-std::cos(Math::PI / 6.f), 1.f, -std::sin(Math::PI / 6.f)),
    Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
  light->diffuse = Color3::Red();

  // Green light
  auto light1 = SpotLight::New(
    "spotLight1", Vector3(0.f, 1.f, 1.f - std::sin(Math::PI / 6.f)),
    Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
  light1->diffuse = Color3::Green();

  // Blue light
  auto light2 = SpotLight::New(
    "spotLight2",
    Vector3(std::cos(Math::PI / 6), 1.f, -std::sin(Math::PI / 6.f)),
    Vector3(0.f, -1.f, 0.f), Math::PI_2, 1.5f, scene);
  light2->diffuse = Color3::Blue();

  GroundOptions groundOptions;
  groundOptions.width  = 4;
  groundOptions.height = 4;
  auto ground = MeshBuilder::CreateGround("ground", groundOptions, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
