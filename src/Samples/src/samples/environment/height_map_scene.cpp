#include <babylon/samples/environment/height_map_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

HeightMapScene::HeightMapScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _positionStepX{0.5f}, reverseSun{true}
{
}

HeightMapScene::~HeightMapScene()
{
}

const char* HeightMapScene::getName()
{
  return "Height Map Scene";
}

void HeightMapScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Light
  _spot           = PointLight::New("spot", Vector3(0.f, 30.f, 10.f), scene);
  _spot->diffuse  = Color3(1.f, 1.f, 1.f);
  _spot->specular = Color3(0.f, 0.f, 0.f);

  // Camera
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
  camera->lowerBetaLimit   = 0.1f;
  camera->upperBetaLimit   = Math::PI_2 * 0.9f;
  camera->lowerRadiusLimit = 30;
  camera->upperRadiusLimit = 250;
  camera->attachControl(canvas, true);

  // Ground
  auto groundMaterial = StandardMaterial::New("ground", scene);
  groundMaterial->setDiffuseTexture(Texture::New("textures/earth.jpg", scene));

  auto ground
    = Mesh::CreateGroundFromHeightMap("ground", "textures/worldHeightMap.jpg",
                                      200, 100, 250, 0, 10, scene, false);
  ground->setMaterial(groundMaterial);

  // Sphere to see the light's position
  _sun = Mesh::CreateSphere("sun", 10, 4, scene);
  _sun->setMaterial(StandardMaterial::New("sun", scene));
  _sun->material()->setEmissiveColor(Color3(1.f, 1.f, 0.f));

  // Skybox
  auto skybox         = Mesh::CreateBox("skyBox", 800.f, scene);
  auto skyboxMaterial = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->setBackFaceCulling(false);
  skyboxMaterial->setReflectionTexture(
    new CubeTexture("textures/skybox", scene));
  skyboxMaterial->reflectionTexture()->setCoordinatesMode(
    TextureConstants::SKYBOX_MODE);
  skyboxMaterial->diffuseColor  = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->setDisableLighting(true);
  skybox->setMaterial(skyboxMaterial);

  // Sun animation
  scene->registerBeforeRender([&](Scene* /*scene*/, EventState& /*es*/) {
    _sun->setPosition(_spot->position());
    _spot->position().x -= _positionStepX;
    if (_spot->position().x < -90.f) {
      if (reverseSun) {
        _positionStepX = -_positionStepX;
      }
      else {
        _spot->position().x = 100.f;
      }
    }
    if (_spot->position().x > 90.f && reverseSun) {
      _positionStepX = -_positionStepX;
    }
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
