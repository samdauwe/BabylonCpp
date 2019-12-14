#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cube texture different faces scene. Example demonstrating how to create a box with
 * different textures on each face.
 * @see https://www.babylonjs-playground.com/#UU7RQ#2
 */
struct CubeTextureDifferentFacesScene : public IRenderableScene {

  CubeTextureDifferentFacesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~CubeTextureDifferentFacesScene() override = default;

  const char* getName() override
  {
    return "Cube texture different faces scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light     = HemisphericLight::New("hemiLight", Vector3(-1, 1, 0), scene);
    light->diffuse = Color3(1.f, 0.f, 0.f);

    // Box with different textures on each face.
    BoxOptions boxOptions;
    auto box                           = MeshBuilder::CreateBox("Box", boxOptions, scene);
    auto boxMaterial                   = StandardMaterial::New("mat", scene);
    boxMaterial->backFaceCulling       = true;
    auto reflectionTexture             = CubeTexture::New("textures/skybox/cubeSide", scene);
    reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
    boxMaterial->reflectionTexture     = reflectionTexture;
    boxMaterial->diffuseColor          = Color3(0, 0, 0);
    boxMaterial->specularColor         = Color3(0, 0, 0);
    box->material                      = boxMaterial;

    box->rotation().y = Math::PI / 8.f;
    box->rotation().x = -Math::PI / 8.f;
  }

}; // end of struct CubeTextureDifferentFacesScene

BABYLON_REGISTER_SAMPLE("Textures", CubeTextureDifferentFacesScene)

} // end of namespace Samples
} // end of namespace BABYLON
