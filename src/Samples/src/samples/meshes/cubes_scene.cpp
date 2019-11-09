#include <babylon/samples/samples_index.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <tuple>
#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cubes Scene. Example demonstrating how to create animated cubes.
 * @see https://playground.babylonjs.com/#F7BSJ6#4
 */
class CubesScene : public IRenderableScene {

  using Index = std::tuple<unsigned, unsigned, unsigned>;

public:
  CubesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _t{0.f}
  {
  }
  ~CubesScene()
  {
  }

  const char* getName() override
  {
    return "Cubes Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    auto camera = FreeCamera::New("camera1", Vector3(10.f, 10.f, 10.f), scene);
  
    // This targets the camera to scene origin
    camera->setTarget(Vector3::Zero());
  
    // This attaches the camera to the canvas
    camera->attachControl(canvas, true);
  
    // This creates a light
    auto light = PointLight::New("light1", Vector3(30.f, 50.f, 30.f), scene);
  
    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 1.3f;
  
    // Change clear color
    scene->clearColor = Color3::FromHexString("#7f8c8d");
  
    // Create boxes
    for (unsigned int i = 0; i < 2; ++i) {
      for (unsigned int j = 0; j < 2; ++j) {
        for (unsigned int k = 0; k < 2; ++k) {
          auto box          = Mesh::CreateBox("box", 2.f, scene);
          box->position().x = i == 0 ? -1.f : 1.f;
          box->position().y = j == 0 ? -1.f : 1.f;
          box->position().z = k == 0 ? -1.f : 1.f;
  
          auto matrial = StandardMaterial::New("", scene);
          if (i * 4 + j * 2 + k < 1) {
            matrial->diffuseColor = Color3::FromHexString("#2ecc71");
          }
          else if (i * 4 + j * 2 + k < 2) {
            matrial->diffuseColor = Color3::FromHexString("#3498db");
          }
          else if (i * 4 + j * 2 + k < 3) {
            matrial->diffuseColor = Color3::FromHexString("#9b59b6");
          }
          else if (i * 4 + j * 2 + k < 4) {
            matrial->diffuseColor = Color3::FromHexString("#34495e");
          }
          else if (i * 4 + j * 2 + k < 5) {
            matrial->diffuseColor = Color3::FromHexString("#f1c40f");
          }
          else if (i * 4 + j * 2 + k < 6) {
            matrial->diffuseColor = Color3::FromHexString("#e67e22");
          }
          else if (i * 4 + j * 2 + k < 7) {
            matrial->diffuseColor = Color3::FromHexString("#e74c3c");
          }
          else if (i * 4 + j * 2 + k < 8) {
            matrial->diffuseColor = Color3::FromHexString("#ecf0f1");
          }
          box->material = matrial;
          _boxes.emplace_back(box);
          _indices.emplace_back(std::make_tuple(i, j, k));
        }
      }
    }
  
    // Box animations
    scene->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState & /*es*/) -> void {
        _t += _scene->getEngine()->getDeltaTime();
        auto val = (1.f + std::sin(_t / 500.f));
        for (unsigned int boxIndex = 0; boxIndex < _boxes.size(); ++boxIndex) {
          auto& box        = _boxes[boxIndex];
          auto& boxIndices = _indices[boxIndex];
          auto i = std::get<0>(boxIndices), j = std::get<1>(boxIndices),
               k = std::get<2>(boxIndices);
          box->position().set((i == 1 ? val : -val) + (i == 0 ? -1.f : 1.f),
                              (j == 1 ? val : -val) + (j == 0 ? -1.f : 1.f),
                              (k == 1 ? val : -val) + (k == 0 ? -1.f : 1.f));
          box->scaling().set(1.f + val / 2.f, 1.f + val / 2.f, 1.f + val / 2.f);
          box->rotation().set(0.f, (j == 1 ? val : -val) / 1.f, 0.f);
        }
      });
  }

private:
  float _t;
  std::vector<MeshPtr> _boxes;
  std::vector<Index> _indices;

}; // end of class CubesScene


BABYLON_REGISTER_SAMPLE("Meshes", CubesScene)
} // end of namespace Samples
} // end of namespace BABYLON