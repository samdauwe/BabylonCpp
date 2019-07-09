#include <babylon/samples/animations/animations_scene.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

AnimationsScene::AnimationsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

AnimationsScene::~AnimationsScene()
{
}

const char* AnimationsScene::getName()
{
  return "Animations Scene";
}

void AnimationsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a light
  auto light       = PointLight::New("Omni", Vector3(0.f, 100.f, 100.f), scene);
  light->intensity = 1.f;

  // Create a camera
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Boxes
  // - Box 1
  auto box1          = Mesh::CreateBox("Box1", 10.f, scene);
  box1->position().x = -20.f;
  // - Box 2
  BoxOptions options;
  options.size          = 10.f;
  options.faceColors[0] = Color4(1.f, 0.f, 0.f, 1.f);   // Red
  options.faceColors[5] = Color4(1.f, 1.f, 0.f, 1.f);   // Yellow
  options.faceColors[1] = Color4(0.f, 1.f, 0.f, 1.f);   // Green
  options.faceColors[4] = Color4(0.f, 0.f, 1.f, 1.f);   // Blue
  options.faceColors[2] = Color4(1.f, 0.f, 1.f, 1.f);   // Magenta
  options.faceColors[3] = Color4(0.5f, 0.f, 0.5f, 1.f); // Purple
  auto box2             = MeshBuilder::CreateBox("Box2", options, scene);
  // - Box 3
  auto box3                 = MeshBuilder::CreateBox("Box1", options, scene);
  box3->position().x        = -20.f;
  box3->position().z        = -30.f;
  auto materialBox          = StandardMaterial::New("texture1", scene);
  materialBox->diffuseColor = Color3::Green();
  auto materialBox2         = StandardMaterial::New("texture2", scene);

  // Applying materials
  box1->material = materialBox;
  box2->material = materialBox2;

  // Positioning box
  box2->position().x = 20.f;

  // Creation of a scaling animation with box 1
  //-------------------------------------------
  // Create a scaling animation at 30 FPS
  auto animationBox1 = Animation::New("scalingAnimation", "scaling.x", 30,
                                      Animation::ANIMATIONTYPE_FLOAT(),
                                      Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  std::vector<IAnimationKey> keysBox1{
    // At the animation key 0, the value of scaling is "1"
    IAnimationKey(0, AnimationValue(1.f)),
    // At the animation key 20, the value of scaling is "0.2"
    IAnimationKey(20, AnimationValue(0.2f)),
    // At the animation key 100, the value of scaling is "1"
    IAnimationKey(100, AnimationValue(1.f)),
  };

  // Adding keys to the animation object
  animationBox1->setKeys(keysBox1);

  // Then add the animation object to box1
  box1->animations.emplace_back(animationBox1);

  // Finally, launch animations on box1, from key 0 to key 100 with loop
  // activated
  scene->beginAnimation(box1, 0, 100, true);

  // Creation of a rotation animation with box 2
  //--------------------------------------------
  // Create a scaling animation at 30 FPS
  auto animationBox2RotX = Animation::New("rotationAnimation", "rotation.x", 30,
                                          Animation::ANIMATIONTYPE_FLOAT(),
                                          Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  std::vector<IAnimationKey> keysRotation{
    IAnimationKey(0, AnimationValue(0.f)),
    IAnimationKey(20, AnimationValue(Math::PI)),
    IAnimationKey(100, AnimationValue(Math::PI2)),
  };

  // Adding keys to the animation object
  animationBox2RotX->setKeys(keysRotation);

  // Then add the animation object to box1
  box2->animations.emplace_back(animationBox2RotX);

  // Finally, launch animations on box2, from key 0 to key 100 with loop
  // activated
  scene->beginAnimation(box2, 0, 100, true);

  // Creation of a rotation animation with box 3
  //--------------------------------------------
  // Create a scaling animation at 30 FPS
  auto animationBox3RotX = Animation::New("rotationAnimation", "rotation.x", 30,
                                          Animation::ANIMATIONTYPE_FLOAT(),
                                          Animation::ANIMATIONLOOPMODE_CYCLE());
  auto animationBox3RotY = Animation::New("rotationAnimation", "rotation.y", 30,
                                          Animation::ANIMATIONTYPE_FLOAT(),
                                          Animation::ANIMATIONLOOPMODE_CYCLE());
  auto animationBox3RotZ = Animation::New("rotationAnimation", "rotation.z", 30,
                                          Animation::ANIMATIONTYPE_FLOAT(),
                                          Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  keysRotation = {
    IAnimationKey(0, AnimationValue(0.f)),
    IAnimationKey(25, AnimationValue(Math::PI_2)),
    IAnimationKey(50, AnimationValue(Math::PI)),
    IAnimationKey(100, AnimationValue(Math::PI2)),
  };

  // Adding keys to the animation object
  animationBox3RotX->setKeys(keysRotation);
  animationBox3RotY->setKeys(keysRotation);
  animationBox3RotZ->setKeys(keysRotation);

  // Then add the animation object to box1
  box3->animations.emplace_back(animationBox3RotX);
  box3->animations.emplace_back(animationBox3RotY);
  box3->animations.emplace_back(animationBox3RotZ);

  // Finally, launch animations on box2, from key 0 to key 100 with loop
  // activated
  scene->beginAnimation(box3, 0, 100, true);
}

} // end of namespace Samples
} // end of namespace BABYLON
