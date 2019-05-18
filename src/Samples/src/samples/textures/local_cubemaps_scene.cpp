#include <babylon/samples/textures/local_cubemaps_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

LocalCubemapsScene::LocalCubemapsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _reflectionTexture{nullptr}
{
}

LocalCubemapsScene::~LocalCubemapsScene()
{
}

const char* LocalCubemapsScene::getName()
{
  return "Local Cubemaps Scene";
}

void LocalCubemapsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Camera
  auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f,
                                     Vector3(0.f, 1.f, 0.f), scene);

  camera->setPosition(Vector3(-15.f, 3.f, 0.f));
  camera->attachControl(canvas, true);

  camera->minZ             = 0.01f;
  camera->upperRadiusLimit = 50.f;
  camera->lowerRadiusLimit = 2.f;
  camera->upperBetaLimit   = Math::PI_2;

  // Light
  auto light = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);
  light->intensity = 0.98f;

  // Ground
  auto ground   = Mesh::CreateGround("ground", 100, 100, 1, scene);
  auto material = StandardMaterial::New("kosh", scene);
  _reflectionTexture
    = CubeTexture::New("/textures/skybox/TropicalSunnyDay", scene);
  _reflectionTexture->boundingBoxSize = Vector3(100.f, 100.f, 100.f);
  material->reflectionTexture         = _reflectionTexture;
  material->disableLighting           = true;
  ground->material                    = material;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("/textures/skybox/TropicalSunnyDay", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->disableLighting = true;
  skybox->material                = skyboxMaterial;

  // Events
  canvas->addKeyEventListener(EventType::KEY_DOWN, [this](KeyboardEvent&& evt) {
    switch (evt.keyCode) {
      case 49: // 1
      {
        if (!_reflectionTexture->boundingBoxSize().has_value()) {
          _reflectionTexture->boundingBoxSize = Vector3(100.f, 100.f, 100.f);
        }
        else {
          _reflectionTexture->boundingBoxSize = std::nullopt;
        }
      } break;
      default:
        break;
    }
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
