#ifndef BABYLON_ANIMATIONS_ANIMATION_H
#define BABYLON_ANIMATIONS_ANIMATION_H

#include <nlohmann/json_fwd.hpp>
#include <unordered_map>

#include <babylon/animations/animation_event.h>
#include <babylon/animations/animation_range.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

struct _IAnimationState;
class Animatable;
class Animation;
class IAnimatable;
struct IAnimationKey;
struct IEasingFunction;
class Node;
class RuntimeAnimation;
class Scene;
using AnimatablePtr       = std::shared_ptr<Animatable>;
using AnimationPtr        = std::shared_ptr<Animation>;
using IEasingFunctionPtr  = std::shared_ptr<IEasingFunction>;
using NodePtr             = std::shared_ptr<Node>;
using RuntimeAnimationPtr = std::shared_ptr<RuntimeAnimation>;

/**
 * @brief Class used to store any kind of animation.
 */
class BABYLON_SHARED_EXPORT Animation {

public:
  /** Statics **/

  /**
   * Float animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_FLOAT = 0;

  /**
   * Vector3 animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_VECTOR3 = 1;

  /**
   * Quaternion animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_QUATERNION = 2;

  /**
   * Matrix animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_MATRIX = 3;

  /**
   * Color3 animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_COLOR3 = 4;

  /**
   * Color4 animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_COLOR4 = 7;

  /**
   * Vector2 animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_VECTOR2 = 5;

  /**
   * Size animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_SIZE = 6;

  /**
   * Boolean animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_BOOL = 20;

  /**
   * Integer animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_INT = 21;

  /**
   * String animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_STRING = 22;

  /**
   * Float32Array animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_FLOAT32ARRAY = 30;

  /**
   * Vector4 animation type
   */
  static constexpr unsigned int ANIMATIONTYPE_VECTOR4 = 40;

  /**
   * Relative Loop Mode
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_RELATIVE = 0;

  /**
   * Cycle Loop Mode
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_CYCLE = 1;

  /**
   * Constant Loop Mode
   */
  static constexpr unsigned int ANIMATIONLOOPMODE_CONSTANT = 2;

public:
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
  static AnimationPtr _PrepareAnimation(const std::string& name, const std::string& targetProperty,
                                        size_t framePerSecond, float totalFrame,
                                        const AnimationValue& from, const AnimationValue& to,
                                        unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
                                        const IEasingFunctionPtr& easingFunction = nullptr);

  /**
   * @brief Sets up an animation.
   * @param property The property to animate
   * @param animationType The animation type to apply
   * @param framePerSecond The frames per second of the animation
   * @param easingFunction The easing function used in the animation
   * @returns The created animation
   */
  static AnimationPtr CreateAnimation(const std::string& property, int animationType,
                                      std::size_t framePerSecond,
                                      const IEasingFunctionPtr& easingFunction = nullptr);

