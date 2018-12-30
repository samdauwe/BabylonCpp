#include <babylon/samples/meshes/look_at_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/probes/reflection_probe.h>

namespace BABYLON {
namespace Samples {

LookAtScene::LookAtScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _t{0.f}
    , _camera{nullptr}
    , _cubesMat{nullptr}
    , _sphereMat{nullptr}
    , _sphere{nullptr}
    , _randomColor{Color3::Black()}
    , _randomDirection{Vector3::Zero()}
{
}

LookAtScene::~LookAtScene()
{
}

const char* LookAtScene::getName()
{
  return "Look At Scene";
}

float LookAtScene::_rnd(float min, float max) const
{
  return std::floor(Math::random() * (max - min + 1.f) + min);
}

void LookAtScene::_createCubesBall(size_t num, Scene* scene)
{
  if (_cubes.size() < num) {
    _cubes.resize(num);
  }

  MeshPtr firstMesh = nullptr;
  for (size_t i = 0; i < num; ++i) {
    if (i == 0) {
      firstMesh = Mesh::CreateBox("b", 1.f, scene);
      _cubes[i] = firstMesh;
    }
    else {
      _cubes[i] = firstMesh->createInstance("b" + std::to_string(i));
    }

    _cubes[i]->scaling
      = Vector3(_rnd(1.f, 1.5f), _rnd(1.f, 1.5f), _rnd(1.f, 10.f));
    _cubes[i]->position
      = Vector3(_rnd(-50.f, 50.f), _rnd(-50.f, 50.f), _rnd(-50.f, 50.f));
    _cubes[i]->lookAt(Vector3::Zero());
  }
}

void LookAtScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color3::Black();

  _camera = ArcRotateCamera::New("Camera", 33.7081f, 0.9001f, 39.91f,
                                 Vector3::Zero(), scene);
  _camera->setTarget(Vector3::Zero());
  _camera->attachControl(canvas, false);

  auto light       = PointLight::New("light1", Vector3(0.f, 10.f, 0.f), scene);
  light->intensity = 0.98f;

  auto light1       = HemisphericLight::New("hemi", Vector3(0, 10, 0), scene);
  light1->intensity = 0.98f;

  _sphere = Mesh::CreateSphere("s", 32, 5, scene);

  _createCubesBall(1000, scene);

  _sphereMat        = StandardMaterial::New("s", scene);
  _sphere->material = _sphereMat;

  auto probe = ReflectionProbe::New("probe", 512, scene);
  probe->renderList().emplace_back(_sphere.get());

  _cubesMat           = StandardMaterial::New("m", scene);
  _cubes[0]->material = _cubesMat;

  auto container      = Mesh::CreateBox("cont", 110, scene);
  auto matCont        = StandardMaterial::New("mc", scene);
  matCont->alpha      = 0.1f;
  container->material = matCont;

  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    // sin/cos random direction
    _randomDirection.x = 25.f * std::cos(_t / 3.5f);
    _randomDirection.y = 25.f + 10.f * std::sin(_t / 4.f);
    _randomDirection.z = 25.f * std::cos(_t / 4.5f);

    // sin/cos random color between 0,1
    _randomColor.r = 0.5f + 0.5f * std::sin(_t / 12.f);
    _randomColor.g = 0.5f + 0.5f * std::sin(_t / 14.f);
    _randomColor.b = 0.5f + 0.5f * std::sin(_t / 16.f);

    // Change sphere and cubes colors
    _sphereMat->diffuseColor  = _randomColor;
    _sphereMat->emissiveColor = _randomColor;
    _cubesMat->diffuseColor   = _randomColor;

    // Move our sphere
    _sphere->position = _randomDirection;

    // Make all cubes look at the moving sphere
    for (auto& cube : _cubes) {
      cube->lookAt(_randomDirection);
    }

    _camera->alpha  = 4.f * (Math::PI / 20.f + std::cos(_t / 30.f));
    _camera->beta   = 2.f * (Math::PI / 20.f + std::sin(_t / 50.f));
    _camera->radius = 180.f + (-50.f + 50.f * std::sin(_t / 10.f));

    _t += 0.1f * getScene()->getAnimationRatio();
  });
}

} // end of namespace Samples
} // namespace BABYLON
