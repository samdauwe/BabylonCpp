#ifndef BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ElasticEase : public EasingFunction {

public:
  ElasticEase(int oscillations = 3, int springiness = 3);
  ~ElasticEase();

  float easeInCore(float gradient) const override;

private:
  float _oscillations;
  float _springiness;

}; // end of class ElasticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
