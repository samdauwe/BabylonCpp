#ifndef BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class ElasticEase;
using ElasticEasePtr = std::shared_ptr<ElasticEase>;

class BABYLON_SHARED_EXPORT ElasticEase : public EasingFunction {

public:
  template <typename... Ts>
  static ElasticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<ElasticEase>(
      new ElasticEase(std::forward<Ts>(args)...));
  }
  ~ElasticEase() override;

  float easeInCore(float gradient) const override;

protected:
  ElasticEase(int oscillations = 3, int springiness = 3);

public:
  float oscillations;
  float springiness;

}; // end of class ElasticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
