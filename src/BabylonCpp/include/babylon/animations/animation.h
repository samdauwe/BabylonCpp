#ifndef BABYLON_ANIMATIONS_ANIMATION_H
#define BABYLON_ANIMATIONS_ANIMATION_H

#include <babylon/animations/animation_event.h>
#include <babylon/animations/animation_range.h>
#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Animation {

private:
  /** Statics **/
  static constexpr unsigned int _ANIMATIONTYPE_FLOAT        = 0;
  static constexpr unsigned int _ANIMATIONTYPE_VECTOR3      = 1;
  static constexpr unsigned int _ANIMATIONTYPE_QUATERNION   = 2;
  static constexpr unsigned int _ANIMATIONTYPE_MATRIX       = 3;
  static constexpr unsigned int _ANIMATIONTYPE_COLOR3       = 4;
  static constexpr unsigned int _ANIMATIONTYPE_VECTOR2      = 5;
  static constexpr unsigned int _ANIMATIONTYPE_SIZE         = 6;
  static constexpr unsigned int _ANIMATIONTYPE_BOOL         = 20;
  static constexpr unsigned int _ANIMATIONTYPE_INT          = 21;
  static constexpr unsigned int _ANIMATIONTYPE_STRING       = 22;
  static constexpr unsigned int _ANIMATIONTYPE_COLOR4       = 23;
  static constexpr unsigned int _ANIMATIONTYPE_FLOAT32ARRAY = 30;
  static constexpr unsigned int _ANIMATIONLOOPMODE_RELATIVE = 0;
  static constexpr unsigned int _ANIMATIONLOOPMODE_CYCLE    = 1;
  static constexpr unsigned int _ANIMATIONLOOPMODE_CONSTANT = 2;

public:
  static constexpr unsigned int ANIMATIONTYPE_FLOAT()
  {
    return Animation::_ANIMATIONTYPE_FLOAT;
  }

  static constexpr unsigned int ANIMATIONTYPE_VECTOR3()
  {
    return Animation::_ANIMATIONTYPE_VECTOR3;
  }

  static constexpr unsigned int ANIMATIONTYPE_QUATERNION()
  {
    return Animation::_ANIMATIONTYPE_QUATERNION;
  }

  static constexpr unsigned int ANIMATIONTYPE_MATRIX()
  {
    return Animation::_ANIMATIONTYPE_MATRIX;
  }

  static constexpr unsigned int ANIMATIONTYPE_COLOR3()
  {
    return Animation::_ANIMATIONTYPE_COLOR3;
  }

  static constexpr unsigned int ANIMATIONTYPE_VECTOR2()
  {
    return Animation::_ANIMATIONTYPE_VECTOR2;
  }

  static constexpr unsigned int ANIMATIONTYPE_SIZE()
  {
    return Animation::_ANIMATIONTYPE_SIZE;
  }

  static constexpr unsigned int ANIMATIONTYPE_BOOL()
  {
    return Animation::_ANIMATIONTYPE_BOOL;
  }

  static constexpr unsigned int ANIMATIONTYPE_INT()
  {
    return Animation::_ANIMATIONTYPE_INT;
  }

  static constexpr unsigned int ANIMATIONTYPE_STRING()
  {
    return Animation::_ANIMATIONTYPE_STRING;
  }

  static constexpr unsigned int ANIMATIONTYPE_COLOR4()
  {
    return Animation::_ANIMATIONTYPE_COLOR4;
  }

  static constexpr unsigned int ANIMATIONTYPE_FLOAT32ARRAY()
  {
    return Animation::_ANIMATIONTYPE_FLOAT32ARRAY;
  }

  static constexpr unsigned int ANIMATIONLOOPMODE_RELATIVE()
  {
    return Animation::_ANIMATIONLOOPMODE_RELATIVE;
  }

  static constexpr unsigned int ANIMATIONLOOPMODE_CYCLE()
  {
    return Animation::_ANIMATIONLOOPMODE_CYCLE;
  }

  static constexpr unsigned int ANIMATIONLOOPMODE_CONSTANT()
  {
    return Animation::_ANIMATIONLOOPMODE_CONSTANT;
  }

  static bool AllowMatricesInterpolation();

  static Animation* _PrepareAnimation(
    const string_t& name, const string_t& targetProperty, size_t framePerSecond,
    int totalFrame, const AnimationValue& from, const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
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

  /**
   * @brief Create and start an animation on a node
   * @param {string} name defines the name of the global animation that will be
   * run on all nodes
   * @param {BABYLON.Node} node defines the root node where the animation will
   * take place
   * @param {string} targetProperty defines property to animate
   * @param {number} framePerSecond defines the number of frame per second yo
   * use
   * @param {number} totalFrame defines the number of frames in total
   * @param {any} from defines the initial value
   * @param {any} to defines the final value
   * @param {number} loopMode defines which loop mode you want to use (off by
   * default)
   * @param {BABYLON.EasingFunction} easingFunction defines the easing function
   * to use (linear by default)
   * @param onAnimationEnd defines the callback to call when animation end
   * @returns the animatable created for this animation
   */
  static Animatable* CreateAndStartAnimation(
    const string_t& name, Node* node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Create and start an animation on a node and its descendants
   * @param {string} name defines the name of the global animation that will be
   * run on all nodes
   * @param {BABYLON.Node} node defines the root node where the animation will
   * take place
   * @param {boolean} directDescendantsOnly if true only direct descendants will
   * be used, if false direct and also indirect (children of children, an so on
   * in a recursive manner) descendants will be used.
   * @param {string} targetProperty defines property to animate
   * @param {number} framePerSecond defines the number of frame per second yo
   * use
   * @param {number} totalFrame defines the number of frames in total
   * @param {any} from defines the initial value
   * @param {any} to defines the final value
   * @param {number} loopMode defines which loop mode you want to use (off by
   * default)
   * @param {BABYLON.EasingFunction} easingFunction defines the easing function
   * to use (linear by default)
   * @param onAnimationEnd defines the callback to call when an animation ends
   * (will be called once per node)
   * @returns the list of animatables created for all nodes
   * Example https://www.babylonjs-playground.com/#MH0VLI
   */
  static vector_t<Animatable*> CreateAndStartHierarchyAnimation(
    const string_t& name, Node* node, bool directDescendantsOnly,
    const string_t& targetProperty, size_t framePerSecond, int totalFrame,
    const AnimationValue& from, const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  static Animatable* CreateMergeAndStartAnimation(
    const string_t& name, Node* node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
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

public:
  Animation(const string_t& name, const string_t& targetProperty,
            size_t framePerSecond, int dataType,
            unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE());
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
   * @brief Return the array of runtime animations currently using this
   * animation.
   */
  vector_t<RuntimeAnimation*>& runtimeAnimations();

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
  vector_t<IAnimationKey>& getKeys();
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
  void setKeys(const vector_t<IAnimationKey>& values);
  unique_ptr_t<Animation> clone() const;
  Json::object serialize() const;
  static Animation* Parse(const Json::value& parsedAnimation);
  static void AppendSerializedAnimations(IAnimatable* source,
                                         picojson::object& destination);

private:
  bool get_hasRunningRuntimeAnimations() const;

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
  ReadOnlyProperty<Animation, bool> hasRunningRuntimeAnimations;

private:
  vector_t<IAnimationKey> _keys;
  IEasingFunction* _easingFunction;
  // The set of event that will be linked to this animation
  vector_t<AnimationEvent> _events;
  unordered_map_t<string_t, AnimationRange> _ranges;

}; // end of class Animation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_H
