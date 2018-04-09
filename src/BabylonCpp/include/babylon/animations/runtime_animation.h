#ifndef BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H
#define BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RuntimeAnimation {

public:
  /**
   * @brief Create a new RuntimeAnimation object.
   * @param target defines the target of the animation
   * @param animation defines the source {BABYLON.Animation} object
   * @param scene defines the hosting scene
   * @param host defines the initiating Animatable
   */
  RuntimeAnimation(IAnimatable* target, Animation* animation, Scene* scene,
                   Animatable* host);
  ~RuntimeAnimation();

  Animation* animation();
  void reset();
  bool isStopped() const;
  void dispose();

  /**
   * @brief Affect the interpolated value to the target.
   * @param currentValue defines the value computed by the animation
   * @param weight defines the weight to apply to this value
   */
  void setValue(const AnimationValue& currentValue, float weight = 1.f);
  Nullable<unsigned int> _getCorrectLoopMode() const;
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
  /**
   * @brief Gets the current frame.
   */
  int get_currentFrame() const;

  /**
   * @brief Gets the weight of the runtime animation.
   */
  float get_weight() const;

  /**
   * @brief Gets the original value of the runtime animation.
   */
  Nullable<AnimationValue>& get_originalValue();

  /**
   * @brief Gets the current value of the runtime animation.
   */
  Nullable<AnimationValue>& get_currentValue();

  /**
   * @brief Gets the path where to store the animated value in the target.
   */
  string_t get_targetPath() const;

  /**
   * @brief Gets the actual target of the runtime animation.
   */
  IAnimatable*& get_target();

public:
  /**
   * Current frame.
   */
  ReadOnlyProperty<RuntimeAnimation, int> currentFrame;

  /**
   * Weight of the runtime animation.
   */
  ReadOnlyProperty<RuntimeAnimation, float> weight;

  /**
   * Original value of the runtime animation.
   */
  ReadOnlyProperty<RuntimeAnimation, Nullable<AnimationValue>> originalValue;

  /**
   * Current value of the runtime animation.
   */
  ReadOnlyProperty<RuntimeAnimation, Nullable<AnimationValue>> currentValue;

  /**
   * Path where to store the animated value in the target.
   */
  ReadOnlyProperty<RuntimeAnimation, string_t> targetPath;

  /**
   * Actual target of the runtime animation.
   */
  ReadOnlyProperty<RuntimeAnimation, IAnimatable*> target;

private:
  int _currentFrame;
  Animation* _animation;
  IAnimatable* _target;
  Animatable* _host;

  Nullable<AnimationValue> _originalValue;
  AnimationValue _originalBlendValue;
  std::map<string_t, AnimationValue> _offsetsCache;
  std::map<string_t, AnimationValue> _highLimitsCache;
  bool _stopped;
  float _blendingFactor;
  Scene* _scene;

  Nullable<AnimationValue> _currentValue;
  IAnimatable* _activeTarget;
  string_t _targetPath;
  float _weight;

  float _ratioOffset;
  millisecond_t _previousDelay;
  float _previousRatio;

}; // end of class RuntimeAnimation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_RUNTIME_ANIMATION_H
