#ifndef BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H
#define BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BounceEase : public EasingFunction {

public:
  template <typename... Ts>
  static BounceEasePtr New(Ts&&... args)
  {
    return shared_ptr_t<BounceEase>(
      new BounceEase(::std::forward<Ts>(args)...));
  }
  ~BounceEase() override;

  float easeInCore(float gradient) const override;

protected:
  BounceEase(int bounces = 3, float bounciness = 2.f);

public:
  float bounces;
  float bounciness;

}; // end of class BounceEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BOUNCE_EASE_H
