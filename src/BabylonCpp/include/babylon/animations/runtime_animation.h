#ifndef BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H
#define BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RuntimeAnimation {

public:
  RuntimeAnimation(IAnimatable* target, Animation* animation, Scene* scene,
                   Animatable* host);
  ~RuntimeAnimation();

  Animation* animation();
  void reset();
  bool isStopped() const;
  void dispose();
  void setValue(const AnimationValue& currentValue, bool blend = false);
  void goToFrame(int frame);
  void _prepareForSpeedRatioChange(float newSpeedRatio);
  bool animate(millisecond_t delay, int from, int to, bool loop,
               float speedRatio, float weight = -1.f);

private:
  AnimationValue _getKeyValue(const AnimationValue& value) const;
  AnimationValue
  _interpolate(int currentFrame, int repeatCount, unsigned int loopMode,
               const AnimationValue& offsetValue    = AnimationValue(),
               const AnimationValue& highLimitValue = AnimationValue());

public:
  int currentFrame;

private:
  Animation* _animation;
  IAnimatable* _target;

  float _originalBlendValue;
  std::map<string_t, AnimationValue> _offsetsCache;
  std::map<string_t, AnimationValue> _highLimitsCache;
  bool _stopped;
  float _blendingFactor;

  float _ratioOffset;
  millisecond_t _previousDelay;
  float _previousRatio;

}; // end of class RuntimeAnimation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H
