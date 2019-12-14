#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief High Dynamic Range (HDR) cube texture scene. Example demonstrating how to create a HDR
 * skybox. High Dynamic Range (HDR) images are panoramic images that cover an entire field of
 * vision..
 * @see http://www.babylonjs-playground.com/#114YPX#5
 */
struct HdrCubeTextureScene : public IRenderableScene {

  HdrCubeTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~HdrCubeTextureScene() override = default;

  const char* getName() override
  {
    return "High Dynamic Range (HDR) Cube Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f, 200.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);
    camera->minZ = 0.1f;

    // Light
    PointLight::New("point", Vector3(20.f, 20.f, 10.f), scene);

    // Skybox
    BoxOptions boxOptions;
    boxOptions.size                    = 1000.f;
    auto skybox                        = MeshBuilder::CreateBox("SkyBox", boxOptions, scene);
    auto skyboxMaterial                = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling    = false;
    auto reflectionTexture             = HDRCubeTexture::New("textures/room.hdr", scene, 512);
    reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skybox->material                   = skyboxMaterial;
    skyboxMaterial->reflectionTexture  = reflectionTexture;
  }

}; // end of struct HDRCubeTextureScene

BABYLON_REGISTER_SAMPLE("Textures", HdrCubeTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
