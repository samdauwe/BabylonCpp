#ifndef BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H
#define BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class ArcRotateCamera;
class Camera;
class PointerInfoPre;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;

class BABYLON_SHARED_EXPORT AutoRotationBehavior
    : public Behavior<ArcRotateCamera> {

public:
  AutoRotationBehavior();
  virtual ~AutoRotationBehavior();

  const char* name() const;

  void init() override;

  void attach(const ArcRotateCameraPtr& camera) override;

  void detach() override;

private:
  /**
   * @brief Sets the flag that indicates if user zooming should stop animation.
   */
  void set_zoomStopsAnimation(bool flag);

  /**
   * @brief Gets the flag that indicates if user zooming should stop animation.
   */
  bool get_zoomStopsAnimation() const;

  /**
   * @brief Sets the default speed at which the camera rotates around the model.
   */
  void set_idleRotationSpeed(float speed);

  /**
   * @brief Gets the default speed at which the camera rotates around the model.
   */
  float get_idleRotationSpeed() const;

  /**
   * @brief Sets the time (in milliseconds) to wait after user interaction
   * before the camera starts rotating.
   */
  void set_idleRotationWaitTime(float time);

  /**
   * @brief Gets the time (milliseconds) to wait after user interaction before
   * the camera starts rotating.
   */
  float get_idleRotationWaitTime() const;

  /**
   * @brief Sets the time (milliseconds) to take to spin up to the full idle
   * rotation speed.
   */
  void set_idleRotationSpinupTime(float time);

  /**
   * @brief Gets the time (milliseconds) to take to spin up to the full idle
   * rotation speed.
   */
  float get_idleRotationSpinupTime() const;

  /**
   * @brief Gets a value indicating if the camera is currently rotating because
   * of this behavior.
   */
  bool get_rotationInProgress() const;

  /**
   * @brief Returns true if user is scrolling.
   * @return true if user is scrolling.
   */
  bool _userIsZooming() const;

  bool _shouldAnimationStopForInteraction();

  /**
   * @brief Applies any current user interaction to the camera. Takes into
   * account maximum alpha rotation.
   */
  void _applyUserInteraction();

  // Tools
  bool _userIsMoving();

public:
  /**
   * Flag that indicates if user zooming should stop animation.
   */
  Property<AutoRotationBehavior, bool> zoomStopsAnimation;

  /**
   * Default speed at which the camera rotates around the model.
   */
  Property<AutoRotationBehavior, float> idleRotationSpeed;

  /**
   *Time (in milliseconds) to wait after user interaction before the camera
   *starts rotating.
   */
  Property<AutoRotationBehavior, float> idleRotationWaitTime;

  /**
   * Time (milliseconds) to take to spin up to the full idle rotation speed.
   */
  Property<AutoRotationBehavior, float> idleRotationSpinupTime;

  /**
   * Value indicating if the camera is currently rotating because of this
   * behavior.
   */
  ReadOnlyProperty<AutoRotationBehavior, bool> rotationInProgress;

private:
  bool _zoomStopsAnimation;
  float _idleRotationSpeed;
  float _idleRotationWaitTime;
  float _idleRotationSpinupTime;

  // Default behavior functions
  Observer<PointerInfoPre>::Ptr _onPrePointerObservableObserver;
  Observer<Camera>::Ptr _onAfterCheckInputsObserver;
  ArcRotateCameraPtr _attachedCamera;
  bool _isPointerDown;
  std::optional<high_res_time_point_t> _lastFrameTime;
  high_res_time_point_t _lastInteractionTime;
  float _cameraRotationSpeed;
  float _lastFrameRadius;

}; // end of class BouncingBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H
