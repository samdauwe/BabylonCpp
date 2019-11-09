#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/mix/mix_material.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the mix material from the materials library.
 * @see https://www.babylonjs-playground.com/#1DFTDT
 */
struct MixMaterialScene : public IRenderableScene {

  MixMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MixMaterialScene() override = default;

  const char* getName() override
  {
    return "Mix Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = FreeCamera::New("camera1", Vector3(5.f, 4.f, -47.f), scene);
    camera->setTarget(Vector3::Zero());
    camera->attachControl(canvas, true);

    // Light
    auto light = HemisphericLight::New("light", Vector3(0.f, 1.f, 0.f), scene);

    // Create mix material
    auto mix = MaterialsLibrary::MixMaterial::New("mix", scene);

    // Setup mix texture 1
    mix->mixTexture1     = Texture::New("textures/mixMap.png", scene);
    mix->diffuseTexture1 = Texture::New("textures/floor.png", scene);
    mix->diffuseTexture2 = Texture::New("textures/rock.png", scene);
    mix->diffuseTexture3 = Texture::New("textures/grass.png", scene);
    mix->diffuseTexture4 = Texture::New("textures/floor.png", scene);

    mix->diffuseTexture1()->uScale = mix->diffuseTexture1()->vScale = 10.f;
    mix->diffuseTexture2()->uScale = mix->diffuseTexture2()->vScale = 10.f;
    mix->diffuseTexture3()->uScale = mix->diffuseTexture3()->vScale = 10.f;
    mix->diffuseTexture4()->uScale = mix->diffuseTexture4()->vScale = 10.f;

    // Setup mix texture 2
    mix->mixTexture2     = Texture::New("textures/mixMap_2.png", scene);
    mix->diffuseTexture5 = Texture::New("textures/leopard_fur.jpg", scene);
    mix->diffuseTexture6 = Texture::New("textures/fur.jpg", scene);
    mix->diffuseTexture7 = Texture::New("textures/sand.jpg", scene);
    mix->diffuseTexture8 = Texture::New("textures/crate.png", scene);

    mix->diffuseTexture5()->uScale = mix->diffuseTexture5()->vScale = 10.f;
    mix->diffuseTexture6()->uScale = mix->diffuseTexture6()->vScale = 10.f;
    mix->diffuseTexture7()->uScale = mix->diffuseTexture7()->vScale = 5.f;
    mix->diffuseTexture8()->uScale = mix->diffuseTexture8()->vScale = 10.f;

    // Ground
    auto ground = Mesh::CreateGroundFromHeightMap("ground", "textures/heightMap.png", 100, 100, 100,
                                                  0, 10, scene, false);
    ground->position().y = -2.05f;
    ground->material     = mix;
  }

}; // end of struct MixMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", MixMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
