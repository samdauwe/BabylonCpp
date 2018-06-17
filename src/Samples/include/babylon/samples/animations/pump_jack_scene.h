#ifndef BABYLON_SAMPLES_ANIMATIONS_PUMP_JACK_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_PUMP_JACK_SCENE_H

#include <babylon/animations/easing/easing_function.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class FunnyEase : public EasingFunction {

public:
  FunnyEase(float distanceOfStick, float lengthOfStick);
  ~FunnyEase() override;

  float easeInCore(float gradient) const override;

private:
  float _distanceOfStick;
  float _lengthOfStick;

}; // end of class FunnyEase

struct PumpJackScene : public IRenderableScene {

public:
  PumpJackScene(ICanvas* iCanvas);
  ~PumpJackScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PumpJackScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_PUMP_JACK_SCENE_H
