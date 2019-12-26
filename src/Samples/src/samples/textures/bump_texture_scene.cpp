#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Bump Texture Scene. Example demonstrating how to use normal map to simulate bump.
 * @see https://www.babylonjs-playground.com/#RK0W5S#0
 */
class BumpTextureScene : public IRenderableScene {

public:
  BumpTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _sphere{nullptr}
  {
  }

  ~BumpTextureScene() override = default;

  const char* getName() override
  {
    return "Bump Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(-5.f, 5.f, 0.f));
    camera->attachControl(canvas, true);

    auto light       = PointLight::New("Omni", Vector3(20, 100, 2), scene);
    light->intensity = 0.97f;

    auto material                  = StandardMaterial::New("normalMap", scene);
    material->bumpTexture          = Texture::New("textures/normalMap.jpg", scene);
    material->bumpTexture()->level = 1.f;
    material->diffuseColor         = Color3(1.f, 0.f, 0.f);

    _sphere           = Mesh::CreateSphere("Sphere", 16, 3, scene);
    _sphere->material = material;

    // Animations
    scene->registerBeforeRender(
      [this](Scene* /*scene*/, EventState& /*es*/) { _sphere->rotation().y += 0.02f; });
  }

private:
  MeshPtr _sphere;

}; // end of class BumpTextureScene

BABYLON_REGISTER_SAMPLE("Textures", BumpTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
