#ifndef BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
#define BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class CircleEase;
using CircleEasePtr = std::shared_ptr<CircleEase>;

class BABYLON_SHARED_EXPORT CircleEase : public EasingFunction {

public:
  template <typename... Ts>
  static CircleEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<CircleEase>(
      new CircleEase(std::forward<Ts>(args)...));
  }
  ~CircleEase() override;

  float easeInCore(float gradient) const override;

protected:
  CircleEase();

}; // end of class CircleEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_CIRCLE_EASE_H