  /**
   * @brief Create and start an animation on a node.
   * @param name defines the name of the global animation that will be run on all nodes
   * @param node defines the root node where the animation will take place
   * @param targetProperty defines property to animate
   * @param framePerSecond defines the number of frame per second yo use
   * @param totalFrame defines the number of frames in total
   * @param from defines the initial value
   * @param to defines the final value
   * @param loopMode defines which loop mode you want to use (off by default)
   * @param easingFunction defines the easing function to use (linear by default)
   * @param onAnimationEnd defines the callback to call when animation end
   * @returns the animatable created for this animation
   */
  static AnimatablePtr
  CreateAndStartAnimation(const std::string& name, const NodePtr& node,
                          const std::string& targetProperty, size_t framePerSecond,
                          float totalFrame, const AnimationValue& from, const AnimationValue& to,
                          unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
                          const IEasingFunctionPtr& easingFunction    = nullptr,
                          const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Create and start an animation on a node and its descendants.
   * @param name defines the name of the global animation that will be run on all nodes
   * @param node defines the root node where the animation will take place
   * @param directDescendantsOnly if true only direct descendants will be used, if false direct and
   * also indirect (children of children, an so on in a recursive manner) descendants will be used
   * @param targetProperty defines property to animate
   * @param framePerSecond defines the number of frame per second to use
   * @param totalFrame defines the number of frames in total
   * @param from defines the initial value
   * @param to defines the final value
   * @param loopMode defines which loop mode you want to use (off by default)
   * @param easingFunction defines the easing function to use (linear by default)
   * @param onAnimationEnd defines the callback to call when an animation ends (will be called once
   * per node)
   * @returns the list of animatables created for all nodes
   * @example https://www.babylonjs-playground.com/#MH0VLI
   */
  static std::vector<AnimatablePtr>
  CreateAndStartHierarchyAnimation(const std::string& name, const NodePtr& node,
                                   bool directDescendantsOnly, const std::string& targetProperty,
                                   size_t framePerSecond, float totalFrame,
                                   const AnimationValue& from, const AnimationValue& to,
                                   unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
                                   const IEasingFunctionPtr& easingFunction    = nullptr,
                                   const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Creates a new animation, merges it with the existing animations and starts it
   * @param name Name of the animation
   * @param node Node which contains the scene that begins the animations
   * @param targetProperty Specifies which property to animate
   * @param framePerSecond The frames per second of the animation
   * @param totalFrame The total number of frames
   * @param from The frame at the beginning of the animation
   * @param to The frame at the end of the animation
   * @param loopMode Specifies the loop mode of the animation
   * @param easingFunction (Optional) The easing function of the animation, which allow custom
   * mathematical formulas for animations
   * @param onAnimationEnd Callback to run once the animation is complete
   * @returns Nullable animation
   */
  static AnimatablePtr CreateMergeAndStartAnimation(
    const std::string& name, const NodePtr& node, const std::string& targetProperty,
    size_t framePerSecond, float totalFrame, const AnimationValue& from, const AnimationValue& to,
    unsigned int loopMode                       = Animation::ANIMATIONLOOPMODE_CYCLE,
    const IEasingFunctionPtr& easingFunction    = nullptr,
    const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Convert the keyframes for all animations belonging to the group to be relative to a
   * given reference frame.
   * @param sourceAnimation defines the Animation containing keyframes to convert
   * @param referenceFrame defines the frame that keyframes in the range will be relative to
   * @param range defines the name of the AnimationRange belonging to the Animation to convert
   * @param cloneOriginal defines whether or not to clone the animation and convert the clone or
   * convert the original animation (default is false)
   * @param clonedName defines the name of the resulting cloned Animation if cloneOriginal is true
   * @returns a new Animation if cloneOriginal is true or the original Animation if cloneOriginal is
   * false
   */
  static AnimatablePtr MakeAnimationAdditive(const AnimationPtr& sourceAnimation,
                                             int referenceFrame = 0, const std::string& range = "",
                                             bool cloneOriginal            = false,
                                             const std::string& clonedName = "");

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
  static Animatable* TransitionTo(const std::string& property, const AnimationValue& targetValue,
                                  const Node* host, Scene* scene, float frameRate,
                                  const AnimationPtr& transition, float duration,
                                  const std::function<void()>& onAnimationEnd = nullptr);

public:
  template <typename... Ts>
  static AnimationPtr New(Ts&&... args)
  {
    auto animation = std::shared_ptr<Animation>(new Animation(std::forward<Ts>(args)...));

    return animation;
  }
  ~Animation(); // = default

  /** Methods **/

  /**
   * @param Converts the animation to a string.
   * @param fullDetails support for multiple levels of logging within scene loading
   * @returns String form of the animation
   */
  [[nodiscard]] std::string toString(bool fullDetails = false) const;

  /**
   * @param Add an event to this animation.
   * @param event Event to add
   */
  void addEvent(const AnimationEvent& event);

  /**
   * @brief Return the array of runtime animations currently using this animation.
   */
  std::vector<RuntimeAnimationPtr>& runtimeAnimations();

  /**
   * @brief Remove all events found at the given frame.
   * @param frame The frame to remove events from
   */
  void removeEvents(float frame);

  /**
   * @brief Retrieves all the events from the animation.
   * @returns Events from the animation
   */
  std::vector<AnimationEvent>& getEvents();

  /**
   * @brief Creates an animation range.
   * @param name Name of the animation range
   * @param from Starting frame of the animation range
   * @param to Ending frame of the animation
   */
  void createRange(const std::string& name, float from, float to);

  /**
   * @brief Deletes an animation range by name.
   * @param name Name of the animation range to delete
   * @param deleteFrames Specifies if the key frames for the range should also be deleted (true) or
   * not (false)
   */
  void deleteRange(const std::string& name, bool deleteFrames = true);

  /**
   * @brief Gets the animation range by name, or null if not defined.
   * @param name Name of the animation range
   * @returns Nullable animation range
   */
  AnimationRange& getRange(const std::string& name);

  void reset();
  [[nodiscard]] bool isStopped() const;

  /**
   * @brief Gets the key frames from the animation.
   * @returns The key frames of the animation
   */
  std::vector<IAnimationKey>& getKeys();

  /**
   * @brief Gets the highest frame rate of the animation.
   * @returns Highest frame rate of the animation
   */
  [[nodiscard]] float getHighestFrame() const;

  /**
   * @brief Gets the easing function of the animation.
   * @returns Easing function of the animation
   */
  IEasingFunctionPtr& getEasingFunction();

  /**
   * @brief Sets the easing function of the animation.
   * @param easingFunction A custom mathematical formula for animation
   */
  void setEasingFunction(const IEasingFunctionPtr& easingFunction);

  /**
   * @brief Interpolates a scalar linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated scalar value
   */
  [[nodiscard]] float floatInterpolateFunction(float startValue, float endValue,
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
  [[nodiscard]] float floatInterpolateFunctionWithTangents(float startValue, float outTangent,
                                                           float endValue, float inTangent,
                                                           float gradient) const;

  /**
   * @brief Interpolates a quaternion using a spherical linear interpolation.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated quaternion value
   */
  [[nodiscard]] Quaternion quaternionInterpolateFunction(const Quaternion& startValue,
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
  [[nodiscard]] Quaternion quaternionInterpolateFunctionWithTangents(const Quaternion& startValue,
                                                                     const Quaternion& outTangent,
                                                                     const Quaternion& endValue,
                                                                     const Quaternion& inTangent,
                                                                     float gradient) const;

  /**
   * @brief Interpolates a Vector3 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated scalar value
   */
  [[nodiscard]] Vector3 vector3InterpolateFunction(const Vector3& startValue,
                                                   const Vector3& endValue, float gradient) const;

  /**
   * @brief Interpolates a Vector3 cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns InterpolatedVector3 value
   */
  [[nodiscard]] Vector3 vector3InterpolateFunctionWithTangents(const Vector3& startValue,
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
  [[nodiscard]] Vector2 vector2InterpolateFunction(const Vector2& startValue,
                                                   const Vector2& endValue, float gradient) const;

  /**
   * @brief Interpolates a Vector2 cubically.
   * @param startValue Start value of the animation curve
   * @param outTangent End tangent of the animation
   * @param endValue End value of the animation curve
   * @param inTangent Start tangent of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Vector2 value
   */
  [[nodiscard]] Vector2 vector2InterpolateFunctionWithTangents(const Vector2& startValue,
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
  [[nodiscard]] Size sizeInterpolateFunction(const Size& startValue, const Size& endValue,
                                             float gradient) const;

  /**
   * @brief Interpolates a Color3 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Color3 value
   */
  [[nodiscard]] Color3 color3InterpolateFunction(const Color3& startValue, const Color3& endValue,
                                                 float gradient) const;

  /**
   * @brief Interpolates a Color4 linearly.
   * @param startValue Start value of the animation curve
   * @param endValue End value of the animation curve
   * @param gradient Scalar amount to interpolate
   * @returns Interpolated Color3 value
   */
  Color4 color4InterpolateFunction(const Color4& startValue, const Color4& endValue,
                                   float gradient) const;

  /**
   * @brief Hidden Internal use only.
   */
  [[nodiscard]] AnimationValue _getKeyValue(const AnimationValue& value) const;

  /**
   * @brief Hidden Internal use only.
   */
  AnimationValue _interpolate(float currentFrame, _IAnimationState& state);

  /**
   * @brief Defines the function to use to interpolate matrices.
   * @param startValue defines the start matrix
   * @param endValue defines the end matrix
   * @param gradient defines the gradient between both matrices
   * @returns the interpolated matrix
   */
  Matrix matrixInterpolateFunction(Matrix& startValue, Matrix& endValue, float gradient) const;

  /**
   * @brief Defines the function to use to interpolate matrices.
   * @param startValue defines the start matrix
   * @param endValue defines the end matrix
   * @param gradient defines the gradient between both matrices
   * @param result defines an optional target matrix where to store the
   * interpolation
   * @returns the interpolated matrix
   */
  Matrix matrixInterpolateFunction(Matrix& startValue, Matrix& endValue, float gradient,
                                   Matrix& result) const;

  /**
   * @brief Makes a copy of the animation.
   * @returns Cloned animation
   */
  [[nodiscard]] AnimationPtr clone() const;

  /**
   * @brief Sets the key frames of the animation.
   * @param values The animation key frames to set
   */
  void setKeys(const std::vector<IAnimationKey>& values);

  /**
   * @brief Serializes the animation to an object.
   * @returns Serialized object
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parses an animation object and creates an animation.
   * @param parsedAnimation Parsed animation object
   * @returns Animation object
   */
  static AnimationPtr Parse(const json& parsedAnimation);

  /**
   * @brief Appends the serialized animations from the source animations.
   * @param source Source containing the animations
   * @param destination Target to store the animations
   */
  static void AppendSerializedAnimations(IAnimatable* source, json& destination);

protected:
  /**
   * @brief Initializes the animation.
   * @param name Name of the animation
   * @param targetProperty Property to animate
   * @param framePerSecond The frames per second of the animation
   * @param dataType The data type of the animation
   * @param loopMode The loop mode of the animation
   * @param enableBlending Specifies if blending should be enabled
   */
  Animation(const std::string& name, const std::string& targetProperty, size_t framePerSecond,
            int dataType, unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE,
            bool enableBlending = false);

private:
  /**
   * @brief Specifies if any of the runtime animations are currently running.
   */
  [[nodiscard]] bool get_hasRunningRuntimeAnimations() const;

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
  std::string name;

  /**
   * Property to animate
   */
  std::string targetProperty;

  /**
   * Hidden Internal use only
   */
  std::vector<RuntimeAnimationPtr> _runtimeAnimations;

  /**
   * Stores an array of target property paths
   */
  std::vector<std::string> targetPropertyPath;

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
  std::vector<IAnimationKey> _keys;

  /**
   * Stores the easing function of the animation
   */
  IEasingFunctionPtr _easingFunction;

  /**
   * The set of event that will be linked to this animation
   */
  std::vector<AnimationEvent> _events;

  /**
   * Stores the animation ranges for the animation
   */
  std::unordered_map<std::string, AnimationRange> _ranges;

}; // end of class Animation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_H
