#include <babylon/samples/samples_index.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/time.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/mesh.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {

class MorphTarget;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Samples {

/**
 * @brief Morph Mesh Scene. Demo to show how to morph meshes with BabylonJS.
 * @see https://github.com/kekkorider/babylonjs-mesh-morph
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class MorphMeshScene : public IRenderableScene {

public:
  MorphMeshScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _testMorphTarget{ nullptr }, _currentAnim{ 0 }
  {
  }

  ~MorphMeshScene()
  {
  }

  const char* getName() override
  {
    return "Morph Mesh Scene";
  }

  void initializeScene (ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 10.f,
      Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Lights
    auto hemisphericLight
      = HemisphericLight::New("HemisphericLight", Vector3(1.f, 1.f, 0.f), scene);
    auto pointLight00
      = PointLight::New("pointLight00", Vector3(3.f, 3.f, -3.f), scene);
    pointLight00->diffuse = Color3(0.1f, 0.5f, 0.4f);

    auto pointLight01 = PointLight::New("pointLight01", Vector3(3, 3, -3), scene);
    pointLight01->diffuse = Color3(0.6f, 0.1f, 0.3f);

    SceneLoader::ImportMesh(
      { "Sphere Default", "Sphere Morph" }, "scenes/", "morph.babylon", scene,
      [this, scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
        const std::vector<IParticleSystemPtr>& /*particleSystems*/,
        const std::vector<SkeletonPtr>& /*skeletons*/,
        const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
          auto test = std::static_pointer_cast<Mesh>(
            scene->getMeshByName("Sphere Default"));
          test->convertToFlatShadedMesh();
          test->position = Vector3::Zero();

          auto test01
            = std::static_pointer_cast<Mesh>(scene->getMeshByName("Sphere Morph"));
          test01->convertToFlatShadedMesh();
          test01->setEnabled(false);

          /*
           * Animations
           */
          auto anim = Animation::New("InfluenceAnimation", "influence", 10,
            Animation::ANIMATIONTYPE_FLOAT(),
            Animation::ANIMATIONLOOPMODE_RELATIVE());
          std::vector<IAnimationKey> keyframes{
            IAnimationKey(0, AnimationValue(0.f)),
            IAnimationKey(10, AnimationValue(1.f)),
          };
          anim->setKeys(keyframes);

          _testMorphTarget = MorphTarget::FromMesh(test01, "test01", 0.f);
          _testMorphTarget->animations.emplace_back(anim);

          auto testManager = MorphTargetManager::New(scene);
          testManager->addTarget(_testMorphTarget);
          test->morphTargetManager = testManager;
      });

    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      auto currentTimestamp = Time::highresTimepointNow();
      auto diffMillis = Time::fpTimeDiff<size_t, std::milli>(_previousTimestamp,
        currentTimestamp);
      if (diffMillis >= 5000ul) {
        _previousTimestamp = currentTimestamp;
        _currentAnim++;
        if (_currentAnim == 2) {
          _currentAnim = 0;
        }
        switch (_currentAnim) {
        case 0:
          _scene->beginAnimation(_testMorphTarget, 10, 0);
          break;
        case 1:
          _scene->beginAnimation(_testMorphTarget, 0, 10);
          break;
        default:
          break;
        }
      }
      });
  }

private:
  high_res_time_point_t _previousTimestamp;
  MorphTargetPtr _testMorphTarget;
  int _currentAnim;

}; // end of class MorphMeshScene


BABYLON_REGISTER_SAMPLE("Animations", MorphMeshScene)
} // end of namespace Samples
} // end of namespace BABYLON