#ifndef BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class QuinticEase;
using QuinticEasePtr = std::shared_ptr<QuinticEase>;

class BABYLON_SHARED_EXPORT QuinticEase : public EasingFunction {

public:
  template <typename... Ts>
  static QuinticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<QuinticEase>(
      new QuinticEase(std::forward<Ts>(args)...));
  }
  ~QuinticEase() override;

  float easeInCore(float gradient) const override;

protected:
  QuinticEase();

}; // end of class QuinticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUINTIC_EASE_H
