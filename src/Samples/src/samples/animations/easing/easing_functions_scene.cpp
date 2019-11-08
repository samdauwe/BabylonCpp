#include <babylon/samples/samples_index.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/easing/bezier_curve_ease.h>
#include <babylon/animations/easing/circle_ease.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {
namespace Samples {

/**
 * @brief Easing Functions Scene. Example demonstarting how to create animations
 * with different easing functions.
 * @see https://www.babylonjs-playground.com/#8ZNVGR#0
 * @see https://doc.babylonjs.com/babylon101/animations#easing-functions
 */
struct EasingFunctionsScene : public IRenderableScene {

  EasingFunctionsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~EasingFunctionsScene()
  {
  }

  const char* getName() override
  {
    return "Easing Functions Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a light
    PointLight::New("Omni", Vector3(0.f, 100.f, 100.f), scene);

    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // ------------------------------------------------------------------
    // Creation of an easing animation within predefined easing functions
    // ------------------------------------------------------------------
    // Torus
    auto torus          = Mesh::CreateTorus("torus", 8, 2, 32, scene, false);
    torus->position().x = 25.f;
    torus->position().z = 30.f;

    // Create a Vector3 animation at 30 FPS
    auto animationTorus
      = Animation::New("torusEasingAnimation", "position", 30, Animation::ANIMATIONTYPE_VECTOR3(),
                       Animation::ANIMATIONLOOPMODE_CYCLE());

    // the torus destination position
    auto nextPos = torus->position().add(Vector3(-80.f, 0.f, 0.f));

    // Animation keys
    std::vector<IAnimationKey> keysTorus{
      IAnimationKey(0, AnimationValue(torus->position())),
      IAnimationKey(120, AnimationValue(nextPos)),
    };
    animationTorus->setKeys(keysTorus);

    // Adding an easing function
    // You can use :
    // 1.  CircleEase()
    // 2.  BackEase(amplitude)
    // 3.  BounceEase(bounces, bounciness)
    // 4.  CubicEase()
    // 5.  ElasticEase(oscillations, springiness)
    // 6.  ExponentialEase(exponent)
    // 7.  PowerEase(power)
    // 8.  QuadraticEase()
    // 9.  QuarticEase()
    // 10. QuinticEase()
    // 11. SineEase()
    // And if you want a total control, you can use a Bezier Curve animation
    // 12.   BezierCurveEase(x1, y1, x2, y2)
    auto easingFunction = CircleEase::New();

    // For each easing function, you can choose beetween EASEIN (default),
    // EASEOUT, EASEINOUT
    easingFunction->setEasingMode(EasingFunction::EASINGMODE_EASEINOUT);

    // Adding easing function to my animation
    animationTorus->setEasingFunction(easingFunction);

    // Adding animation to my torus animations collection
    torus->animations.emplace_back(animationTorus);

    // Finally, launch animations on torus, from key 0 to key 120 with loop
    // activated
    scene->beginAnimation(torus, 0, 120, true);

    // -------------------------------------------------------------------
    // Using Bezier curve to create a custom easing function
    // See here to see some samples and values : http://cubic-bezier.com/
    // -------------------------------------------------------------------
    // Torus
    auto bezierTorus          = Mesh::CreateTorus("torus", 8, 2, 32, scene, false);
    bezierTorus->position().x = 25.f;
    bezierTorus->position().z = 0.f;

    // Create the animation
    auto animationBezierTorus
      = Animation::New("animationBezierTorus", "position", 30, Animation::ANIMATIONTYPE_VECTOR3(),
                       Animation::ANIMATIONLOOPMODE_CYCLE());
    std::vector<IAnimationKey> keysBezierTorus{
      IAnimationKey(0, AnimationValue(bezierTorus->position())),
      IAnimationKey(120, AnimationValue(bezierTorus->position().add(Vector3(-80, 0, 0))))};
    animationBezierTorus->setKeys(keysBezierTorus);
    auto bezierEase = BezierCurveEase::New(0.32f, -0.73f, 0.69f, 1.59f);
    animationBezierTorus->setEasingFunction(bezierEase);
    bezierTorus->animations.emplace_back(animationBezierTorus);
    scene->beginAnimation(bezierTorus, 0, 120, true);

    // ------------------------------------------
    // Create a simple animation without easing functions
    // ------------------------------------------
    auto torus0                  = Mesh::CreateTorus("torus", 8, 2, 32, scene, false);
    torus0->position().x         = 25.f;
    torus0->position().z         = -30.f;
    auto torus0Material          = StandardMaterial::New("texture1", scene);
    torus0Material->diffuseColor = Color3(0.f, 1.f, 0.f); // Green
    torus0->material             = torus0Material;

    Animation::CreateAndStartAnimation("anim", torus0, "position", 30, 120,
                                       AnimationValue(torus0->position()),
                                       AnimationValue(torus0->position().add(Vector3(-80, 0, 0))));
  }

}; // end of struct EasingFunctionsScene



BABYLON_REGISTER_SAMPLE("Animations", EasingFunctionsScene)
} // end of namespace Samples
} // end of namespace BABYLON
