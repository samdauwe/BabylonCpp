#ifndef BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H
#define BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H

#include <babylon/babylon_global.h>

#include <babylon/behaviors/behavior.h>
#include <babylon/core/nullable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT AutoRotationBehavior
  : public Behavior<ArcRotateCamera> {

public:
  AutoRotationBehavior();
  virtual ~AutoRotationBehavior();

  const char* name() const;

  /**
   * @brief Sets the flag that indicates if user zooming should stop animation.
   */
  void setZoomStopsAnimation(bool flag);

  /**
   * @brief Gets the flag that indicates if user zooming should stop animation.
   */
  bool zoomStopsAnimation() const;

  /**
   * @brief Sets the default speed at which the camera rotates around the model.
   */
  void setIdleRotationSpeed(float speed);

  /**
   * @brief Gets the default speed at which the camera rotates around the model.
   */
  float idleRotationSpeed() const;

  /**
   * @brief Sets the time (in milliseconds) to wait after user interaction
   * before the camera starts rotating.
   */
  void setIdleRotationWaitTime(float time);

  /**
   * @brief Gets the time (milliseconds) to wait after user interaction before
   * the camera starts rotating.
   */
  float idleRotationWaitTime() const;

  /**
   * @brief Sets the time (milliseconds) to take to spin up to the full idle
   * rotation speed.
   */
  void setIdleRotationSpinupTime(float time);

  /**
   * @brief Gets the time (milliseconds) to take to spin up to the full idle
   * rotation speed.
   */
  float idleRotationSpinupTime() const;

  /**
   * @brief Gets a value indicating if the camera is currently rotating because
   * of this behavior.
   */
  bool rotationInProgress() const;

  void attach(ArcRotateCamera* camera) override;
  void detach() override;

private:
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

private:
  bool _zoomStopsAnimation;
  float _idleRotationSpeed;
  float _idleRotationWaitTime;
  float _idleRotationSpinupTime;

  // Default behavior functions
  Observer<PointerInfoPre> _onPrePointerObservableObserver;
  Observer<Camera> _onAfterCheckInputsObserver;
  ArcRotateCamera* _attachedCamera;
  bool _isPointerDown = false;
  Nullable<high_res_time_point_t> _lastFrameTime;
  high_res_time_point_t _lastInteractionTime;
  float _cameraRotationSpeed;
  float _lastFrameRadius;

}; // end of class BouncingBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_CAMERAS_AUTO_ROTATION_BEHAVIOR_H
