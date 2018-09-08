#ifndef BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class QuarticEase;
using QuarticEasePtr = std::shared_ptr<QuarticEase>;

class BABYLON_SHARED_EXPORT QuarticEase : public EasingFunction {

public:
  template <typename... Ts>
  static QuarticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<QuarticEase>(
      new QuarticEase(std::forward<Ts>(args)...));
  }
  ~QuarticEase() override;

  float easeInCore(float gradient) const override;

protected:
  QuarticEase();

}; // end of class QuarticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_QUARTIC_EASE_H
