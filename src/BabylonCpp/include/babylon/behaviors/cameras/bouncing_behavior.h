#ifndef BABYLON_BEHAVIORS_CAMERAS_BOUNCING_BEHAVIOR_H
#define BABYLON_BEHAVIORS_CAMERAS_BOUNCING_BEHAVIOR_H

#include <babylon/animations/easing/back_ease.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Animatable;
class Animation;
class ArcRotateCamera;
class Camera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using AnimationPtr       = std::shared_ptr<Animation>;

/**
 * @brief Add a bouncing effect to an ArcRotateCamera when reaching a specified
 * minimum and maximum radius
 * @see http://doc.babylonjs.com/how_to/camera_behaviors#bouncing-behavior
 */
class BABYLON_SHARED_EXPORT BouncingBehavior : public Behavior<ArcRotateCamera> {

public:
  /**
   * The easing function used by animations
   */
  static BackEasePtr _EasingFunction;

  /**
   * The easing mode used by animations
   */
  static unsigned int EasingMode;

public:
  BouncingBehavior();
  ~BouncingBehavior() override; // = default

  /**
   * @brief Gets the name of the behavior.
   */
  [[nodiscard]] const char* name() const;

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the behavior to its arc rotate camera.
   * @param camera Defines the camera to attach the behavior to
   * @param predicate Predicate to use for pick filtering
   */
  void attach(const ArcRotateCameraPtr& camera,
              const std::function<bool(const AbstractMeshPtr& m)>& predicate = nullptr) override;

  /**
   * @brief Detaches the behavior from its current arc rotate camera.
   */
  void detach() override;

  /**
   * @brief Stops and removes all animations that have been applied to the
   * camera
   */
  void stopAllAnimations();

protected:
  /**
   * @brief Removes all animation locks. Allows new animations to be added to
   * any of the camera properties.
   */
  void _clearAnimationLocks();

private:
  /**
   * @brief Gets a value indicating if the lowerRadiusTransitionRange and
   * upperRadiusTransitionRange are defined automatically.
   */
  [[nodiscard]] bool get_autoTransitionRange() const;

  /**
   * @brief Sets a value indicating if the lowerRadiusTransitionRange and
   * upperRadiusTransitionRange are defined automatically
   * Transition ranges will be set to 5% of the bounding box diagonal in world
   * space
   */
  void set_autoTransitionRange(bool value);

  /**
   * @brief Checks if the camera radius is at the specified limit. Takes into
   * account animation locks.
   * @param radiusLimit The limit to check against.
   * @return Bool to indicate if at limit.
   */
  [[nodiscard]] bool _isRadiusAtLimit(float radiusLimit) const;

  /**
   * @brief Applies an animation to the radius of the camera, extending by the
   * radiusDelta.
   * @param radiusDelta The delta by which to animate to. Can be negative.
   */
  void _applyBoundRadiusAnimation(float radiusDelta);

public:
  /**
   * The duration of the animation, in milliseconds
   */
  float transitionDuration;

  /**
   * Length of the distance animated by the transition when lower radius is
   * reached
   */
  float lowerRadiusTransitionRange;

  /**
   * Length of the distance animated by the transition when upper radius is
   * reached
   */
  float upperRadiusTransitionRange;

  /**
   * Value indicating if the lowerRadiusTransitionRange and
   * upperRadiusTransitionRange are defined automatically
   */
  Property<BouncingBehavior, bool> autoTransitionRange;

private:
  bool _autoTransitionRange;

  // Connection
  ArcRotateCameraPtr _attachedCamera;
  Observer<Camera>::Ptr _onAfterCheckInputsObserver;
  Observer<AbstractMesh>::Ptr _onMeshTargetChangedObserver;

  // Animations
  bool _radiusIsAnimating;
  AnimationPtr _radiusBounceTransition;
  std::vector<Animatable*> _animatables;
  float _cachedWheelPrecision;

}; // end of class BouncingBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_CAMERAS_BOUNCING_BEHAVIOR_H
