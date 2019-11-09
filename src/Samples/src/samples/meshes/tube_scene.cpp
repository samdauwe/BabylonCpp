#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct TubeScene : public IRenderableScene {

  TubeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }
  ~TubeScene()
  {
  }

  const char* getName() override
  {
    return "Tube Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Change clear color of the scene
    scene->clearColor = Color4(0.5f, 0.5f, 0.5f);
  
    // camera
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f,
                                       Vector3(0.f, 0.f, -0.f), scene);
    camera->setPosition(Vector3(0, 0, -60));
    camera->attachControl(canvas, true);
  
    auto light = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
    light->intensity = 0.7f;
  
    auto spot       = SpotLight::New("spot", Vector3(25.f, 15.f, -10.f),
                               Vector3(-1.f, -0.8f, 1.f), 15.f, 1.f, scene);
    spot->diffuse   = Color3(1.f, 1.f, 1.f);
    spot->specular  = Color3(0.f, 0.f, 0.f);
    spot->intensity = 0.8f;
  
    auto mat             = StandardMaterial::New("mat1", scene);
    mat->alpha           = 1.f;
    mat->diffuseColor    = Color3(0.5f, 0.5f, 1.0f);
    mat->backFaceCulling = false;
    mat->wireframe       = false;
  
    auto curvePoints = [](float l, float t) {
      std::vector<Vector3> path;
      float step = l / t;
      for (float i = -l / 2.f; i < l / 2.f; i += step) {
        float x = std::sin(i / Math::PI * 2) + i;
        path.emplace_back(Vector3(x, 0.f, 0.f));
      }
      return path;
    };
  
    auto curve = curvePoints(40.f, 400.f);
  
    const auto radiusFunction = [](unsigned int i, float /*distance*/) {
      float t = static_cast<float>(i) / Math::PI * 2.f / 8.f;
      return std::sin(t) + static_cast<float>(i) / 25.f;
    };
  
    // Create tube
    auto tube = Mesh::CreateTube("tube", curve, 2, 60, radiusFunction, 0, scene,
                                 false, Mesh::FRONTSIDE);
    tube->material = mat;
  }

}; // end of struct TubeScene


BABYLON_REGISTER_SAMPLE("Meshes", TubeScene)
} // end of namespace Samples
} // end of namespace BABYLON