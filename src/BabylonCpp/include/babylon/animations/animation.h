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

  static bool AllowMatricesInterpolation();

  static Animation*
  _PrepareAnimation(const string_t& name, const string_t& targetProperty,
                    size_t framePerSecond, int totalFrame,
                    const AnimationValue& from, const AnimationValue& to,
                    unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
                    IEasingFunction* easingFunction = nullptr);

  /**
   * @brief Sets up an animation.
   * @param property the property to animate
   * @param animationType the animation type to apply
   * @param easingFunction the easing function used in the animation
   * @returns The created animation
   */
  static Animation* CreateAnimation(const string_t& property, int animationType,
                                    std::size_t framePerSecond,
                                    IEasingFunction* easingFunction = nullptr);

  static Animatable* CreateAndStartAnimation(
    const string_t& name, Node* node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE,
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  static Animatable* CreateMergeAndStartAnimation(
    const string_t& name, Node* node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE,
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Transition property of the Camera to the target Value.
   * @param property The property to transition
   * @param targetValue The target Value of the property
   * @param host The object where the property to animate belongs
   * @param scene Scene used to run the animation
   * @param frameRate Framerate (in frame/s) to use
   * @param transition The transition type we want to use
   * @param duration The duration of the animation, in milliseconds
   * @param onAnimationEnd Call back trigger at the end of the animation.
   */
  static Animatable*
  TransitionTo(const string_t& property, const AnimationValue& targetValue,
               const AnimationValue& host, Scene* scene, float frameRate,
               Animation* transition, float duration,
               const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Return the array of runtime animations currently using this
   * animation.
   */
  vector_t<RuntimeAnimation*>& runtimeAnimations();

  bool hasRunningRuntimeAnimations();

  Animation(const string_t& name, const string_t& targetProperty,
            size_t framePerSecond, int dataType,
            unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE);
  ~Animation();

  /** Methods **/
  /**
   * @param {boolean} fullDetails - support for multiple levels of
   * logging within scene loading
   */
  string_t toString(bool fullDetails = false) const;
  /**
   * Add an event to this animation.
   */
  void addEvent(const AnimationEvent& event);

  /**
   * Remove all events found at the given frame
   * @param frame
   */
  void removeEvents(int frame);
  vector_t<AnimationEvent>& getEvents();
  void createRange(const string_t& name, float from, float to);
  void deleteRange(const string_t& name, bool deleteFrames = true);
  AnimationRange& getRange(const string_t& name);
  void reset();
  bool isStopped() const;
  vector_t<AnimationKey>& getKeys();
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
  void setKeys(const vector_t<AnimationKey>& values);
  unique_ptr_t<Animation> clone() const;
  Json::object serialize() const;
  static Animation* Parse(const Json::value& parsedAnimation);
  static void AppendSerializedAnimations(IAnimatable* source,
                                         picojson::object& destination);

private:
  static bool _AllowMatricesInterpolation;

public:
  string_t name;
  string_t targetProperty;
  vector_t<RuntimeAnimation*> _runtimeAnimations;
  vector_t<string_t> targetPropertyPath;
  size_t framePerSecond;
  int dataType;
  unsigned int loopMode;
  float blendingSpeed;
  bool enableBlending;

private:
  vector_t<AnimationKey> _keys;
  IEasingFunction* _easingFunction;
  // The set of event that will be linked to this animation
  vector_t<AnimationEvent> _events;
  unordered_map_t<string_t, AnimationRange> _ranges;

}; // end of class Animation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_H
