#ifndef BABYLON_ANIMATIONS_EASING_CUBIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_CUBIC_EASE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/easing_function.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubicEase : public EasingFunction {

public:
  template <typename... Ts>
  static CubicEasePtr New(Ts&&... args)
  {
    return shared_ptr_t<CubicEase>(new CubicEase(::std::forward<Ts>(args)...));
  }
  ~CubicEase() override;

  float easeInCore(float gradient) const override;

protected:
  CubicEase();

}; // end of class CubicEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_CUBIC_EASE_H
