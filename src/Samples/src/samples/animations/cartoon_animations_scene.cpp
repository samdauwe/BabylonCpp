#include <babylon/samples/animations/cartoon_animations_scene.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

CartoonAnimationsScene::CartoonAnimationsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CartoonAnimationsScene::~CartoonAnimationsScene()
{
}

const char* CartoonAnimationsScene::getName()
{
  return "Cartoon Animations Scene";
}

void CartoonAnimationsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto light1
    = DirectionalLight::New("DirectionalLight", Vector3(0, -1, 0), scene);
  auto light2 = HemisphericLight::New("HemiLight", Vector3(0, 1, -1), scene);
  light1->intensity = 0.25f;
  light2->intensity = 0.5f;

  size_t frameRate = 20;

  /********* Performers *********/

  // Camera
  auto camera = TargetCamera::New("TargetCamera", Vector3(0, 3, -30), scene);
  camera->attachControl(canvas, false);

  // Door
  BoxOptions doorOptions;
  doorOptions.width  = 2.f;
  doorOptions.height = 4.f;
  doorOptions.depth  = 0.1f;
  auto door          = MeshBuilder::CreateBox("door", doorOptions, scene);

  BoxOptions hingeOptions;
  auto hinge       = MeshBuilder::CreateBox("hinge", hingeOptions, scene);
  hinge->isVisible = false;
  door->setParent(hinge);
  hinge->position().y = 2.f;
  door->position().x  = -1.f;

  // Lights
  SphereOptions spereOptions(0.2f);
  auto sphereLight = MeshBuilder::CreateSphere("sphere", spereOptions, scene);
  sphereLight->material = StandardMaterial::New("", scene);
  sphereLight->material()->setEmissiveColor(Color3(1, 1, 1));
  sphereLight->position().x = 2.f;
  sphereLight->position().y = 3.f;
  sphereLight->position().z = 0.1f;

  vector_t<Mesh*> sphereLights{sphereLight};
  Float32Array lightPositions{-2.f, 3.f, 6.9f};

  for (unsigned int i = 0; i < 1; ++i) {
    sphereLights.emplace_back(sphereLight->clone(""));
    sphereLights[i + 1]->position
      = Vector3(lightPositions[3 * i], lightPositions[3 * i + 1],
                lightPositions[3 * i + 2]);
  }

  vector_t<SpotLight*> spotLights(sphereLights.size());
  Float32Array lightDirections{-0.5f, -0.25f, 1.f, 0.f, 0.f, -1.f};
  for (unsigned int i = 0; i < sphereLights.size(); ++i) {
    auto iStr     = ::std::to_string(i);
    spotLights[i] = SpotLight::New(
      "spotlight" + iStr, sphereLights[i]->position(),
      Vector3(lightDirections[3 * i], lightDirections[3 * i + 1],
              lightDirections[3 * i + 2]),
      Math::PI / 8.f, 5, scene);
    spotLights[i]->diffuse   = Color3(1, 1, 1);
    spotLights[i]->specular  = Color3(0.5f, 0.5f, 0.5f);
    spotLights[i]->intensity = 0.f;
  }

  /********* Animations *************/

  // -- For camera to sweep round --
  auto rotate = new Animation("rotate", "rotation.y", frameRate,
                              Animation::ANIMATIONTYPE_FLOAT(),
                              Animation::ANIMATIONLOOPMODE_CONSTANT());

  // Create rotate animation keys
  auto frameRateInt = static_cast<int>(frameRate);
  std::vector<IAnimationKey> rotateKeys{
    IAnimationKey(0, AnimationValue(0.f)),
    IAnimationKey(9 * frameRateInt, AnimationValue(0.f)),
    IAnimationKey(14 * frameRateInt, AnimationValue(Math::PI)),
  };

  // Set rotate animation keys
  rotate->setKeys(rotateKeys);

  // -- For camera move forward --
  auto movein = new Animation("movein", "position", frameRate,
                              Animation::ANIMATIONTYPE_VECTOR3(),
                              Animation::ANIMATIONLOOPMODE_CONSTANT());

  // Create movein animation keys
  std::vector<IAnimationKey> moveinKeys{
    IAnimationKey(0, AnimationValue(Vector3(0, 5, -30))),
    IAnimationKey(3, AnimationValue(Vector3(0, 2, -10))),
    IAnimationKey(5, AnimationValue(Vector3(0, 2, -10))),
    IAnimationKey(8, AnimationValue(Vector3(-2, 2, 3))),
  };

  // Set movein animation keys
  movein->setKeys(moveinKeys);

  // -- For door to open and close --
  auto sweep = new Animation("sweep", "rotation.y", frameRate,
                             Animation::ANIMATIONTYPE_FLOAT(),
                             Animation::ANIMATIONLOOPMODE_CONSTANT());

  // Create sweep animation keys
  std::vector<IAnimationKey> sweepKeys{
    IAnimationKey(0, AnimationValue(0)),
    IAnimationKey(3 * frameRateInt, AnimationValue(0.f)),
    IAnimationKey(5 * frameRateInt, AnimationValue(Math::PI / 3.f)),
    IAnimationKey(13 * frameRateInt, AnimationValue(Math::PI / 3.f)),
    IAnimationKey(15 * frameRateInt, AnimationValue(0.f)),
  };

  // Set sweep animation keys
  sweep->setKeys(sweepKeys);

  // -- For light to brighten and dim --
  auto lightDimmer = new Animation("dimmer", "intensity", frameRate,
                                   Animation::ANIMATIONTYPE_FLOAT(),
                                   Animation::ANIMATIONLOOPMODE_CONSTANT());

  // Create light dimmer animation keys
  std::vector<IAnimationKey> lightKeys{
    IAnimationKey(0, AnimationValue(0)),
    IAnimationKey(7 * frameRateInt, AnimationValue(0.f)),
    IAnimationKey(10 * frameRateInt, AnimationValue(1.f)),
    IAnimationKey(14 * frameRateInt, AnimationValue(1.f)),
    IAnimationKey(15 * frameRateInt, AnimationValue(0.f)),
  };

  // Set light dimmer animation keys
  lightDimmer->setKeys(lightKeys);

  /******* Run Clips **********/
  scene->beginDirectAnimation(camera, {movein, rotate}, 0, 25 * frameRateInt,
                              false);
  scene->beginDirectAnimation(hinge, {sweep}, 0, 25 * frameRateInt, false);
  scene->beginDirectAnimation(spotLights[0], {lightDimmer}, 0,
                              25 * frameRateInt, false);
  scene->beginDirectAnimation(spotLights[1], {lightDimmer /*->clone()*/}, 0,
                              25 * frameRateInt, false);

  /************** Peripherals of Scene ***************/
  GroundOptions groundOptions;
  groundOptions.width  = 50;
  groundOptions.height = 50;
  MeshBuilder::CreateGround("ground", groundOptions, scene);

  BoxOptions wall1Options;
  wall1Options.width  = 8.f;
  wall1Options.height = 6.f;
  wall1Options.depth  = 0.1f;
  auto wall1          = MeshBuilder::CreateBox("door", wall1Options, scene);
  wall1->position().x = -6.f;
  wall1->position().y = 3.f;

  BoxOptions wall2Options;
  wall2Options.width  = 4.f;
  wall2Options.height = 6.f;
  wall2Options.depth  = 0.1f;
  auto wall2          = MeshBuilder::CreateBox("door", wall2Options, scene);
  wall2->position().x = 2.f;
  wall2->position().y = 3.f;

  BoxOptions wall3Options;
  wall3Options.width  = 2.f;
  wall3Options.height = 2.f;
  wall3Options.depth  = 0.1f;
  auto wall3          = MeshBuilder::CreateBox("door", wall3Options, scene);
  wall3->position().x = -1.f;
  wall3->position().y = 5.f;

  BoxOptions wall4Options;
  wall4Options.width  = 14.f;
  wall4Options.height = 6.f;
  wall4Options.depth  = 0.1f;
  auto wall4          = MeshBuilder::CreateBox("door", wall4Options, scene);
  wall4->position().x = -3.f;
  wall4->position().y = 3.f;
  wall4->position().z = 7.f;

  BoxOptions wall5Options;
  wall5Options.width  = 7.f;
  wall5Options.height = 6.f;
  wall5Options.depth  = 0.1f;
  auto wall5          = MeshBuilder::CreateBox("door", wall5Options, scene);
  wall5->rotation().y = Math::PI_2;
  wall5->position().x = -10.f;
  wall5->position().y = 3.f;
  wall5->position().z = 3.5f;

  BoxOptions wall6Options;
  wall6Options.width  = 7.f;
  wall6Options.height = 6.f;
  wall6Options.depth  = 0.1f;
  auto wall6          = MeshBuilder::CreateBox("door", wall6Options, scene);
  wall6->rotation().y = Math::PI_2;
  wall6->position().x = 4.f;
  wall6->position().y = 3.f;
  wall6->position().z = 3.5f;

  BoxOptions roofOptions;
  roofOptions.width  = 14.f;
  roofOptions.height = 7.f;
  roofOptions.depth  = 0.1f;
  auto roof          = MeshBuilder::CreateBox("door", roofOptions, scene);
  roof->rotation().x = Math::PI_2;
  roof->position().x = -3.f;
  roof->position().y = 6.f;
  roof->position().z = 3.5f;
}

} // end of namespace Samples
} // end of namespace BABYLON
