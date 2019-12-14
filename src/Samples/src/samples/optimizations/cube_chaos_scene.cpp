#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>
#include <vector>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cube Chaos Scene. Example demonstrating how to create animated scene with cubes using
 * instanced based rendering.
 * @see http://www.babylonjs-playground.com/#1E6NJO
 */
class CubeChaosScene : public IRenderableScene {

public:
  CubeChaosScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _time{0.f}
      , _camera{nullptr}
      , _mainMesh{nullptr}
      , _mainMeshMaterial{nullptr}
  {
  }

  ~CubeChaosScene() override = default;

  const char* getName() override
  {
    return "Cube Chaos Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    _camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -10.f), scene);

    // This targets the camera to scene origin
    _camera->setTarget(Vector3::Zero());

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    auto mainLight       = DirectionalLight::New("MainLight", Vector3(-1.f, -1.f, 1.f), scene);
    mainLight->intensity = 0.7f;

    auto cameraLight       = PointLight::New("cameraLight", Vector3(1.f, 10.f, 1.f), scene);
    cameraLight->intensity = 0.6f;
    cameraLight->position  = _camera->position;

    // Create cubes
    _mainMesh                                            = Mesh::CreateBox("Box", 1, scene);
    _mainMesh->scaling                                   = Vector3::Zero();
    _mainMeshMaterial                                    = StandardMaterial::New("meterial", scene);
    _mainMeshMaterial->diffuseColor                      = Color3(0.5f, 0.5f, 1.f);
    _mainMeshMaterial->specularColor                     = Color3(1.f, 1.f, 1.f);
    _mainMeshMaterial->useGlossinessFromSpecularMapAlpha = true;
    _mainMesh->material                                  = _mainMeshMaterial;

    auto index = 0.f;
    for (float x = -15; x < 15; x += 0.8f) {
      for (float z = -15; z < 15; z += 0.8f) {

        auto iMesh          = _mainMesh->createInstance("foo");
        iMesh->position().x = x;
        iMesh->position().z = z;
        iMesh->position().y = (std::abs(x) + std::abs(z)) / 10.f;

        iMesh->scaling = Vector3(1.f, 1.f, 1.f);
        iMesh->rotation().x += index / 500.f;
        _meshes.emplace_back(iMesh);

        ++index;
      }
    }

    // Create animations
    scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
      auto delta = scene->getEngine()->getDeltaTime() / 1000.f;
      _time += delta;

      // Set mesh colors
      _mainMeshMaterial->diffuseColor.r = std::sin(_time + 0.f) / 3.f + 0.3f;
      _mainMeshMaterial->diffuseColor.g = std::sin(_time + 2.f) / 3.f + 0.3f;
      _mainMeshMaterial->diffuseColor.b = std::sin(_time + 4.f) / 3.f + 0.3f;

      // Rotate meshes
      auto index = 0.f;
      for (auto& mesh : _meshes) {
        mesh->rotation().x += (std::sin(_time + 4.f) / 3.f) / 10.f;
        mesh->rotation().y += (std::sin(_time + 4.f) / 3.f) / 10.f;

        mesh->scaling().y = std::cos(index) / 2.f + 0.5f;
        mesh->scaling().x = std::sin(index) / 2.f + 0.5f;

        delta += 0.000005f;
        ++index;
      }

      // Change background color
      scene->clearColor.r = std::cos(_time / 2.f + 0.f) / 3.f + 0.6f;
      scene->clearColor.g = std::cos(_time / 2.f + 2.f) / 3.f + 0.6f;
      scene->clearColor.b = std::cos(_time / 2.f + 4.f) / 3.f + 0.6f;

      // Move camera
      _camera->position().x = std::cos(_time / 10.f) * 10.f;
      _camera->position().z = std::sin(_time / 10.f) * 10.f;
      _camera->position().y = std::cos(_time / 5.f) * 5.f + 7.f;

      // Look camera to scene center
      _camera->setTarget(Vector3::Zero());
    });
  }

private:
  float _time;
  FreeCameraPtr _camera;
  MeshPtr _mainMesh;
  StandardMaterialPtr _mainMeshMaterial;
  std::vector<InstancedMeshPtr> _meshes;

}; // end of class CubeChaosScene

BABYLON_REGISTER_SAMPLE("Optimizations", CubeChaosScene)

} // end of namespace Samples
} // end of namespace BABYLON
