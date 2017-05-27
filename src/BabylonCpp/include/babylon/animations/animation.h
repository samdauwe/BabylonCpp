#ifndef BABYLON_ANIMATIONS_ANIMATION_H
#define BABYLON_ANIMATIONS_ANIMATION_H

#include <babylon/animations/animation_event.h>
#include <babylon/animations/animation_key.h>
#include <babylon/animations/animation_range.h>
#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Animation {

public:
  /** Statics **/
  static constexpr unsigned int ANIMATIONTYPE_FLOAT        = 0;
  static constexpr unsigned int ANIMATIONTYPE_VECTOR3      = 1;
  static constexpr unsigned int ANIMATIONTYPE_QUATERNION   = 2;
  static constexpr unsigned int ANIMATIONTYPE_MATRIX       = 3;
  static constexpr unsigned int ANIMATIONTYPE_COLOR3       = 4;
  static constexpr unsigned int ANIMATIONTYPE_VECTOR2      = 5;
  static constexpr unsigned int ANIMATIONTYPE_SIZE         = 6;
  static constexpr unsigned int ANIMATIONTYPE_BOOL         = 20;
  static constexpr unsigned int ANIMATIONTYPE_INT          = 21;
  static constexpr unsigned int ANIMATIONTYPE_STRING       = 22;
  static constexpr unsigned int ANIMATIONTYPE_COLOR4       = 23;
  static constexpr unsigned int ANIMATIONLOOPMODE_RELATIVE = 0;
  static constexpr unsigned int ANIMATIONLOOPMODE_CYCLE    = 1;
  static constexpr unsigned int ANIMATIONLOOPMODE_CONSTANT = 2;

  static Animation*
  _PrepareAnimation(const std::string& name, const std::string& targetProperty,
                    size_t framePerSecond, int totalFrame,
                    const AnimationValue& from, const AnimationValue& to,
                    unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
                    IEasingFunction* easingFunction = nullptr);
  static Animatable* CreateAndStartAnimation(
    const std::string& name, Node* node, const std::string& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE,
    IEasingFunction* easingFunction = nullptr,
    const std::function<void()>& onAnimationEnd = nullptr);
  static Animatable* CreateMergeAndStartAnimation(
    const std::string& name, Node* node, const std::string& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE,
    IEasingFunction* easingFunction = nullptr,
    const std::function<void()>& onAnimationEnd = nullptr);

  Animation(const std::string& name, const std::string& targetProperty,
            size_t framePerSecond, int dataType,
            unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE);
  ~Animation();

  /** Methods **/
  /**
   * @param {boolean} fullDetails - support for multiple levels of
   * logging within scene loading
   */
  std::string toString(bool fullDetails = false) const;
  /**
   * Add an event to this animation.
   */
  void addEvent(const AnimationEvent& event);

  /**
   * Remove all events found at the given frame
   * @param frame
   */
  void removeEvents(int frame);
  void createRange(const std::string& name, float from, float to);
  void deleteRange(const std::string& name, bool deleteFrames = true);
  AnimationRange& getRange(const std::string& name);
  void reset();
  bool isStopped() const;
  std::vector<AnimationKey>& getKeys();
  int getHighestFrame() const;
  IEasingFunction* getEasingFunction();
  void setEasingFunction(IEasingFunction* easingFunction);
  float floatInterpolateFunction(float startValue, float endValue,
                                 float gradient) const;
  float floatInterpolateFunctionWithTangents(float startValue, float outTangent,
                                             float endValue, float inTangent,
                                             float gradient) const;
  Quaternion quaternionInterpolateFunction(const Quaternion& startValue,
                                           const Quaternion& endValue,
                                           float gradient) const;
  Quaternion quaternionInterpolateFunctionWithTangents(
    const Quaternion& startValue, const Quaternion& outTangent,
    const Quaternion& endValue, const Quaternion& inTangent,
    float gradient) const;
  Vector3 vector3InterpolateFunction(const Vector3& startValue,
                                     const Vector3& endValue,
                                     float gradient) const;
  Vector3 vector3InterpolateFunctionWithTangents(const Vector3& startValue,
                                                 const Vector3& outTangent,
                                                 const Vector3& endValue,
                                                 const Vector3& inTangent,
                                                 float gradient) const;
  Vector2 vector2InterpolateFunction(const Vector2& startValue,
                                     const Vector2& endValue,
                                     float gradient) const;
  Vector2 vector2InterpolateFunctionWithTangents(const Vector2& startValue,
                                                 const Vector2& outTangent,
                                                 const Vector2& endValue,
                                                 const Vector2& inTangent,
                                                 float gradient) const;
  Size sizeInterpolateFunction(const Size& startValue, const Size& endValue,
                               float gradient) const;
  Color3 color3InterpolateFunction(const Color3& startValue,
                                   const Color3& endValue,
                                   float gradient) const;
  Matrix matrixInterpolateFunction(const Matrix& startValue,
                                   const Matrix& endValue,
                                   float gradient) const;
  std::unique_ptr<Animation> clone() const;
  void setKeys(const std::vector<AnimationKey>& values);
  void setValue(const AnimationValue& currentValue, bool blend = false);
  void goToFrame(int frame);
  bool animate(millisecond_t delay, float from, float to, bool loop,
               float speedRatio);
  Json::object serialize() const;
  static Animation* Parse(const Json::value& parsedAnimation);

private:
  AnimationValue _getKeyValue(const AnimationValue& value) const;
  AnimationValue
  _interpolate(int currentFrame, int repeatCount, unsigned int loopMode,
               const AnimationValue& offsetValue    = AnimationValue(),
               const AnimationValue& highLimitValue = AnimationValue());

public:
  IAnimatable* _target;
  std::string name;
  std::string targetProperty;
  std::vector<std::string> targetPropertyPath;
  size_t framePerSecond;
  int dataType;
  unsigned int loopMode;
  int currentFrame;
  bool allowMatricesInterpolation;
  float blendingSpeed;
  float _originalBlendValue;
  bool enableBlending;

private:
  std::vector<AnimationKey> _keys;
  std::map<std::string, AnimationValue> _offsetsCache;
  std::map<std::string, AnimationValue> _highLimitsCache;
  bool _stopped;
  float _blendingFactor;
  IEasingFunction* _easingFunction;
  // The set of event that will be linked to this animation
  std::vector<AnimationEvent> _events;
  std::unordered_map<std::string, AnimationRange> _ranges;

}; // end of class Animation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_H
