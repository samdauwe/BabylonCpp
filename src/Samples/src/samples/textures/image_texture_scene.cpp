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
 * @brief Image Texture Scene. Example demonstrating how to apply an image with transparency to a
 * mesh.
 * @see https://www.babylonjs-playground.com/#YDO1F#75
 */
struct ImageTextureScene : public IRenderableScene {

  ImageTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImageTextureScene() override = default;

  const char* getName() override
  {
    return "Image Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Scene setup
    auto camera
      = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI_2, 2.5f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);
    auto light       = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
    light->intensity = 0.7f;

    // Create material from image with alpha
    auto mat                        = StandardMaterial::New("dog", scene);
    mat->diffuseTexture             = Texture::New("/textures/alaskanMalamuteBlank.png", scene);
    mat->diffuseTexture()->hasAlpha = true;
    mat->backFaceCulling            = false;

    // Apply material to a box
    BoxOptions options;
    auto box      = MeshBuilder::CreateBox("box", options, scene);
    box->material = mat;
  }

}; // end of struct ImageTextureScene

BABYLON_REGISTER_SAMPLE("Textures", ImageTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
