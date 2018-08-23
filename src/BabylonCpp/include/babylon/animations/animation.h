#ifndef BABYLON_ANIMATIONS_ANIMATION_H
#define BABYLON_ANIMATIONS_ANIMATION_H

#include <babylon/animations/animation_event.h>
#include <babylon/animations/animation_range.h>
#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Class used to store any kind of animation.
 */
class BABYLON_SHARED_EXPORT Animation {

private:
  /** Statics **/

  /**
   * Float animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_FLOAT = 0;

  /**
   * Vector3 animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_VECTOR3 = 1;

  /**
   * Quaternion animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_QUATERNION = 2;

  /**
   * Matrix animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_MATRIX = 3;

  /**
   * Color3 animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_COLOR3 = 4;

  /**
   * Vector2 animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_VECTOR2 = 5;

  /**
   * Size animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_SIZE = 6;

  /**
   * Boolean animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_BOOL = 20;

  /**
   * Integer animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_INT = 21;

  /**
   * String animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_STRING = 22;

  /**
   * Color4 animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_COLOR4 = 23;

  /**
   * Float32Array animation type
   */
  static constexpr unsigned int _ANIMATIONTYPE_FLOAT32ARRAY = 30;

  /**
   * Relative Loop Mode
   */
  static constexpr unsigned int _ANIMATIONLOOPMODE_RELATIVE = 0;

  /**
   * Cycle Loop Mode
   */
  static constexpr unsigned int _ANIMATIONLOOPMODE_CYCLE = 1;

  /**
   * Constant Loop Mode
   */
  static constexpr unsigned int _ANIMATIONLOOPMODE_CONSTANT = 2;

public:
  /**
   * @brief Get the float animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_FLOAT()
  {
    return Animation::_ANIMATIONTYPE_FLOAT;
  }

  /**
   * @brief Get the Vector3 animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_VECTOR3()
  {
    return Animation::_ANIMATIONTYPE_VECTOR3;
  }

  /**
   * @brief Get the Quaternion animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_QUATERNION()
  {
    return Animation::_ANIMATIONTYPE_QUATERNION;
  }

  /**
   * @brief Get the Matrix animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_MATRIX()
  {
    return Animation::_ANIMATIONTYPE_MATRIX;
  }

  /**
   * @brief Get the Color3 animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_COLOR3()
  {
    return Animation::_ANIMATIONTYPE_COLOR3;
  }

  /**
   *@brief  Get the Vector2 animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_VECTOR2()
  {
    return Animation::_ANIMATIONTYPE_VECTOR2;
  }

  /**
   *@brief Get the Size animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_SIZE()
  {
    return Animation::_ANIMATIONTYPE_SIZE;
  }

  /**
   * @brief Get the Boolean animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_BOOL()
  {
    return Animation::_ANIMATIONTYPE_BOOL;
  }

  /**
   * @brief Get the Integer animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_INT()
  {
    return Animation::_ANIMATIONTYPE_INT;
  }

  /**
   * @brief Get the String animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_STRING()
  {
    return Animation::_ANIMATIONTYPE_STRING;
  }

  /**
   * @brief Get the Color4 animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_COLOR4()
  {
    return Animation::_ANIMATIONTYPE_COLOR4;
  }

  /**
   * @brief Get the Float32Array animation type.
   */
  static constexpr unsigned int ANIMATIONTYPE_FLOAT32ARRAY()
  {
    return Animation::_ANIMATIONTYPE_FLOAT32ARRAY;
  }

  /**
   * @brief Get the Relative Loop Mode.
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_RELATIVE()
  {
    return Animation::_ANIMATIONLOOPMODE_RELATIVE;
  }

  /**
   * @brief Get the Cycle Loop Mode.
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_CYCLE()
  {
    return Animation::_ANIMATIONLOOPMODE_CYCLE;
  }

  /**
   * @brief Get the Constant Loop Mode.
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_CONSTANT()
  {
    return Animation::_ANIMATIONLOOPMODE_CONSTANT;
  }

  /**
   * @brief Use matrix interpolation instead of using direct key value when
   * animating matrices.
   */
  static bool AllowMatricesInterpolation();

  /**
   * @brief When matrix interpolation is enabled, this boolean forces the system
   * to use Matrix.DecomposeLerp instead of Matrix.Lerp. Interpolation is more
   * precise but slower.
   */
  static bool AllowMatrixDecomposeForInterpolation();

