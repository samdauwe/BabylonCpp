#include <babylon/samples/lights/lights_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/string.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

LightsScene::LightsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _alpha{0.f}
{
}

LightsScene::~LightsScene()
{
}

const char* LightsScene::getName()
{
  return "Lights Scene";
}

void LightsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Setup camera
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  camera->setPosition(Vector3(-15.f, 10.f, 0.f));

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Light generator
  auto generateLight = [this](const Vector3& position, const Color3& color) {
    // Create light
    auto light = PointLight::New(String::concat("Omni", _lights.size()),
                                 position, _scene.get());
    // Light colors
    light->diffuse  = color;
    light->specular = color;
    // Store created object
    _lights.emplace_back(light);
  };

  // Light sphere generator
  auto generateLightSphere = [this](const Color3& color) {
    // Create light sphere
    auto lightSphere = Mesh::CreateSphere(
      String::concat("Sphere", _lightSpheres.size()), 16, 0.5f, _scene.get());
    lightSphere->setMaterial(StandardMaterial::New(
      String::concat("mat", _lightSpheres.size()), _scene.get()));
    lightSphere->material()->setDiffuseColor(Color3::Black());
    lightSphere->material()->setSpecularColor(Color3::Black());
    lightSphere->material()->setEmissiveColor(color);
    // Store created object
    _lightSpheres.emplace_back(lightSphere);
  };

  // Light positions
  std::array<Vector3, 6> lightPositions{{
    Vector3(0.f, 10.f, 0.f),  // Light 0
    Vector3(0.f, -10.f, 0.f), // Light 1
    Vector3(10.f, 0.f, 0.f),  // Light 2
    Vector3(1.f, -1.f, 0.f),  // Light 3
    Vector3(10.f, 0.f, 0.f),  // light 4
    Vector3(10.f, 0.f, 0.f),  // light 6
  }};

  // Light colors
  std::array<Color3, 6> lightColors{{
    Color3::Red(),    // Light 0: red
    Color3::Green(),  // Light 1: green
    Color3::Blue(),   // Light 2: blue
    Color3::White(),  // Light 3: white
    Color3::Yellow(), // light 4: yellow
    Color3::Cyan(),   // light 5: cyan
  }};

  // Create lights
  for (unsigned int index = 0; index < lightPositions.size(); ++index) {
    generateLight(lightPositions[index], lightColors[index]);
    generateLightSphere(lightColors[index]);
  }

  // Create material
  auto material          = StandardMaterial::New("mat", scene);
  material->diffuseColor = Color3(1.f, 1.f, 1.f);
  material->setMaxSimultaneousLights(
    static_cast<unsigned>(lightPositions.size()));

  // Generate sphere using ribbon
  auto generateSphere = [](float radius) {
    float tes  = 60.f;
    float pi2  = Math::PI2;
    float step = pi2 / tes;
    std::vector<std::vector<Vector3>> paths;
    for (float p = -Math::PI / 2.f; p < Math::PI / 2.f; p += step / 2.f) {
      std::vector<Vector3> path;
      for (float i = 0.f; i < pi2; i += step) {
        float x = radius * std::sin(i) * std::cos(p);
        float z = radius * std::cos(i) * std::cos(p);
        float y = radius * std::sin(p);
        path.emplace_back(Vector3(x, y, z));
      }
      paths.emplace_back(path);
    }
    std::vector<Vector3> lastPath;
    for (float j = 0.f; j < pi2; j += step) {
      lastPath.emplace_back(Vector3(0.f, radius, 0.f));
    }
    paths.emplace_back(lastPath);
    return paths;
  };

  auto sphere
    = Mesh::CreateRibbon("Sphere", generateSphere(3.f), false, true, 0, scene);
  sphere->setMaterial(material);

  // Animations
  scene->onBeforeRenderObservable.add([this](Scene*, EventState&) {
    _lights[0]->setPosition(
      Vector3(10.f * std::sin(_alpha), 0.f, 10.f * std::cos(_alpha)));
    _lights[1]->setPosition(
      Vector3(10.f * std::sin(_alpha), 0.f, -10.f * std::cos(_alpha)));
    _lights[2]->setPosition(
      Vector3(10.f * std::cos(_alpha), 0.f, 10.f * std::sin(_alpha)));
    _lights[3]->setPosition(
      Vector3(10.f * std::cos(_alpha), 0.f, -10.f * std::sin(_alpha)));
    _lights[4]->setPosition(
      Vector3(10.f * std::cos(_alpha), 10.f * std::sin(_alpha), 0.f));
    _lights[5]->setPosition(
      Vector3(10.f * std::cos(_alpha), -10.f * std::sin(_alpha), 0.f));
    for (unsigned int index = 0; index < _lights.size(); ++index) {
      _lightSpheres[index]->setPosition(_lights[index]->position());
    }
    _alpha += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
