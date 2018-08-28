#ifndef BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ElasticEase : public EasingFunction {

public:
  template <typename... Ts>
  static ElasticEasePtr New(Ts&&... args)
  {
    return shared_ptr_t<ElasticEase>(
      new ElasticEase(::std::forward<Ts>(args)...));
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
