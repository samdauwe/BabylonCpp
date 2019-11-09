#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Bump Texture Spheres Scene. Example demonstrating how to use normal map to simulate bump
 * on multiple spheres.
 * @see https://www.babylonjs-playground.com/#20OAV9#23
 */
class BumpTextureSpheresScene : public IRenderableScene {

public:
  BumpTextureSpheresScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _sphere{nullptr}
  {
  }

  ~BumpTextureSpheresScene() override = default;

  const char* getName() override
  {
    return "Bump Texture Spheres Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 4.f, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light     = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
    light->diffuse = Color3(1.f, 1.f, 1.f);

    // Material 0
    auto mat0          = StandardMaterial::New("mat0", scene);
    mat0->diffuseColor = Color3(1.f, 0.f, 0.f);
    mat0->bumpTexture  = Texture::New("textures/normalMap.png", scene);

    // Material 1
    auto mat1            = StandardMaterial::New("mat1", scene);
    mat1->diffuseTexture = Texture::New("textures/stone_bump.png", scene);
    mat1->bumpTexture    = Texture::New("textures/normalMap.png", scene);

    // Material 2
    auto mat2            = StandardMaterial::New("mat2", scene);
    mat2->diffuseTexture = Texture::New("textures/grass.png", scene);
    mat2->bumpTexture    = Texture::New("textures/normalMap.png", scene);

    // Sphere 0
    auto _sphere0          = Mesh::CreateSphere("Sphere0", 16, 1, scene);
    _sphere0->position().x = -1.5f;
    _sphere0->material     = mat0;

    // Sphere 1
    auto _sphere1      = Mesh::CreateSphere("Sphere1", 16, 1, scene);
    _sphere1->material = mat1;

    // Sphere 2
    auto _sphere2          = Mesh::CreateSphere("Sphere2", 16, 1, scene);
    _sphere2->position().x = 1.5f;
    _sphere2->material     = mat2;
  }

private:
  MeshPtr _sphere;

}; // end of class BumpTextureSpheresScene

BABYLON_REGISTER_SAMPLE("Textures", BumpTextureSpheresScene)

} // end of namespace Samples
} // end of namespace BABYLON
