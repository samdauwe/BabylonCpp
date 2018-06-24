#include <babylon/samples/lights/light_projected_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

LightProjectedTextureScene::LightProjectedTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _spotLight{nullptr}, _alpha{0.f}
{
}

LightProjectedTextureScene::~LightProjectedTextureScene()
{
}

const char* LightProjectedTextureScene::getName()
{
  return "Light Projected Texture Scene";
}

void LightProjectedTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Setup environment
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 90.f, Vector3::Zero(), scene);
  camera->lowerBetaLimit   = 0.1f;
  camera->upperBetaLimit   = Math::PI_2 * 0.9f;
  camera->lowerRadiusLimit = 30;
  camera->upperRadiusLimit = 150;
  camera->attachControl(canvas, true);

  // Hemispheric light
  auto light = HemisphericLight::New("dir01", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.1f;

  // Spot light
  _spotLight = SpotLight::New("spot02", Vector3(30.f, 40.f, 30.f),
                              Vector3(-1.f, -2.f, -1.f), 1.1f, 16.f, scene);
  _spotLight->projectionTexture = Texture::New("textures/co.png", scene);
  _spotLight->setDirectionToTarget(Vector3::Zero());
  _spotLight->intensity = 1.5f;

  // Ground
  auto ground = Mesh::CreateGroundFromHeightMap(
    "ground", "textures/heightMap.png", 100, 100, 100, 0, 10, scene, false);
  auto groundMaterial = StandardMaterial::New("ground", scene);
  groundMaterial->setDiffuseTexture(Texture::New("textures/ground.jpg", scene));
  auto diffuseTexture = static_cast<Texture*>(groundMaterial->diffuseTexture());
  diffuseTexture->uScale        = 6.f;
  diffuseTexture->vScale        = 6.f;
  groundMaterial->specularColor = Color3(0.f, 0.f, 0.f);
  ground->position().y          = -2.05f;
  ground->material              = groundMaterial;

  // Animations
  scene->registerBeforeRender([&](Scene* /*scene*/, EventState& /*es*/) {
    _spotLight->setPosition(
      Vector3(::std::cos(_alpha) * 60.f, 40.f, ::std::sin(_alpha) * 60.f));
    _spotLight->setDirectionToTarget(Vector3::Zero());
    _alpha += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