  /**
   * @brief Hidden Internal use.
   */
  static AnimationPtr _PrepareAnimation(
    const string_t& name, const string_t& targetProperty, size_t framePerSecond,
    int totalFrame, const AnimationValue& from, const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr);

  /**
   * @brief Sets up an animation.
   * @param property The property to animate
   * @param animationType The animation type to apply
   * @param framePerSecond The frames per second of the animation
   * @param easingFunction The easing function used in the animation
   * @returns The created animation
   */
  static AnimationPtr
  CreateAnimation(const string_t& property, int animationType,
                  std::size_t framePerSecond,
                  IEasingFunction* easingFunction = nullptr);

  /**
   * @brief Create and start an animation on a node.
   * @param name defines the name of the global animation that will be run on
   * all nodes
   * @param node defines the root node where the animation will take place
   * @param targetProperty defines property to animate
   * @param framePerSecond defines the number of frame per second yo use
   * @param totalFrame defines the number of frames in total
   * @param from defines the initial value
   * @param to defines the final value
   * @param loopMode defines which loop mode you want to use (off by default)
   * @param easingFunction defines the easing function to use (linear by
   * default)
   * @param onAnimationEnd defines the callback to call when animation end
   * @returns the animatable created for this animation
   */
  static AnimatablePtr CreateAndStartAnimation(
    const string_t& name, const NodePtr& node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Create and start an animation on a node and its descendants.
   * @param name defines the name of the global animation that will be run on
   * all nodes
   * @param node defines the root node where the animation will take place
   * @param directDescendantsOnly if true only direct descendants will be used,
   * if false direct and also indirect (children of children, an so on in a
   * recursive manner) descendants will be used
   * @param targetProperty defines property to animate
   * @param framePerSecond defines the number of frame per second to use
   * @param totalFrame defines the number of frames in total
   * @param from defines the initial value
   * @param to defines the final value
   * @param loopMode defines which loop mode you want to use (off by default)
   * @param easingFunction defines the easing function to use (linear by
   * default)
   * @param onAnimationEnd defines the callback to call when an animation ends
   * (will be called once per node)
   * @returns the list of animatables created for all nodes
   * Example https://www.babylonjs-playground.com/#MH0VLI
   */
  static vector_t<AnimatablePtr> CreateAndStartHierarchyAnimation(
    const string_t& name, const NodePtr& node, bool directDescendantsOnly,
    const string_t& targetProperty, size_t framePerSecond, int totalFrame,
    const AnimationValue& from, const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Creates a new animation, merges it with the existing animations and
   * starts it.
   * @param name Name of the animation
   * @param node Node which contains the scene that begins the animations
   * @param targetProperty Specifies which property to animate
   * @param framePerSecond The frames per second of the animation
   * @param totalFrame The total number of frames
   * @param from The frame at the beginning of the animation
   * @param to The frame at the end of the animation
   * @param loopMode Specifies the loop mode of the animation
   * @param easingFunction (Optional) The easing function of the animation,
   * which allow custom mathematical formulas for animations
   * @param onAnimationEnd Callback to run once the animation is complete
   * @returns Nullable animation
   */
  static AnimatablePtr CreateMergeAndStartAnimation(
    const string_t& name, const NodePtr& node, const string_t& targetProperty,
    size_t framePerSecond, int totalFrame, const AnimationValue& from,
    const AnimationValue& to,
    unsigned int loopMode           = Animation::ANIMATIONLOOPMODE_CYCLE(),
    IEasingFunction* easingFunction = nullptr,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Transition property of an host to the target Value.
   * @param property The property to transition
   * @param targetValue The target Value of the property
   * @param host The object where the property to animate belongs
   * @param scene Scene used to run the animation
   * @param frameRate Framerate (in frame/s) to use
   * @param transition The transition type we want to use
   * @param duration The duration of the animation, in milliseconds
   * @param onAnimationEnd Callback trigger at the end of the animation
   * @returns Nullable animation
   */
  static Animatable*
  TransitionTo(const string_t& property, const AnimationValue& targetValue,
               const AnimationValue& host, Scene* scene, float frameRate,
               const AnimationPtr& transition, float duration,
               const ::std::function<void()>& onAnimationEnd = nullptr);

public:
  template <typename... Ts>
  static AnimationPtr New(Ts&&... args)
  {
    auto animation
      = shared_ptr_t<Animation>(new Animation(::std::forward<Ts>(args)...));

    return animation;
  }
  ~Animation();

  /** Methods **/

  /**
   * @param Converts the animation to a string.
   * @param fullDetails support for multiple levels of logging within scene
   * loading
   * @returns String form of the animation
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @param Add an event to this animation.
   * @param event Event to add
   */
  void addEvent(const AnimationEvent& event);

  /**
   * @brief Return the array of runtime animations currently using this
   * animation.
   */
  vector_t<RuntimeAnimationPtr>& runtimeAnimations();

  /**
   * @brief Remove all events found at the given frame.
   * @param frame The frame to remove events from
   */
  void removeEvents(int frame);

  /**
   * @brief Retrieves all the events from the animation.
   * @returns Events from the animation
   */
  vector_t<AnimationEvent>& getEvents();

  /**
   * @brief Creates an animation range.
   * @param name Name of the animation range
   * @param from Starting frame of the animation range
   * @param to Ending frame of the animation
   */
  void createRange(const string_t& name, float from, float to);

  /**
   * @brief Deletes an animation range by name.
   * @param name Name of the animation range to delete
   * @param deleteFrames Specifies if the key frames for the range should also
   * be deleted (true) or not (false)
   */
  void deleteRange(const string_t& name, bool deleteFrames = true);

  /**
   * @brief Gets the animation range by name, or null if not defined.
   * @param name Name of the animation range
   * @returns Nullable animation range
   */
  AnimationRange& getRange(const string_t& name);

  void reset();
  bool isStopped() const;

  /**
   * @brief Gets the key frames from the animation.
   * @returns The key frames of the animation
   */
  vector_t<IAnimationKey>& getKeys();

  /**
   * @brief Gets the highest frame rate of the animation.
   * @returns Highest frame rate of the animation
   */
  float getHighestFrame() const;

  /**
   * @brief Gets the easing function of the animation.
   * @returns Easing function of the animation
   */
  IEasingFunction* getEasingFunction();

  /**
   * @brief Sets the easing function of the animation.
   * @param easingFunction A custom mathematical formula for animation
   */
  void setEasingFunction(IEasingFunction* easingFunction);

  /**
   * @brief Interpolates a scalar linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated scalar value
   */
  float floatInterpolateFunction(float startValue, float endValue,
                                 float gradient) const;

  /**
   * @brief Interpolates a scalar cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated scalar value
   */
  float floatInterpolateFunctionWithTangents(float startValue, float outTangent,
                                             float endValue, float inTangent,
                                             float gradient) const;

  /**
   * @brief Interpolates a quaternion using a spherical linear interpolation.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated quaternion value
   */
  Quaternion quaternionInterpolateFunction(const Quaternion& startValue,
                                           const Quaternion& endValue,
                                           float gradient) const;

  /**
   * @brief Interpolates a quaternion cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation curve
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated quaternion value
   */
  Quaternion quaternionInterpolateFunctionWithTangents(
    const Quaternion& startValue, const Quaternion& outTangent,
    const Quaternion& endValue, const Quaternion& inTangent,
    float gradient) const;

  /**
   * @brief Interpolates a Vector3 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated scalar value
   */
  Vector3 vector3InterpolateFunction(const Vector3& startValue,
                                     const Vector3& endValue,
                                     float gradient) const;

  /**
   * @brief Interpolates a Vector3 cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns InterpolatedVector3 value
   */
  Vector3 vector3InterpolateFunctionWithTangents(const Vector3& startValue,
                                                 const Vector3& outTangent,
                                                 const Vector3& endValue,
                                                 const Vector3& inTangent,
                                                 float gradient) const;

  /**
   * @brief Interpolates a Vector2 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Vector2 value
   */
  Vector2 vector2InterpolateFunction(const Vector2& startValue,
                                     const Vector2& endValue,
                                     float gradient) const;

  /**
   * @brief Interpolates a Vector2 cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Vector2 value
   */
  Vector2 vector2InterpolateFunctionWithTangents(const Vector2& startValue,
                                                 const Vector2& outTangent,
                                                 const Vector2& endValue,
                                                 const Vector2& inTangent,
                                                 float gradient) const;

  /**
   * @brief Interpolates a size linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Size value
   */
  Size sizeInterpolateFunction(const Size& startValue, const Size& endValue,
                               float gradient) const;

  /**
   * @brief Interpolates a Color3 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Color3 value
   */
  Color3 color3InterpolateFunction(const Color3& startValue,
                                   const Color3& endValue,
                                   float gradient) const;

  /**
   * @brief Hidden Internal use only.
   */
  AnimationValue _getKeyValue(const AnimationValue& value) const;

  /**
   * @brief Hidden Internal use only.
   */
  AnimationValue
  _interpolate(float currentFrame, int repeatCount,
               nullable_t<AnimationValue>& workValue, unsigned int loopMode,
               const AnimationValue& offsetValue    = AnimationValue(),
               const AnimationValue& highLimitValue = AnimationValue());

  /**
   * @brief Defines the function to use to interpolate matrices.
   * @param startValue defines the start matrix
   * @param endValue defines the end matrix
   * @param gradient defines the gradient between both matrices
   * @returns the interpolated matrix
   */
  Matrix matrixInterpolateFunction(Matrix& startValue, Matrix& endValue,
                                   float gradient) const;

  /**
   * @brief Defines the function to use to interpolate matrices.
   * @param startValue defines the start matrix
   * @param endValue defines the end matrix
   * @param gradient defines the gradient between both matrices
   * @param result defines an optional target matrix where to store the
   * interpolation
   * @returns the interpolated matrix
   */
  Matrix matrixInterpolateFunction(Matrix& startValue, Matrix& endValue,
                                   float gradient, Matrix& result) const;

  /**
   * @brief Makes a copy of the animation.
   * @returns Cloned animation
   */
  AnimationPtr clone() const;

  /**
   * @brief Sets the key frames of the animation.
   * @param values The animation key frames to set
   */
  void setKeys(const vector_t<IAnimationKey>& values);

  /**
   * @brief Serializes the animation to an object.
   * @returns Serialized object
   */
  Json::object serialize() const;

  /**
   * @brief Parses an animation object and creates an animation.
   * @param parsedAnimation Parsed animation object
   * @returns Animation object
   */
  static Animation* Parse(const Json::value& parsedAnimation);

  /**
   * @brief Appends the serialized animations from the source animations.
   * @param source Source containing the animations
   * @param destination Target to store the animations
   */
  static void AppendSerializedAnimations(IAnimatable* source,
                                         picojson::object& destination);

protected:
  /**
   * @brief Initializes the animation.
   * @param name Name of the animation
   * @param targetProperty Property to animate
   * @param framePerSecond The frames per second of the animation
   * @param dataType The data type of the animation
   * @param loopMode The loop mode of the animation
   * @param enableBlendings Specifies if blending should be enabled
   */
  Animation(const string_t& name, const string_t& targetProperty,
            size_t framePerSecond, int dataType,
            unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE());

private:
  /**
   * @brief Specifies if any of the runtime animations are currently running.
   */
  bool get_hasRunningRuntimeAnimations() const;

private:
  /**
   * Use matrix interpolation instead of using direct key value when animating
   * matrices
   */
  static bool _AllowMatricesInterpolation;

  /**
   * When matrix interpolation is enabled, this boolean forces the system to use
   * Matrix.DecomposeLerp instead of Matrix.Lerp. Interpolation is more precise
   * but slower
   */
  static bool _AllowMatrixDecomposeForInterpolation;

public:
  /**
   * Name of the animation
   */
  string_t name;

  /**
   * Property to animate
   */
  string_t targetProperty;

  /**
   * Hidden Internal use only
   */
  vector_t<RuntimeAnimationPtr> _runtimeAnimations;

  /**
   * Stores an array of target property paths
   */
  vector_t<string_t> targetPropertyPath;

  /**
   * The frames per second of the animation
   */
  size_t framePerSecond;

  /**
   * The data type of the animation
   */
  int dataType;

  /**
   * The loop mode of the animation
   */
  unsigned int loopMode;

  /**
   * Stores the blending speed of the animation
   */
  float blendingSpeed;

  /**
   * Specifies if blending should be enabled
   */
  bool enableBlending;

  /**
   * Specifies if any of the runtime animations are currently running
   */
  ReadOnlyProperty<Animation, bool> hasRunningRuntimeAnimations;

private:
  /**
   * Stores the key frames of the animation
   */
  vector_t<IAnimationKey> _keys;

  /**
   * Stores the easing function of the animation
   */
  IEasingFunction* _easingFunction;

  /**
   * The set of event that will be linked to this animation
   */
  vector_t<AnimationEvent> _events;

  /**
   * Stores the animation ranges for the animation
   */
  unordered_map_t<string_t, AnimationRange> _ranges;

}; // end of class Animation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_H
