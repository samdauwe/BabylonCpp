#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/babylon_common.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/core/time.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {

class StandardMaterial;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Samples {

/**
 * @brief Animations 101 scene. Example demonstrating how to add animations.
 * @see https://www.babylonjs-playground.com/#QYFDDP#1
 * @see https://doc.babylonjs.com/babylon101/animations
 */
struct Animations101Scene : public IRenderableScene {

public:
  Animations101Scene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _previousTimestamp{Time::highresTimepointNow()}
      , _materialBox2{nullptr}
  {
  }

  ~Animations101Scene() override = default;

  const char* getName() override
  {
    return "Animations 101 Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a light
    auto light       = PointLight::New("Omni", Vector3(0.f, 100.f, 100.f), scene);
    light->intensity = 1.f;

    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Boxes
    // - Box 1
    auto box1          = Mesh::CreateBox("Box1", 10.f, scene);
    box1->position().x = -20.f;
    // - Box 2
    auto box2 = Mesh::CreateBox("Box2", 10.f, scene);

    auto materialBox          = StandardMaterial::New("texture1", scene);
    materialBox->diffuseColor = Color3::Green();
    _materialBox2             = StandardMaterial::New("texture2", scene);

    // Applying materials
    box1->material = materialBox;
    box2->material = _materialBox2;

    // Positioning box
    box2->position().x = 20.f;

    // Creation of a scaling animation with box 1
    //-------------------------------------------
    // Create a scaling animation at 30 FPS
    auto animationBox
      = Animation::New("tutoAnimation", "scaling.x", 30, Animation::ANIMATIONTYPE_FLOAT,
                       Animation::ANIMATIONLOOPMODE_CYCLE);

    // Here we have chosen a loop mode, but you can change to :
    //  Use previous values and increment it
    //  (BABYLON.Animation.ANIMATIONLOOPMODE_RELATIVE) Restart from initial value
    //  (BABYLON.Animation.ANIMATIONLOOPMODE_CYCLE) Keep the final value
    //  (BABYLON.Animation.ANIMATIONLOOPMODE_CONSTANT)

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
    animationBox->setKeys(keysBox1);

    // Then add the animation object to box1
    box1->animations.emplace_back(animationBox);

    // Finally, launch animations on box1, from key 0 to key 100 with loop
    // activated
    scene->beginAnimation(box1, 0.f, 100.f, true);

    // Creation of a manual animation with box 2
    //------------------------------------------
    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      auto currentTimestamp = Time::highresTimepointNow();
      auto diffMillis = Time::fpTimeDiff<size_t, std::milli>(_previousTimestamp, currentTimestamp);
      if (diffMillis >= 1000ul) {
        _previousTimestamp = currentTimestamp;
        // The color is defined at run time with random()
        _materialBox2->diffuseColor = Color3(Math::random(), Math::random(), Math::random());
      }
    });
  }

private:
  high_res_time_point_t _previousTimestamp;
  StandardMaterialPtr _materialBox2;

}; // end of struct Animations101Scene

BABYLON_REGISTER_SAMPLE("Animations", Animations101Scene)

} // end of namespace Samples
} // end of namespace BABYLON
