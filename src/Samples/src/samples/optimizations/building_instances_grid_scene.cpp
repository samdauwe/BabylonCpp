#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/core/time.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Building Instances Grid Scene. Example demonstrating how to create a large grid of boxes
 * using instanced based rendering.
 * @see https://www.babylonjs-playground.com/#9PK6N#4
 */
class BuildingInstancesGridScene : public IRenderableScene {

public:
  BuildingInstancesGridScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _sun{nullptr}, _light2{nullptr}, _alpha{0.f}
  {
  }

  ~BuildingInstancesGridScene() override = default;

  const char* getName() override
  {
    return "Building Instances Grid Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Adding a light
    auto light       = PointLight::New("Omni", Vector3(20.f, 100.f, 20.f), scene);
    light->diffuse   = Color3(0.1f, 0.1f, 0.17f);
    _light2          = DirectionalLight::New("dirlight", Vector3(1.f, -0.75f, 0.25f), scene);
    _light2->diffuse = Color3(0.95f, 0.7f, 0.4f);

    // Adding an Arc Rotate Camera
    auto camera = ArcRotateCamera::New("Camera", 0.2f, 1.27f, 45.f, Vector3::Zero(), scene);
    camera->setTarget(Vector3(0.f, 1.f, 0.f));
    camera->attachControl(canvas, false);
    camera->lowerRadiusLimit = 1.f;

    // Sun
    _sun                  = Mesh::CreateSphere("sun", 8, 30, scene);
    auto sunMat           = StandardMaterial::New("sunmat", scene);
    sunMat->emissiveColor = Color3(1.f, 1.f, 0.9f);
    sunMat->specularColor = Color3(0.f, 0.f, 0.f);
    _sun->material        = sunMat;

    // Generate ground
    auto ground                   = Mesh::CreateGround("ground1", 220, 220, 2, scene);
    auto groundMaterial           = StandardMaterial::New("ground", scene);
    groundMaterial->diffuseColor  = Color3(0.3f, 0.3f, 0.4f);
    groundMaterial->specularColor = Color3(0.1f, 0.1f, 0.1f);
    groundMaterial->specularPower = 1.f;
    ground->material              = groundMaterial;

    // Material
    auto material           = StandardMaterial::New("building", scene);
    material->diffuseColor  = Color3(0.8f, 0.8f, 0.85f);
    material->specularColor = Color3(0.4f, 0.4f, 0.4f);
    material->specularPower = 4.f;

    // Buildings
    auto mesh      = Mesh::CreateBox("building", 1.f, scene);
    mesh->material = material;

    float grid_size    = 70.f;
    float grid_spacing = 3.5f;
    for (float i = 0; i < grid_size; ++i) {
      for (float j = 0; j < grid_size; ++j) {

        float diff = std::min(std::abs(j - grid_size / 2.f), std::abs(i - grid_size / 2.f));
        if (diff < 2.f) {
          continue;
        }

        auto inst = mesh->createInstance("building_inst");
        auto size = 5.f + Math::random() * 10.f;

        size *= std::min(1.f, 0.2f + 1.f * (diff / (grid_size / 2.f)));

        inst->position().y = size / 2.f;
        inst->scaling().y  = size;
        inst->scaling().x  = grid_spacing * 0.5f + Math::random() * grid_spacing * 0.4f;
        inst->scaling().z  = inst->scaling().x;
        inst->position().x = -grid_size * 0.425f * grid_spacing + i * grid_spacing * 0.85f
                             + Math::random() * 0.15f * grid_spacing;
        inst->position().z = -grid_size * 0.425f * grid_spacing + j * grid_spacing * 0.85f
                             + Math::random() * 0.15f * grid_spacing;
      }
    }

    mesh->setEnabled(false);

    // Move the light with the camera
    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      auto alt           = std::sin(_alpha);
      auto color         = Color3::Lerp(Color3(1.f, 1.f, 0.5f), Color3(1.f, 0.f, 0.f), 1.f + alt);
      _light2->direction = Vector3(0.25f, alt, std::cos(_alpha));
      _light2->diffuse   = color;
      _light2->intensity = 1.f - std::pow(1.f - std::abs(alt), 3.f);
      _sun->position().copyFrom(_light2->direction());
      _sun->position().scaleInPlace(-200.f);
      _alpha += 0.01f;
    });
  }

private:
  MeshPtr _sun;
  DirectionalLightPtr _light2;
  float _alpha;

}; // end of class BuildingInstancesGridScene

BABYLON_REGISTER_SAMPLE("Optimizations", BuildingInstancesGridScene)

} // end of namespace Samples
} // end of namespace BABYLON
