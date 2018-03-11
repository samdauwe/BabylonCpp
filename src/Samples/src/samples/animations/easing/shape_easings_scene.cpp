#include <babylon/samples/animations/easing/shape_easings_scene.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/easing/cubic_ease.h>
#include <babylon/animations/easing/quadratic_ease.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ShapeEasingsScene::ShapeEasingsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ShapeEasingsScene::~ShapeEasingsScene()
{
}

const char* ShapeEasingsScene::getName()
{
  return "Shape Easings Scene";
}

void ShapeEasingsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  _camera = ArcRotateCamera::New("camera1", -Math::PI_2, 1.2f, 25,
                                 Vector3(0, 1, 0), scene);
  _camera->setPosition(Vector3(0.f, 15.f, -30.f));
  _camera->attachControl(canvas, true);

  // Create a light
  auto light       = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  light->intensity = 0.7f;

  // Create 4 boxes
  auto box1 = Mesh::CreateBox("box1", 2, scene);
  box1->setMaterial(StandardMaterial::New("b1mat", scene));
  box1->material()->setDiffuseColor(Color3(1, 0, 0));
  box1->setPosition(Vector3(-5, 1, 5));

  auto box2 = Mesh::CreateBox("box2", 2, scene);
  box2->setMaterial(StandardMaterial::New("b2mat", scene));
  box2->material()->setDiffuseColor(Color3(1, 0, 0));
  box2->setPosition(Vector3(5, 1, 5));

  auto box3 = Mesh::CreateBox("box3", 2, scene);
  box3->setMaterial(StandardMaterial::New("b3mat", scene));
  box3->material()->setDiffuseColor(Color3(1, 0, 0));
  box3->setPosition(Vector3(-5, 1, -5));

  auto box4 = Mesh::CreateBox("box4", 2, scene);
  box4->setMaterial(StandardMaterial::New("b4mat", scene));
  box4->material()->setDiffuseColor(Color3(1, 0, 0));
  box4->setPosition(Vector3(5, 1, -5));

  // Create 4 cylinders
  auto shape5 = Mesh::CreateCylinder("cyl5", 3.f, 3.f, 0.f, 6, 1, scene, false);
  shape5->setMaterial(StandardMaterial::New("c5mat", scene));
  shape5->material()->setDiffuseColor(Color3(0, 1, 1));
  shape5->setPosition(Vector3(-1.75f, 1.f, 1.75f));

  auto shape6 = Mesh::CreateCylinder("cyl6", 3.f, 3.f, 0.f, 6, 1, scene, false);
  shape6->setMaterial(StandardMaterial::New("c6mat", scene));
  shape6->material()->setDiffuseColor(Color3(1, 0, 1));
  shape6->setPosition(Vector3(1.75f, 1.f, 1.75f));

  auto shape7 = Mesh::CreateCylinder("cyl7", 3.f, 3.f, 0.f, 6, 1, scene, false);
  shape7->setMaterial(StandardMaterial::New("c7mat", scene));
  shape7->material()->setDiffuseColor(Color3(0.3f, 0.3f, 1));
  shape7->setPosition(Vector3(-1.75f, 1.f, -1.75f));

  auto shape8 = Mesh::CreateCylinder("cyl8", 3.f, 3.f, 0.f, 6, 1, scene, false);
  shape8->setMaterial(StandardMaterial::New("c8mat", scene));
  shape8->material()->setDiffuseColor(Color3(0.9f, 0.9f, 0.f));
  shape8->setPosition(Vector3(1.75f, 1.f, -1.75f));

  // Create the ground mesh
  auto ground = Mesh::CreateGround("ground1", 20, 20, 2, scene);
  ground->setMaterial(StandardMaterial::New("gmat", scene));
  ground->material()->setDiffuseColor(Color3(0.1f, 0.2f, 0.f));

  // Create 6 animations keys
  std::vector<IAnimationKey> keys1{
    IAnimationKey(1000, AnimationValue(3.f)),
    IAnimationKey(1060, AnimationValue(1.f)),
    IAnimationKey(1120, AnimationValue(3.f)),
  };

  std::vector<IAnimationKey> keys2{
    IAnimationKey(1000, AnimationValue(-Math::PI / 8.f)),
    IAnimationKey(1060, AnimationValue(Math::PI / 8.f)),
    IAnimationKey(1120, AnimationValue(-Math::PI / 8.f)),
  };

  std::vector<IAnimationKey> keys3{
    IAnimationKey(1000, AnimationValue(Math::PI)),
    IAnimationKey(1060, AnimationValue(-Math::PI)),
    IAnimationKey(1120, AnimationValue(Math::PI)),
  };

  std::vector<IAnimationKey> keys4{
    IAnimationKey(1000, AnimationValue(Color3(0.f, 0.f, 1.f))),
    IAnimationKey(1030, AnimationValue(Color3(0.f, 1.f, 0.f))),
    IAnimationKey(1060, AnimationValue(Color3(1.f, 0.f, 0.f))),
    IAnimationKey(1090, AnimationValue(Color3(1.f, 0.f, 1.f))),
    IAnimationKey(1120, AnimationValue(Color3(0.f, 0.f, 1.f))),
  };

  std::vector<IAnimationKey> keys5{
    IAnimationKey(1000, AnimationValue(1.f)),
    IAnimationKey(1060, AnimationValue(-1.f)),
    IAnimationKey(1120, AnimationValue(1.f)),
  };

  std::vector<IAnimationKey> keys6{
    IAnimationKey(1000, AnimationValue(-Math::PI / 4.f)),
    IAnimationKey(1060, AnimationValue(Math::PI / 4.f)),
    IAnimationKey(1120, AnimationValue(-Math::PI / 4.f)),
  };

  // Create 8 animations
  auto a1
    = new Animation("at", "position.y", 30, Animation::ANIMATIONTYPE_FLOAT(),
                    Animation::ANIMATIONLOOPMODE_CYCLE());
  auto a2
    = new Animation("a2", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT(),
                    Animation::ANIMATIONLOOPMODE_CYCLE());
  auto a3
    = new Animation("a3", "rotation.x", 30, Animation::ANIMATIONTYPE_FLOAT(),
                    Animation::ANIMATIONLOOPMODE_CYCLE());
  auto a4 = new Animation("a4", "material.diffuseColor", 30,
                          Animation::ANIMATIONTYPE_COLOR3(),
                          Animation::ANIMATIONLOOPMODE_CYCLE());
  auto a5
    = new Animation("a5", "rotation.z", 30, Animation::ANIMATIONTYPE_FLOAT(),
                    Animation::ANIMATIONLOOPMODE_CYCLE());
  auto a6
    = new Animation("a6", "rotation.y", 30, Animation::ANIMATIONTYPE_FLOAT(),
                    Animation::ANIMATIONLOOPMODE_CYCLE());

  // Set the animation keys
  a1->setKeys(keys1);
  a2->setKeys(keys2);
  a3->setKeys(keys3);
  a4->setKeys(keys4);
  a5->setKeys(keys5);
  a6->setKeys(keys6);

  // Add the animations to mesh.animations array
  box1->animations.emplace_back(a1);
  box2->animations.emplace_back(a2);
  box3->animations.emplace_back(a3);
  box4->animations.emplace_back(a4);
  shape5->animations.emplace_back(a5);
  shape6->animations.emplace_back(a6);

  /** Animation start for a0 - a6 **/

  // -- Make an Animatable --
  // params: (scene, target, fromFrame, toFrame, loopAnimation, speedRatio,
  // onAnimationEnd, animations)
  auto aa1 = new Animatable(scene, box1, 1000, 1120, 1, 1.f, nullptr, {a1});
  auto aa2 = new Animatable(scene, box2, 1000, 1120, 1, 1.f, nullptr, {a2});
  // Start the animatables
  aa1->_animate(millisecond_t(1));
  aa2->_animate(millisecond_t(1));

  // -- Start animations using Scene.beginAnimation --
  // params: (target, from, to, loop, speedRatio, onAnimationEnd, animatable)
  scene->beginAnimation(box3, 1000, 1120, true, 1.f, nullptr);
  scene->beginAnimation(shape5, 1000, 1120, true, 1.f, nullptr);

  // -- Start animations using Scene.beginDirectAnimation --
  // params: (target, animations, from, to, loop, speedRatio, onAnimationEnd)
  scene->beginDirectAnimation(box4, {a4}, 1000, 1120, true, 1.f, nullptr);
  scene->beginDirectAnimation(shape6, {a6}, 1000, 1120, true, 1.f, nullptr);

  /** a7a - a7b - easing animations on shape 7 **/

  // Create animations at7 and at7b
  auto a7a = new Animation("shape7EasingAnimation", "position", 30,
                           Animation::ANIMATIONTYPE_VECTOR3(),
                           Animation::ANIMATIONLOOPMODE_CYCLE());

  auto a7b = new Animation("shape7EasingAnimationb", "rotation", 30,
                           Animation::ANIMATIONTYPE_VECTOR3(),
                           Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  std::vector<IAnimationKey> keys7{
    IAnimationKey(0, AnimationValue(shape7->position())),
    IAnimationKey(40, AnimationValue(box1->position())),
    IAnimationKey(80, AnimationValue(box4->position())),
    IAnimationKey(230, AnimationValue(shape7->position())),
  };

  std::vector<IAnimationKey> keys7b{
    IAnimationKey(0, AnimationValue(Vector3(0, 0, 0))),
    IAnimationKey(40, AnimationValue(Vector3(0, -Math::PI2, 0))),
    IAnimationKey(80, AnimationValue(Vector3(0, Math::PI2, 0))),
    IAnimationKey(230, AnimationValue(Vector3(0, 0, 0))),
  };

  // Set the animation keys
  a7a->setKeys(keys7);
  a7b->setKeys(keys7b);

  // Create the easing functions
  auto myease1 = new CubicEase();
  auto myease2 = new QuadraticEase();

  // For each easing function, you can choose beetween EASEIN (default),
  // EASEOUT, EASEINOUT
  myease1->setEasingMode(EasingFunction::EASINGMODE_EASEINOUT);
  myease2->setEasingMode(EasingFunction::EASINGMODE_EASEINOUT);

  // Adding the easing functions to the animations
  a7a->setEasingFunction(myease1);
  a7b->setEasingFunction(myease2);

  // Adding the animations to the shape7 animations collection
  shape7->animations.emplace_back(a7a);
  shape7->animations.emplace_back(a7b);

  // Finally, start all animations on shape7, from key 0 to key 120 with loop
  // true
  scene->beginAnimation(shape7, 0, 120, true);

  // create the camera animation
  scene->onBeforeCameraRenderObservable.add([this](Camera*, EventState&) {
    // _camera->alpha += 0.01f * getScene()->getAnimationRatio();
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
