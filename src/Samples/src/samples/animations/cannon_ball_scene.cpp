#include <babylon/samples/animations/cannon_ball_scene.h>

#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

CannonBallScene::CannonBallScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _camera{nullptr}
    , _pl{nullptr}
    , _cannon{nullptr}
    , _ball{nullptr}
    , _altitudeZero{-12.f}
    , _radius{3.f}
    , _ballRadius{2.5f}
    , _dt{1.f / 60.f}
    , _t{0.f}
    , _doBugglingAnimation{false}
    , _start{0.f}
    , _startBuggling{1.f} // seconds after initial start
    , _startFiring{2.f}   // idem
    , _startBall{2.2f}    // idem
    , _x0{10.f}           // initial ball fire position x
    , _y0{10.f}           // initial ball fire position y
    , _vx{40.f}           // x speed ball
    , _vy{30.f}           // y speed ball
    , _g{9.81f}           // gravity
    , _loss{0.8f}         // bounce energy loss
    , _speedFactor{5.f}
    , _buggleFactor{2.f}
    , _radiusBuggled{0.f} // internal reminder, don't modify
    , _timeBall{0.f}      // internal fly timer, don't modify
    , _bounce{0.f}        // bounce counter
{
}

CannonBallScene::~CannonBallScene()
{
}

const char* CannonBallScene::getName()
{
  return "Cannon Ball Scene";
}

void CannonBallScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change the scene clear color
  scene->clearColor = Color3(0.5f, 0.5f, 0.5f);

  // Camera
  _camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
  _camera->setPosition(Vector3(-160.f, 100.f, -100.f));
  _camera->attachControl(canvas, true);

  // Lights
  auto light = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
  light->intensity = 0.7f;
  _pl              = PointLight::New("pl", Vector3::Zero(), scene);
  _pl->diffuse     = Color3(1.f, 1.f, 1.f);
  _pl->specular    = Color3(0.5f, 0.5f, 0.f);
  _pl->intensity   = 0.95f;

  // Materials
  auto mat              = StandardMaterial::New("mat1", scene);
  mat->alpha            = 1.f;
  mat->diffuseColor     = Color3(0.5f, 0.5f, 1.f);
  mat->backFaceCulling  = false;
  auto ballmat          = StandardMaterial::New("mat2", scene);
  ballmat->alpha        = 1.f;
  ballmat->diffuseColor = Color3(1.f, 0.5f, 0.5f);

  // Cannon
  auto radius = 3.f;
  for (float i = 0; i < 20; ++i) {
    auto x = i - 10.f;
    auto y = i / 2.f;
    auto z = 0.f;
    _axis.emplace_back(Vector3(x, y, z));
  }

  // Cannon and ball
  auto ground          = Mesh::CreateGround("ground", 1000, 80, 4, scene);
  ground->position().y = _altitudeZero;
  ground->position().x = 450.f;
  _cannon
    = Mesh::CreateTube("cannon", _axis, radius, 20, nullptr, 0, scene, true);
  _cannon->material   = mat;
  _ball               = Mesh::CreateSphere("ball", 10, _ballRadius * 2, scene);
  _ball->material     = ballmat;
  _ball->position().y = 5.f;

  // Animation
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    if (_t < _startBuggling) {
      _nothing(_t);
    }
    else if (_t < _startFiring) {
      if (_doBugglingAnimation) {
        _buggling(_t);
      }
    }
    else if (_t < _startBall) {
      _timeBall        = _startBall;
      _ball->isVisible = true;
      if (_doBugglingAnimation) {
        _firing(_t);
      }
    }
    else {
      _flying(_t);
    }

    _t += _dt;
    _pl->position = _camera->position;
  });
}

void CannonBallScene::_nothing(float /*t*/)
{
}

void CannonBallScene::_buggling(float t)
{
  for (auto& axisItem : _axis) {
    axisItem.x = axisItem.x * 0.999f;
  }
  const auto radiusFunction
    = [this, t](unsigned int i, float /*distance*/) -> float {
    auto bug = Math::PI * i / 19.f;
    auto rad = _radius + (t - _startBuggling) * std::sin(bug) * _buggleFactor;
    _radiusBuggled = rad;
    return rad;
  };
  _cannon = Mesh::CreateTube("", _axis, 0.f, 0, radiusFunction, 0, nullptr,
                             false, 0, _cannon);
}

void CannonBallScene::_firing(float t)
{
  for (auto& axisItem : _axis) {
    axisItem.x = axisItem.x * 1.05f;
  }
  const auto radiusFunction
    = [this, t](unsigned int i, float /*distance*/) -> float {
    auto bug = Math::PI * i / 19.f;
    auto rad = _radiusBuggled - (t - _startFiring) * std::sin(bug);
    return rad;
  };
  _cannon = Mesh::CreateTube("", _axis, 0.f, 0, radiusFunction, 0, nullptr,
                             false, 0, _cannon);
}

void CannonBallScene::_flying(float t)
{
  auto k = (t - _timeBall) * _speedFactor;    // flying time
  auto x = _vx * k + _x0;                     // x = vx * t + x0
  auto y = -k * k * _g / 2.f + k * _vy + _y0; // y = - g * t² / 2 + vy * t + y0

  if (_ball->position().y < _altitudeZero + _ballRadius) {
    ++_bounce;
    _vx       = _vx * std::pow(_loss, _bounce);
    _vy       = _g * k - _vy; // v(t) = -( -g * t + vy) : bouncing == opposite
    _vy       = _vy * std::pow(_loss, _bounce);
    _x0       = x;
    _y0       = _altitudeZero + _ballRadius;
    y         = _y0;
    _timeBall = t;
  }
  _ball->position().x = x;
  _ball->position().y = y;
}

BABYLON_REGISTER_SAMPLE("Animations", CannonBallScene)
} // end of namespace Samples
} // end of namespace BABYLON
