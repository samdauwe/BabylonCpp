#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Raycast on height map scene. Example demonstrating how to raycast to find positions on a
 * heightmap.
 * @see https://www.babylonjs-playground.com/#QM57B#0
 * @see https://doc.babylonjs.com/babylon101/raycasts
 */
class RaycastOnHeightMapScene : public IRenderableScene {

public:
  RaycastOnHeightMapScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _angle{0.f}, _ground{nullptr}, _sphere{nullptr}
  {
  }

  ~RaycastOnHeightMapScene() override = default;

  const char* getName() override
  {
    return "Raycast On Height Map Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    auto sun       = PointLight::New("Omni0", Vector3(60.f, 100.f, 10.f), scene);
    sun->intensity = 0.98f;

    camera->setPosition(Vector3(-20.f, 20.f, 0.f));
    camera->attachControl(canvas);

    // Ground
    _ground = Mesh::CreateGroundFromHeightMap("ground", "textures/worldHeightMap.jpg", 100, 100,
                                              100, 0, 10, scene, false);
    auto groundMaterial            = StandardMaterial::New("ground", scene);
    groundMaterial->diffuseTexture = Texture::New("textures/earth.jpg", scene);

    groundMaterial->specularColor = Color3(0.f, 0.f, 0.f);
    _ground->position().y         = -2.05f;
    _ground->material             = groundMaterial;

    // Sphere to see the light's position
    _sphere               = Mesh::CreateSphere("sun", 16, 1, scene);
    _sphere->position().y = 10.f;

    // Animation
    scene->registerBeforeRender([&](Scene* /*scene*/, EventState& /*es*/) {
      _sphere->position().x = 10.f * std::cos(_angle);
      _sphere->position().z = 10.f * std::sin(_angle);

      _angle += 0.01f;

      // Casting a ray to get height
      Ray ray(Vector3(_sphere->position().x,
                      _ground->getBoundingInfo()->boundingBox.maximumWorld.y + 1.f,
                      _sphere->position().z),
              Vector3(0.f, -1.f, 0.f)); // Direction

      Matrix worldInverse;
      _ground->getWorldMatrix().invertToRef(worldInverse);

      ray           = Ray::Transform(ray, worldInverse);
      auto pickInfo = _ground->intersects(ray);

      if (pickInfo.hit) {
        _sphere->position().y = pickInfo.pickedPoint->y + 0.5f;
      }
    });
  }

private:
  float _angle;
  MeshPtr _ground, _sphere;

}; // end of class RaycastOnHeightMapScene

BABYLON_REGISTER_SAMPLE("Meshes", RaycastOnHeightMapScene)

} // namespace Samples
} // end of namespace BABYLON
