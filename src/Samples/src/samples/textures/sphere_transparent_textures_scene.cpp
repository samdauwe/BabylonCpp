#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Sphere Transparent Textures Scene. Example demonstrating how to achieve transparency by
 * setting a materials alpha property from 0 (invisible) to 1 (opaque).
 * @see http://www.babylonjs-playground.com/#20OAV9#17
 */
struct SphereTransparentTexturesScene : public IRenderableScene {

  SphereTransparentTexturesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~SphereTransparentTexturesScene() override = default;

  const char* getName() override
  {
    return "Sphere Transparent Textures Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, 3 * Math::PI / 8.f, 5.f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light direction is up and left
    auto light         = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
    light->diffuse     = Color3(1.f, 0.f, 0.f);
    light->specular    = Color3(0.f, 1.f, 0.f);
    light->groundColor = Color3(0.f, 1.f, 0.f);

    auto redMat            = StandardMaterial::New("redMat", scene);
    redMat->diffuseTexture = Texture::New("textures/grass.png", scene);

    auto greenMat            = StandardMaterial::New("greenMat", scene);
    greenMat->diffuseTexture = Texture::New("textures/grass.png", scene);
    greenMat->alpha          = 0.5f;

    // Red
    SphereOptions sphereOptions;
    auto sphere1          = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
    sphere1->material     = redMat;
    sphere1->position().z = 1.5f;

    // Green Transparent
    auto sphere2      = MeshBuilder::CreateSphere("sphere2", sphereOptions, scene);
    sphere2->material = greenMat;
  }

}; // end of struct SphereTransparentTexturesScene

BABYLON_REGISTER_SAMPLE("Textures", SphereTransparentTexturesScene)

} // end of namespace Samples
} // end of namespace BABYLON
