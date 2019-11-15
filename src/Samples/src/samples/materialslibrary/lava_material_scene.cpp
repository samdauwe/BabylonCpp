#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/lava/lava_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the lava material from the materials library.
 * @see https://www.babylonjs-playground.com/#1BLVWO#25
 * @see https://doc.babylonjs.com/extensions/Lava
 */
struct LavaMaterialScene : public IRenderableScene {

  LavaMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~LavaMaterialScene() override = default;

  const char* getName() override
  {
    return "Lava Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 50.f, -300.f), scene);

    // This targets the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // This attaches the camera to the canvas
    camera->attachControl(canvas, true);

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 1.f;

    auto ground = Mesh::CreateGround("ground", 500, 500, 100, scene);

    // Lava Material creation
    auto lavaMaterial = MaterialsLibrary::LavaMaterial::New("lava", scene);
    // Set the bump texture
    lavaMaterial->noiseTexture = Texture::New("textures/lava/cloud.png", scene);
    // Set the diffuse texture
    lavaMaterial->diffuseTexture = Texture::New("textures/lava/lavatile.jpg", scene);
    lavaMaterial->speed          = 1.5f;
    lavaMaterial->fogColor       = Color3(1.f, 0.f, 0.f);

    ground->material = lavaMaterial;
  }

}; // end of struct LavaMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", LavaMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
