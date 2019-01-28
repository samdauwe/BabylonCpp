#include <babylon/samples/specialfx/realtime_refraction_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/probes/reflection_probe.h>

namespace BABYLON {
namespace Samples {

RealtimeRefractionScene::RealtimeRefractionScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _yellowSphere{nullptr}, _greenSphere{nullptr}
{
}

RealtimeRefractionScene::~RealtimeRefractionScene()
{
}

const char* RealtimeRefractionScene::getName()
{
  return "Realtime Refraction Scene";
}

void RealtimeRefractionScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 10.f, Vector3::Zero(), scene);

  camera->setPosition(Vector3(0.f, 5.f, -10.f));
  camera->attachControl(canvas, true);

  camera->upperBetaLimit   = Math::PI_2;
  camera->lowerRadiusLimit = 4.f;

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.7f;

  auto knot = Mesh::CreateTorusKnot("knot", 1, 0.4f, 128, 64, 2, 3, scene);

  _yellowSphere = Mesh::CreateSphere("yellowSphere", 16, 1.5, scene);
  _yellowSphere->setPivotMatrix(Matrix::Translation(3.f, 0.f, 0.f), false);
  auto yellowMaterial          = StandardMaterial::New("yellowMaterial", scene);
  yellowMaterial->diffuseColor = Color3::Yellow();
  _yellowSphere->material      = yellowMaterial;

  _greenSphere = Mesh::CreateSphere("greenSphere", 16, 1.5, scene);
  _greenSphere->setPivotMatrix(Matrix::Translation(0.f, 0.f, 3.f), false);
  auto greenMaterial          = StandardMaterial::New("greenMaterial", scene);
  greenMaterial->diffuseColor = Color3::Green();
  _greenSphere->material      = greenMaterial;

  // Ground
  auto ground                    = Mesh::CreateBox("Mirror", 1.f, scene);
  ground->scaling                = Vector3(100.f, 0.01f, 100.f);
  auto groundMaterial            = StandardMaterial::New("ground", scene);
  auto diffuseTexture            = Texture::New("/textures/amiga.jpg", scene);
  diffuseTexture->uScale         = 10.f;
  diffuseTexture->vScale         = 10.f;
  groundMaterial->diffuseTexture = diffuseTexture;
  ground->position               = Vector3(0.f, -2.f, 0.f);
  ground->material               = groundMaterial;

  // Main material
  auto mainMaterial = StandardMaterial::New("main", scene);
  knot->material    = mainMaterial;

  auto probe = ReflectionProbe::New("main", 512, scene);
  probe->renderList().emplace_back(_yellowSphere.get());
  probe->renderList().emplace_back(_greenSphere.get());
  probe->renderList().emplace_back(ground.get());
  mainMaterial->diffuseColor                         = Color3(1.f, 0.5f, 0.5f);
  mainMaterial->reflectionTexture                    = probe->cubeTexture();
  mainMaterial->reflectionFresnelParameters()->bias  = 0.5f;
  mainMaterial->reflectionFresnelParameters()->power = 16.f;
  mainMaterial->reflectionFresnelParameters()->leftColor  = Color3::Black();
  mainMaterial->reflectionFresnelParameters()->rightColor = Color3::White();
  mainMaterial->reflectionFresnelParameters()->isEnabled  = false;
  mainMaterial->indexOfRefraction                         = 1.05f;

  // Fog
  scene->fogMode = Scene::FOGMODE_LINEAR;
  scene->fogColor
    = Color3(scene->clearColor.r, scene->clearColor.g, scene->clearColor.b);
  scene->fogStart = 20.f;
  scene->fogEnd   = 50.f;

  // Animations
  scene->registerBeforeRender([this](Scene*, EventState&) {
    _yellowSphere->rotation().y += 0.01f;
    _greenSphere->rotation().y += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
