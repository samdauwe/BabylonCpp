#ifndef BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H
#define BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H

#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EasingFunction : public IEasingFunction {

public:
  // Statics
  static constexpr unsigned int EASINGMODE_EASEIN    = 0;
  static constexpr unsigned int EASINGMODE_EASEOUT   = 1;
  static constexpr unsigned int EASINGMODE_EASEINOUT = 2;

public:
  EasingFunction();
  virtual ~EasingFunction();

  /** Properties **/
  void setEasingMode(unsigned int easingMode);
  unsigned int getEasingMode() const;
  virtual float easeInCore(float gradient) const = 0;
  float ease(float gradient) override;

private:
  unsigned int _easingMode;

}; // end of class EasingFunction

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_EASING_FUNCTION_H
