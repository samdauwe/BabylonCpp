#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Sphere Textures Scene. Example demonstrating 3 spheres that are lit by the same
 * hemispheric light, with diffuse red and groundColor green. The first sphere has a diffuse
 * texture, the middle an emissive texture and the one on the right has material with red diffuse
 * color and an ambient texture.
 * @see https://www.babylonjs-playground.com/#20OAV9#15
 */
struct SphereTexturesScene : public IRenderableScene {

  SphereTexturesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~SphereTexturesScene() override = default;

  const char* getName() override
  {
    return "Sphere Textures Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is up and left
    auto light         = HemisphericLight::New("hemiLight", Vector3(-1, 1, 0), scene);
    light->diffuse     = Color3(1.f, 0.f, 0.f);
    light->specular    = Color3(0.f, 1.f, 0.f);
    light->groundColor = Color3(0.f, 1.f, 0.f);

    auto grass0            = StandardMaterial::New("grass0", scene);
    grass0->diffuseTexture = Texture::New("textures/grass.png", scene);

    auto grass1             = StandardMaterial::New("grass1", scene);
    grass1->emissiveTexture = Texture::New("textures/grass.png", scene);

    auto grass2            = StandardMaterial::New("grass2", scene);
    grass2->ambientTexture = Texture::New("textures/grass.png", scene);
    grass2->diffuseColor   = Color3(1.f, 0.f, 0.f);

    // Diffuse texture
    SphereOptions sphereOptions;
    auto sphere0          = MeshBuilder::CreateSphere("sphere0", sphereOptions, scene);
    sphere0->position().x = -1.5f;
    sphere0->material     = grass0;

    // Emissive texture
    auto sphere1      = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
    sphere1->material = grass1;

    // Ambient texture and diffuse color
    auto sphere2          = MeshBuilder::CreateSphere("sphere2", sphereOptions, scene);
    sphere2->material     = grass2;
    sphere2->position().x = 1.5f;
  }

}; // end of struct SphereTexturesScene

BABYLON_REGISTER_SAMPLE("Textures", SphereTexturesScene)

} // end of namespace Samples
} // end of namespace BABYLON
