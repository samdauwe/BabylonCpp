#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Bouncing Cube Scene.
 *
 * In this simple example, you've got :
 * - a finite world defined by the big box
 * - some random platforms inside it, built with instances
 * - a Mario player, the tiny red box
 *
 * Mario is given an initial random velocity and applied a constant gravity. You
 * can tweak the energy loss against the box walls with the variable restitution
 * (here : 100% = 1.0)
 *
 * When Mario hits a platform coming downward (he's above the platform), he
 * bounces back with some energy loss (variable platformBounceRestitution), when
 * he hits the platform under it, he's just rejected. This is a very simple and
 * naive physics engine because I don't even test if Mario hits the platform
 * borders here. But it is fast. ﻿
 *
 * Note that I also test all the platforms what is not performant at all (well,
 * they aren't very numerous). The use of an octree or any preselection (we know
 * where all the obstacles are in the space before the scene starts !) would be
 * better. ﻿
 *
 * @see http://www.babylonjs-playground.com/#PBVEM#17
 */
class BouncingCubeScene : public IRenderableScene {

public:
  BouncingCubeScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _limit{0.f}
      , _restitution{0.f}
      , _platformBounceRestitution{0.f}
      , _k{0.f}
      , _mario{nullptr}
  {
  }
  ~BouncingCubeScene() override = default;

  const char* getName() override
  {
    return "Bouncing Cube Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor = Color3::Black();
    auto camera       = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(0.f, 10.f, -500.f));
    camera->attachControl(canvas, true);

    auto pl       = PointLight::New("pl", Vector3::Zero(), scene);
    pl->diffuse   = Color3(1.f, 1.f, 1.f);
    pl->intensity = 1.f;
    pl->position  = camera->position;

    auto boxSize               = 300.f;
    auto marioSize             = 5.f;
    auto platformNb            = 25;
    auto platformHeight        = 3.f;
    auto platformMaxWidth      = 40.f;
    auto platformMaxDepth      = 20.f;
    auto platformMinSize       = 20.f;
    _gravity                   = Vector3(0.f, -0.2f, 0.f);
    _platformBounceRestitution = 0.97f;
    auto speed                 = 2.f;
    _restitution               = 0.95f;
    _limit                     = (boxSize - marioSize) * 0.5f;
    auto platformLimit         = boxSize - platformMaxWidth - 10.f;
    _velocity                  = Vector3(Math::random(), Math::random(), Math::random());
    _velocity.scaleInPlace(speed);
    auto initialVel = Vector3(0.5f, 2.f, 0.f);
    _velocity.addInPlace(initialVel);

    BoxOptions boxOptions;
    boxOptions.size            = boxSize;
    boxOptions.sideOrientation = Mesh::BACKSIDE;
    auto box                   = MeshBuilder::CreateBox("b", boxOptions, scene);
    auto boxmat                = StandardMaterial::New("boxmat", scene);
    boxmat->diffuseColor       = Color3(0.4f, 0.6f, 1.f);
    boxmat->specularColor      = Color3::Black();
    box->material              = boxmat;

    BoxOptions marioOptions;
    marioOptions.size       = marioSize;
    _mario                  = MeshBuilder::CreateBox("m", marioOptions, scene);
    auto marioMat           = StandardMaterial::New("marioMat", scene);
    marioMat->diffuseColor  = Color3::Red();
    marioMat->specularColor = Color3::Black();
    _mario->material        = marioMat;

    BoxOptions platformOptions;
    platformOptions.size       = 1.f;
    auto platform              = MeshBuilder::CreateBox("p", platformOptions, scene);
    auto platformMat           = StandardMaterial::New("platform0", scene);
    platformMat->diffuseColor  = Color3::Green();
    platformMat->specularColor = Color3::Black();
    platformMat->alpha         = 0.7f;
    platform->material         = platformMat;
    platform->position
      = Vector3((0.5f - Math::random()) * platformLimit, (0.5f - Math::random()) * platformLimit,
                (0.5f - Math::random()) * platformLimit);
    platform->scaling = Vector3(platformMinSize + platformMaxWidth * Math::random(), platformHeight,
                                platformMinSize + Math::random() * platformMaxDepth);

    _platforms.emplace_back(platform);
    for (int i = 1; i < platformNb; ++i) {
      auto instance = platform->createInstance("platform" + std::to_string(i));
      instance->position
        = Vector3((0.5f - Math::random()) * platformLimit, (0.5f - Math::random()) * platformLimit,
                  (0.5f - Math::random()) * platformLimit);
      platform->scaling
        = Vector3(platformMinSize + platformMaxWidth * Math::random(), platformHeight,
                  platformMinSize + Math::random() * platformMaxDepth);
      _platforms.emplace_back(instance);
    }

    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      _applyForces();
      _checkCollisions();
      _moveMario();
      _movePlatforms();
      _k += 0.001f;
    });
  }

private:
  void _applyForces();
  void _checkCollisions();
  void _moveMario();
  void _movePlatforms();

private:
  Vector3 _gravity;
  Vector3 _velocity;
  float _limit;
  float _restitution;
  float _platformBounceRestitution;
  float _k;
  MeshPtr _mario;
  std::vector<AbstractMeshPtr> _platforms;

}; // end of class BouncingCubeScene

} // end of namespace Samples
} // end of namespace BABYLON

namespace BABYLON {
namespace Samples {

void BouncingCubeScene::_applyForces()
{
  _velocity.addInPlace(_gravity);
}

void BouncingCubeScene::_checkCollisions()
{
  // Check box walls
  if (std::abs(_mario->position().x) > _limit) {
    _velocity.x *= -1.f;
    _mario->position().x = (_mario->position().x > 0.f) ? _limit : -_limit;
  }
  if (std::abs(_mario->position().y) > _limit) {
    _velocity.y *= -1.f * _restitution;
    _mario->position().y = (_mario->position().y > 0.f) ? _limit : -_limit;
  }
  if (std::abs(_mario->position().z) > _limit) {
    _velocity.z *= -1.f;
    _mario->position().z = (_mario->position().z > 0.f) ? _limit : -_limit;
  }
  // Check platforms
  size_t i = 0;
  for (auto& platform : _platforms) {
    if (_mario->intersectsMesh(*platform)) {
      // naive impact point check
      if (_mario->position().y > platform->position().y) {
        _mario->position().y = platform->position().y + platform->scaling().y * 0.5f;
        _velocity.y *= -_platformBounceRestitution;
      }
      if (_mario->position().y < platform->position().y) {
        _velocity.y *= -1.f;
      }
    }
    ++i;
  }
}

void BouncingCubeScene::_moveMario()
{
  _mario->position().addInPlace(_velocity);
}

void BouncingCubeScene::_movePlatforms()
{
  size_t i = 0;
  for (auto& platform : _platforms) {
    auto p = i % 3;
    switch (p) {
      case 0:
        platform->position().x += 0.1f * std::sin(_k * i);
        break;
      case 1:
        platform->position().y -= 0.2f * std::cos(_k * i);
        break;
      case 2:
        platform->position().z -= 0.03f * std::sin(_k * i);
        break;
    }
    ++i;
  }
}

BABYLON_REGISTER_SAMPLE("Animations", BouncingCubeScene)

} // end of namespace Samples
} // end of namespace BABYLON
