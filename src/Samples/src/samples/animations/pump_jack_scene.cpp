#include <babylon/samples/animations/pump_jack_scene.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/easing/bezier_curve_ease.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

FunnyEase::FunnyEase(float distanceOfStick, float lengthOfStick)
    : EasingFunction{}
    , _distanceOfStick{distanceOfStick}
    , _lengthOfStick{lengthOfStick}
{
}

FunnyEase::~FunnyEase()
{
}

float FunnyEase::easeInCore(float gradient) const
{
  float angle = gradient * Math::PI2;
  angle += std::asin(std::sin(angle) * _distanceOfStick / _lengthOfStick);
  gradient = angle / (Math::PI2);
  return gradient;
}

PumpJackScene::PumpJackScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

PumpJackScene::~PumpJackScene()
{
}

const char* PumpJackScene::getName()
{
  return "Pump Jack Scene";
}

void PumpJackScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Configuration
  int animationFrames        = 200;
  float lengthOfStick        = 6.f;
  float lengthOfTopStick     = 10.f;
  float lengthOfPumpStick    = lengthOfStick + 5.f;
  float distanceOfStick      = 1.f;
  unsigned int moveDirection = 1;

  // Create custom easing function
  auto myEase = new FunnyEase(distanceOfStick, lengthOfStick);

  // Initialization
  auto camera = FreeCamera::New("camera1", Vector3(10.f, 10.f, -20.f), scene);
  camera->setTarget(Vector3::Zero());
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("Hemi0", Vector3(0.f, 1.f, 0.f), scene);
  light->diffuse     = Color3(1.f, 1.f, 1.f);
  light->specular    = Color3(1.f, 1.f, 1.f);
  light->groundColor = Color3(0.f, 0.f, 0.f);
  light->intensity   = 0.7f;

  //#################//
  // Main motor axis //
  //#################//

  auto parentObject = Mesh::CreateBox("parent", 0.1f, scene);

  auto motorAxis
    = Mesh::CreateCylinder("motorAxis", 5, 0.5f, 0.5f, 24, 1, scene, true, 0);
  motorAxis->rotation().x = Math::PI_2;
  motorAxis->setParent(parentObject);

  auto motorAnimation
    = new Animation("anim", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT,
                    Animation::ANIMATIONLOOPMODE_CYCLE);

  // Animation keys
  std::vector<AnimationKey> motorAnimationKeys{
    AnimationKey(0, AnimationValue(0.f)),
    AnimationKey(animationFrames,
                 AnimationValue(Math::PI2 * (moveDirection ? -1.f : 1.f))),
  };

  motorAnimation->setKeys(motorAnimationKeys);
  parentObject->animations.emplace_back(motorAnimation);
  scene->beginAnimation(parentObject, 0, animationFrames, true);

  //###########//
  // Front Arm //
  //###########//

  auto frontArmPaddle = Mesh::CreateBox("frontArmPaddle", 2.f, scene);
  frontArmPaddle->setParent(parentObject);
  frontArmPaddle->scaling().y  = 2.f;
  frontArmPaddle->scaling().z  = 0.1f;
  frontArmPaddle->scaling().x  = 0.5f;
  frontArmPaddle->position().y = 1.5f;
  frontArmPaddle->position().z = -2.3f;

  auto frontWeight = Mesh::CreateBox("frontWeight", 1.f, scene);
  frontWeight->setParent(parentObject);
  frontWeight->position().y = 3.f;
  frontWeight->position().z = -2.3f;
  frontWeight->setScaling(Vector3(3.f, 1.6f, 0.3f));

  auto frontPivot = Mesh::CreateBox("frontPivot", 0.5f, scene);
  frontPivot->setParent(parentObject);
  frontPivot->position().y = distanceOfStick;
  frontPivot->position().z = -2.3f;

  auto frontStick = Mesh::CreateBox("frontStick", 1.f, scene);
  frontStick->setParent(frontPivot);
  frontStick->position().x = -(lengthOfStick / 2.f);
  frontStick->position().z = -0.3f;
  frontStick->setScaling(Vector3(lengthOfStick, 0.2f, 0.2f));

  auto frontStickAnimation
    = new Animation("anim", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT,
                    Animation::ANIMATIONLOOPMODE_CYCLE);

  // Animation keys
  std::vector<AnimationKey> frontStickAnimationKeys{
    AnimationKey(
      0, AnimationValue(moveDirection ? -2.5f * Math::PI : -Math::PI_2)),
    AnimationKey(
      animationFrames,
      AnimationValue(moveDirection ? -Math::PI_2 : -2.5f * Math::PI)),
  };

  frontStickAnimation->setKeys(frontStickAnimationKeys);
  frontStickAnimation->setEasingFunction(myEase);
  frontPivot->animations.emplace_back(frontStickAnimation);
  scene->beginAnimation(frontPivot, 0, animationFrames, true);

  //###########//
  // Back Arm //
  //###########//

  auto backArmPaddle = Mesh::CreateBox("backArmPaddle", 2.f, scene);
  backArmPaddle->setParent(parentObject);
  backArmPaddle->scaling().y  = 2.f;
  backArmPaddle->scaling().z  = 0.1f;
  backArmPaddle->scaling().x  = 0.5f;
  backArmPaddle->position().y = 1.5f;
  backArmPaddle->position().z = 2.3f;

  auto backWeight = Mesh::CreateBox("backWeight", 1.f, scene);
  backWeight->setParent(parentObject);
  backWeight->position().y = 3.f;
  backWeight->position().z = 2.3f;
  backWeight->setScaling(Vector3(3.f, 1.6f, 0.3f));

  auto backPivot = Mesh::CreateBox("backPivot", 0.5f, scene);
  backPivot->setParent(parentObject);
  backPivot->position().y = distanceOfStick;
  backPivot->position().z = 2.3f;

  auto backStick = Mesh::CreateBox("backStick", 1.f, scene);
  backStick->setParent(backPivot);
  backStick->position().x = -(lengthOfStick / 2.f);
  backStick->position().z = 0.3f;
  backStick->setScaling(Vector3(lengthOfStick, 0.2f, 0.2f));

  auto backStickAnimation
    = new Animation("anim", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT,
                    Animation::ANIMATIONLOOPMODE_CYCLE);

  // Animation keys
  std::vector<AnimationKey> backStickAnimationKeys{
    AnimationKey(
      0, AnimationValue(moveDirection ? -2.5f * Math::PI : -Math::PI_2)),
    AnimationKey(
      animationFrames,
      AnimationValue(moveDirection ? -Math::PI_2 : -2.5f * Math::PI)),
  };

  backStickAnimation->setKeys(backStickAnimationKeys);
  backStickAnimation->setEasingFunction(myEase);
  backPivot->animations.emplace_back(backStickAnimation);
  scene->beginAnimation(backPivot, 0, animationFrames, true);

  //############//
  // Top Pieces //
  //############//

  auto topStick = Mesh::CreateBox("topStick", 1, scene);
  topStick->setScaling(Vector3(lengthOfTopStick, 0.2f, 0.2f));
  topStick->position().x = -lengthOfTopStick / 2.f;
  topStick->position().y = lengthOfStick;

  auto topStickAnimation
    = new Animation("anim", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT,
                    Animation::ANIMATIONLOOPMODE_CYCLE);

  float moveAngle = std::asin((2.f * distanceOfStick) / lengthOfTopStick);

  // Animation keys
  std::vector<AnimationKey> topStickAnimationKeys{
    AnimationKey(0, AnimationValue(moveAngle)),
    AnimationKey(animationFrames / 2, AnimationValue(-moveAngle)),
    AnimationKey(animationFrames, AnimationValue(moveAngle)),
  };

  topStickAnimation->setKeys(topStickAnimationKeys);
  topStickAnimation->setEasingFunction(
    new BezierCurveEase(0.35f, 0.1f, 0.65f, 0.9f));
  topStick->animations.emplace_back(topStickAnimation);
  scene->beginAnimation(topStick, 0, animationFrames, true);

  auto topBar
    = Mesh::CreateCylinder("topBar", 5.5f, 0.7f, 0.7f, 24, 1, scene, true, 0);
  topBar->setParent(topStick);
  topBar->position().x = 0.5f;
  topBar->setScaling(Vector3(1.f / lengthOfTopStick, 5.f, 5.f));
  topBar->rotation().x = Math::PI_2;

  auto topBall = Mesh::CreateSphere("topBall", 12, 1, scene, true, 0);
  topBall->setParent(topStick);
  topBall->position().x = -0.5f;
  topBall->setScaling(Vector3(1.f / lengthOfTopStick, 5.f, 5.f));

  //############//
  // Pump Stick //
  //############//

  auto pumpStick          = Mesh::CreateBox("pumpStick", 1.f, scene);
  pumpStick->position().x = -lengthOfTopStick;
  pumpStick->setScaling(Vector3(0.2f, lengthOfPumpStick, 0.2f));

  auto pumpStickAnimation
    = new Animation("anim", "position.y", 30, Animation::ANIMATIONTYPE_FLOAT,
                    Animation::ANIMATIONLOOPMODE_CYCLE);

  // Animation keys
  std::vector<AnimationKey> pumpStickAnimationKeys{
    AnimationKey(0, AnimationValue(lengthOfStick - (lengthOfPumpStick / 2.f)
                                   - distanceOfStick)),
    AnimationKey(animationFrames / 2,
                 AnimationValue(lengthOfStick - (lengthOfPumpStick / 2.f)
                                + distanceOfStick)),
    AnimationKey(animationFrames,
                 AnimationValue(lengthOfStick - (lengthOfPumpStick / 2.f)
                                - distanceOfStick)),
  };

  pumpStickAnimation->setKeys(pumpStickAnimationKeys);
  pumpStickAnimation->setEasingFunction(
    new BezierCurveEase(0.35f, 0.1f, 0.65f, 0.9f));
  pumpStick->animations.emplace_back(pumpStickAnimation);
  scene->beginAnimation(pumpStick, 0, animationFrames, true);

  //####################//
  // Visual Complements //
  //####################//

  auto ground          = Mesh::CreateBox("ground", 1.f, scene);
  ground->position().x = -5.f;
  ground->position().y = -5.f;
  ground->scaling().x  = 20.f;
  ground->scaling().z  = 10.f;

  auto groundEntrance
    = Mesh::CreateCylinder("groundEntrance", 3, 1, 1, 12, 1, scene, true, 0);
  groundEntrance->position().x = -lengthOfTopStick;
  groundEntrance->position().y = ground->position().y + 1.5f;

  auto centerPillarFront = Mesh::CreateBox("centerPillarFront", 1.f, scene);
  centerPillarFront->position().x = -lengthOfTopStick / 2.f;
  centerPillarFront->position().z = -1.f;
  centerPillarFront->scaling().y  = 11.f;
  centerPillarFront->rotation().x = 0.16f;

  auto centerPillarBack = Mesh::CreateBox("centerPillarBack", 1.f, scene);
  centerPillarBack->position().x = -lengthOfTopStick / 2.f;
  centerPillarBack->position().z = 1.f;
  centerPillarBack->scaling().y  = 11.f;
  centerPillarBack->rotation().x = -0.16f;

  auto topAxis
    = Mesh::CreateCylinder("topAxis", 2, 1, 1, 12, 1, scene, true, 0);
  topAxis->position().x = -lengthOfTopStick / 2.f;
  topAxis->position().y = lengthOfStick - 0.6f;
  topAxis->rotation().x = Math::PI_2;

  auto engine2         = Mesh::CreateBox("engine2", 1.f, scene);
  engine2->scaling().x = 3.f;
  engine2->scaling().y = 2.f;
  engine2->scaling().z = 2.f;

  auto support          = Mesh::CreateBox("support", 1.f, scene);
  support->scaling().y  = -ground->position().x;
  support->scaling().x  = 0.7f;
  support->scaling().z  = 0.7f;
  support->position().y = ground->position().x / 2.f;
  support->position().z = -1.f;

  auto support2          = Mesh::CreateBox("support2", 1.f, scene);
  support2->scaling().y  = -ground->position().x;
  support2->scaling().x  = 0.7f;
  support2->scaling().z  = 0.7f;
  support2->position().y = ground->position().x / 2.f;
  support2->position().z = 1.f;

  auto support3          = Mesh::CreateBox("support3", 1.f, scene);
  support3->scaling().y  = -ground->position().x + 1.f;
  support3->scaling().x  = 0.7f;
  support3->scaling().z  = 0.7f;
  support3->position().x = 2.f;
  support3->position().y = ground->position().x / 2.f;
  support3->position().z = 1.f;
  support3->rotation().z = 0.7f;

  auto support4          = Mesh::CreateBox("support4", 1.f, scene);
  support4->scaling().y  = -ground->position().x + 1.f;
  support4->scaling().x  = 0.7f;
  support4->scaling().z  = 0.7f;
  support4->position().x = 2.f;
  support4->position().y = ground->position().x / 2.f;
  support4->position().z = -1.f;
  support4->rotation().z = 0.7f;

  auto support5          = Mesh::CreateBox("support5", 1.f, scene);
  support5->scaling().y  = -ground->position().x + 1.f;
  support5->scaling().x  = 0.7f;
  support5->scaling().z  = 0.7f;
  support5->position().x = -2.f;
  support5->position().y = ground->position().x / 2.f;
  support5->position().z = 1.f;
  support5->rotation().z = -0.7f;

  auto support6          = Mesh::CreateBox("support6", 1.f, scene);
  support6->scaling().y  = -ground->position().x + 1.f;
  support6->scaling().x  = 0.7f;
  support6->scaling().z  = 0.7f;
  support6->position().x = -2.f;
  support6->position().y = ground->position().x / 2.f;
  support6->position().z = -1.f;
  support6->rotation().z = -0.7f;

  auto support7          = Mesh::CreateBox("support7", 1.f, scene);
  support7->position().x = -lengthOfTopStick / 4.f - 0.5f;
  support7->position().y = 2.f;
  support7->scaling().y  = 8.f;
  support7->scaling().x  = 0.7f;
  support7->scaling().z  = 0.7f;
  support7->rotation().z = 0.5f;
}

} // end of namespace Samples
} // end of namespace BABYLON
