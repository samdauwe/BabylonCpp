#include <babylon/samples/textures/hdr_cube_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

HDRCubeTextureScene::HDRCubeTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

HDRCubeTextureScene::~HDRCubeTextureScene()
{
}

const char* HDRCubeTextureScene::getName()
{
  return "High Dynamic Range (HDR) cube texture scene";
}

void HDRCubeTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f,
                                     200, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);
  camera->minZ = 0.1f;

  // Light
  PointLight::New("point", Vector3(20.f, 20.f, 10.f), scene);

  // Skybox
  BoxOptions boxOptions(1000.f);
  auto skybox         = MeshBuilder::CreateBox("SkyBox", boxOptions, scene);
  auto skyboxMaterial = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  auto reflectionTexture = HDRCubeTexture::New("textures/room.hdr", scene, 512);
  reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  skybox->material                   = skyboxMaterial;
  skyboxMaterial->reflectionTexture  = reflectionTexture;
}

} // end of namespace Samples
} // end of namespace BABYLON
