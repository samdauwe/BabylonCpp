#ifndef BABYLON_ANIMATIONS_EASING_BACK_EASE_H
#define BABYLON_ANIMATIONS_EASING_BACK_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BackEase;
using BackEasePtr = std::shared_ptr<BackEase>;

class BABYLON_SHARED_EXPORT BackEase : public EasingFunction {

public:
  template <typename... Ts>
  static BackEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<BackEase>(new BackEase(std::forward<Ts>(args)...));
  }
  ~BackEase() override;

  float easeInCore(float gradient) const override;

protected:
  BackEase(float amplitude = 1.f);

public:
  float amplitude;

}; // end of class BackEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BACK_EASE_H
