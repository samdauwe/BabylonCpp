#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Shadows Scene. Example scene demonstrating how to setup a scene with lights and meshes to
 * cast different types of shadows.
 * @see https://www.babylonjs-playground.com/#IFYDRS#0
 */
class ShadowsScene : public IRenderableScene {

public:
  ShadowsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _torus{nullptr}, _alpha{0.f}
  {
  }

  ~ShadowsScene() override = default;

  const char* getName() override
  {
    return "Shadows Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Setup environment
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 90.f, Vector3::Zero(), scene);
    camera->lowerBetaLimit   = 0.1f;
    camera->upperBetaLimit   = Math::PI_2 * 0.9f;
    camera->lowerRadiusLimit = 30.f;
    camera->upperRadiusLimit = 150.f;
    camera->attachControl(canvas, true);

    // light1
    auto light       = DirectionalLight::New("dir01", Vector3(-1.f, -2.f, -1.f), scene);
    light->position  = Vector3(20.f, 40.f, 20.f);
    light->intensity = 0.5f;

    auto lightSphere                   = Mesh::CreateSphere("sphere", 10, 2, scene);
    lightSphere->position              = light->position;
    auto lightSphereMaterial           = StandardMaterial::New("light", scene);
    lightSphereMaterial->emissiveColor = Color3(1.f, 1.f, 0.f);
    lightSphere->material              = lightSphereMaterial;

    // light2
    auto light2 = SpotLight::New("spot02", Vector3(30.f, 40.f, 20.f), Vector3(-1.f, -2.f, -1.f),
                                 1.1f, 16.f, scene);
    light2->intensity = 0.5f;

    auto lightSphere2                   = Mesh::CreateSphere("sphere", 10, 2, scene);
    lightSphere2->position              = light2->position;
    auto lightSphere2Material           = StandardMaterial::New("light", scene);
    lightSphere2Material->emissiveColor = Color3(1.f, 1.f, 0.f);
    lightSphere2->material              = lightSphere2Material;

    // Ground
    auto ground = Mesh::CreateGroundFromHeightMap("ground", "textures/heightMap.png", 100, 100, 100,
                                                  0, 10, scene, false);
    auto groundMaterial            = StandardMaterial::New("ground", scene);
    auto diffuseTexture            = Texture::New("textures/ground.jpg", scene);
    diffuseTexture->uScale         = 6.f;
    diffuseTexture->vScale         = 6.f;
    groundMaterial->diffuseTexture = diffuseTexture;
    groundMaterial->specularColor  = Color3(0.f, 0.f, 0.f);
    ground->position().y           = -2.05f;
    ground->material               = groundMaterial;

    // Torus
    _torus = Mesh::CreateTorus("torus", 4, 2, 30, scene, false);

    // Shadows
    auto shadowGenerator = ShadowGenerator::New(1024, light);
    shadowGenerator->addShadowCaster(_torus);
    shadowGenerator->useExponentialShadowMap = true;

    auto shadowGenerator2 = ShadowGenerator::New(1024, light2);
    shadowGenerator2->addShadowCaster(_torus);
    shadowGenerator2->usePoissonSampling = true;

    ground->receiveShadows = true;

    // Animations
    scene->registerBeforeRender([this](Scene*, EventState&) {
      _torus->rotation().x += 0.01f;
      _torus->rotation().z += 0.02f;

      _torus->position = Vector3(std::cos(_alpha) * 30.f, 10.f, std::sin(_alpha) * 30.f);
      _alpha += 0.01f;
    });
  }

private:
  MeshPtr _torus;
  float _alpha;

}; // end of class ShadowsScene

BABYLON_REGISTER_SAMPLE("Shadows", ShadowsScene)

} // end of namespace Samples
} // end of namespace BABYLON
