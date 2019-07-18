#include <babylon/samples/animations/pick_and_play_animation.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {
namespace Samples {

PickAndPlayAnimationScene::PickAndPlayAnimationScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _t{0.f}
    , _mat0{nullptr}
    , _mat1{nullptr}
    , _mat2{nullptr}
    , _mesh0{nullptr}
    , _animYEnabled{true}
    , _animY{nullptr}
    , _anim0{nullptr}
    , _animatableColor{nullptr}
    , _pickedMesh{nullptr}
{
}

PickAndPlayAnimationScene::~PickAndPlayAnimationScene()
{
}

const char* PickAndPlayAnimationScene::getName()
{
  return "Pick And Play Animation Scene";
}

void PickAndPlayAnimationScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color4(0.f, 0.f, 0.f, 1.f);
  scene->autoClear  = true;

  // materials
  _mat0            = PBRMetallicRoughnessMaterial::New("mat0", scene);
  _mat0->metallic  = 0.f;
  _mat0->roughness = 1.f;
  _mat0->baseColor = Color3(0.f, 0.2f, 0.9f);
  _mat0->alpha     = 0.9f;

  _mat1            = PBRMetallicRoughnessMaterial::New("mat1", scene);
  _mat1->metallic  = 0.f;
  _mat1->roughness = 1.f;
  _mat1->baseColor = Color3(0.5f, 0.f, 0.f);

  _mat2                = PBRMetallicRoughnessMaterial::New("mat2", scene);
  _mat2->metallic      = 0.f;
  _mat2->roughness     = 1.f;
  _mat2->emissiveColor = Color3(0.1f, 0.6f, 0.1f);
  _mat2->baseColor     = Color3(0.f, 0.f, 0.f);
  _mat2->animations    = {};

  //
  // meshes
  //

  _mesh0 = TransformNode::New("mesh0", scene);

  GroundOptions groundOptions;
  groundOptions.width  = 1000;
  groundOptions.height = 1000;
  auto ground      = MeshBuilder::CreateGround("ground", groundOptions, scene);
  ground->material = _mat0;

  auto N    = 16u;
  auto size = 1.f;
  for (auto i = 1u; i <= N; ++i) {
    TorusKnotOptions torusKnotOptions;
    torusKnotOptions.radius = size;
    const auto istr         = std::to_string(i);
    auto mesh
      = MeshBuilder::CreateTorusKnot("mesh" + istr, torusKnotOptions, scene);
    mesh->parent       = _mesh0.get();
    auto angle         = i / N * 2.f * Math::PI;
    mesh->position().x = 5.f * std::sin(angle);
    mesh->position().z = 5.f * std::cos(angle);
    _mesh0Offsets.emplace_back(static_cast<float>(i) / N);
    mesh->material = _mat1;
    auto anim      = Animation::CreateAndStartAnimation(
      "anim" + istr, mesh, "rotation.y", 30, 180.f, 0.f, -2.f * Math::PI,
      Animation::ANIMATIONLOOPMODE_CYCLE());
  }

  //
  // animations
  //

  // kind #1 ---- complex-animation
  _animY = [this](Scene*, EventState&) {
    if (_animYEnabled) {
      auto i = 0u;
      for (const auto& mesh : _mesh0->getChildMeshes()) {
        mesh->position().y
          = 3.f * std::sin((_mesh0Offsets[i] + _t) * 2.f * Math::PI);
        ++i;
      }
      _t += 0.001f;
    }
  };
  scene->registerAfterRender(_animY);

  // kind #2 ---- helper-function -> Animatable{}
  _anim0 = Animation::CreateAndStartAnimation(
    "anim0", _mesh0, "rotation.y", 10 /*fps*/, 180.f /*totalFrame*/, 0.f,
    2.f * Math::PI, Animation::ANIMATIONLOOPMODE_CYCLE());

  // kind #3 ---- Animation{}
  auto animColor
    = Animation::New("anim", "baseColor", 60, Animation::ANIMATIONTYPE_COLOR3(),
                     Animation::ANIMATIONLOOPMODE_CYCLE());
  std::vector<IAnimationKey> colorAnimation{
    // yoyo
    IAnimationKey(0, AnimationValue(Color3(1, 0, 0))),
    IAnimationKey(45, AnimationValue(Color3(0, 1, 0))),
    IAnimationKey(90, AnimationValue(Color3(0, 0, 1))),
    IAnimationKey(135, AnimationValue(Color3(0, 1, 0))),
    IAnimationKey(180, AnimationValue(Color3(1, 0, 0)))};
  animColor->setKeys(colorAnimation);
  _mat2->animations.emplace_back(animColor);
  _animatableColor = scene->beginAnimation(_mat2, 0.f, 180.f, true);
  _animatableColor->pause();

  //
  // light and shadow
  //

  auto light0       = SpotLight::New("light0", Vector3(10.f, 20.f, 0.f),
                               Vector3(-1.f, -2.f, 0.f), Math::PI_2, 1, scene);
  light0->intensity = 1500.f;
  auto light1       = SpotLight::New("light1", Vector3(-10.f, 20.f, 0.f),
                               Vector3(1.f, -2.f, 0.f), Math::PI_2, 1, scene);
  light1->intensity = 1500.f;
  auto sg0          = ShadowGenerator::New(std::pow(2, 11), light0);
  auto sg1          = ShadowGenerator::New(std::pow(2, 11), light1);
  sg0->useCloseExponentialShadowMap = true;
  sg0->bias                         = -0.005f;
  sg1->useCloseExponentialShadowMap = true;
  sg1->bias                         = -0.005f;
  for (const auto& mesh : _mesh0->getChildMeshes()) {
    sg0->addShadowCaster(mesh);
    sg1->addShadowCaster(mesh);
    mesh->receiveShadows = true;
  }
  ground->receiveShadows = true;

  //
  // camera
  //

  auto camera0 = ArcRotateCamera::New("camera0", Math::PI / 180.f * -140.f,
                                      Math::PI / 180.f * 90.f, 12.f,
                                      _mesh0->position(), scene);
  camera0->attachControl(canvas);
  camera0->upperBetaLimit   = 85.f * (Math::PI / 180.f);
  camera0->lowerRadiusLimit = 10.f;
  camera0->upperRadiusLimit = 20.f;
  camera0->wheelPrecision   = 100.f;

  //
  // events
  //
  scene->registerBeforeRender([this](Scene*, EventState&) {
    auto result = _scene->pick(_scene->pointerX, _scene->pointerY);
    if (result && result->hit) {
      // if (mesh0.getChildren().includes(result.pickedMesh)) {
      if (String::startsWith(result->pickedMesh->name, "mesh")) {
        _anim0->pause();
        _animYEnabled = false;
        // _scene->unregisterAfterRender(_animY);
        _pickedMesh = result->pickedMesh;
        for (const auto& mesh : _mesh0->getChildren()) {
          _scene->getAnimatableByTarget(mesh)->pause();
          std::static_pointer_cast<Mesh>(mesh)->material = _mat1;
        }
        _pickedMesh->material = _mat2;
        _animatableColor->restart();
        return;
      }
    }
    // IF not hovering boxes
    _anim0->restart(); // ... kind #2
    if (_pickedMesh) {
      _animYEnabled = true;
      // _scene->registerAfterRender(_animY); // ... kind #1
      _pickedMesh->material = _mat1;
      _animatableColor.reset();
      _pickedMesh = nullptr;
    }
    for (const auto& mesh : _mesh0->getChildren()) {
      _scene->getAnimatableByTarget(mesh)->restart();
    }
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
