#include <babylon/samples/cameras/follow_camera_scene.h>

#include <babylon/cameras/follow_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

FollowCameraScene::FollowCameraScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _box{nullptr}
    , _camera{nullptr}
    , _alpha{0.f}
    , _orbitRadius{20.f}
{
}

FollowCameraScene::~FollowCameraScene()
{
}

const char* FollowCameraScene::getName()
{
  return "Follow Camera Scene";
}

void FollowCameraScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates and initially positions a follow camera
  _camera = FollowCamera::New("FollowCam", Vector3(0.f, 10.f, -10.f), scene);

  // The goal distance of camera from target
  _camera->radius = 30.f;

  // The goal height of camera above local origin (centre) of target
  _camera->heightOffset = 10.f;

  // The goal rotation of camera around local origin (centre) of target in x y
  // plane
  _camera->rotationOffset = 0.f;

  // Acceleration of camera in moving from current to goal position
  _camera->cameraAcceleration = 0.005f;

  // The speed at which acceleration is halted
  _camera->maxCameraSpeed = 10.f;

  // Camera.target is set after the target's creation

  // This attaches the camera to the canvas
  _camera->attachControl(canvas, true);

  // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
  auto light = HemisphericLight::New("light", Vector3(0.f, 1.f, 0.f), scene);

  // Material
  auto mat            = StandardMaterial::New("mat1", scene);
  mat->alpha          = 1.f;
  mat->diffuseColor   = Color3(0.5f, 0.5f, 1.f);
  auto texture        = Texture::New("textures/spriteAtlas.png", scene);
  mat->diffuseTexture = texture;

  // Different face for each side of box to show camera rotation
  unsigned int hSpriteNb = 6; // 6 sprites per row
  unsigned int vSpriteNb = 4; // 4 sprite rows

  std::array<Vector4, 6> faceUV;
  for (unsigned int i = 0; i < 6; i++) {
    faceUV[i] = Vector4(i / static_cast<float>(hSpriteNb), 0.f,
                        (i + 1) / static_cast<float>(hSpriteNb),
                        1 / static_cast<float>(vSpriteNb));
  }

  // Shape to follow
  BoxOptions boxOptions;
  boxOptions.size   = 2.f;
  boxOptions.faceUV = faceUV;
  _box              = MeshBuilder::CreateBox("box", boxOptions, scene);
  _box->position    = Vector3(20.f, 0.f, 10.f);
  _box->material    = mat;

  // Add 400 boxes
  const auto getRandomPosition = []() -> Vector3 {
    return Vector3(-50.f + Math::random() * 100.f,
                   -50.f + Math::random() * 100.f,
                   -50.f + Math::random() * 100.f);
  };

  boxOptions      = BoxOptions();
  boxOptions.size = 2.f;
  auto box        = MeshBuilder::CreateBox("box_0", boxOptions, scene);
  box->position   = getRandomPosition();

  for (unsigned int i = 1; i < 400; ++i) {
    const auto iStr       = std::to_string(i);
    auto boxInstance      = box->createInstance("box_" + iStr);
    boxInstance->position = getRandomPosition();
  }

  // Set target for camera
  _camera->lockedTarget = _box;

  // Move the box to see that the camera follows it
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _alpha += 0.01f;
    _box->position().x = _orbitRadius * std::cos(_alpha);
    _box->position().y = _orbitRadius * std::sin(_alpha);
    _box->position().z = 10.f * std::sin(2.f * _alpha);

    // Change the viewing angle of the camera as it follows the box
    _camera->rotationOffset = std::fmod(18.f * _alpha, 360.f);
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
