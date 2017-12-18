#ifndef BABYLON_BEHAVIORS_CAMERAS_FRAMING_BEHAVIOR_H
#define BABYLON_BEHAVIORS_CAMERAS_FRAMING_BEHAVIOR_H

#include <babylon/babylon_global.h>

#include <babylon/animations/easing/exponential_ease.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/core/nullable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FramingBehavior : public Behavior<ArcRotateCamera> {

public:
  // Statics
  /**
   * The easing function used by animations
   */
  static ExponentialEase _EasingFunction;

  /**
   * The easing mode used by animations
   */
  static unsigned int EasingMode;

  /**
   * The camera can move all the way towards the mesh.
   */
  static constexpr unsigned int IgnoreBoundsSizeMode = 0;

  /**
   * The camera is not allowed to zoom closer to the mesh than the point at
   * which the adjusted bounding sphere touches the frustum sides
   */
  static constexpr unsigned int FitFrustumSidesMode = 1;

public:
  FramingBehavior();
  virtual ~FramingBehavior();

  const char* name() const;

  /**
   * @brief Sets the current mode used by the behavior
   */
  void setMode(unsigned int mode);

  /**
   * @brief Gets current mode used by the behavior.
   */
  unsigned int mode() const;

  /**
   * @brief Sets the scale applied to the radius (1 by default)
   */
  void setRadiusScale(float radius);

  /**
   * @brief Gets the scale applied to the radius
   */
  float radiusScale() const;

  /**
   * @brief Sets the scale to apply on Y axis to position camera focus. 0.5 by
   * default which means the center of the bounding box.
   */
  void setPositionScale(float scale);

  /**
   * @brief Gets the scale to apply on Y axis to position camera focus. 0.5 by
   * default which means the center of the bounding box.
   */
  float positionScale() const;

  /**
   * @brief Sets the angle above/below the horizontal plane to return to when
   * the return to default elevation idle behaviour is triggered, in radians.
   */
  void setDefaultElevation(float elevation);

  /**
   * @brief Gets the angle above/below the horizontal plane to return to when
   * the return to default elevation idle behaviour is triggered, in radians.
   */
  float defaultElevation() const;

  /**
   * @brief Sets the time (in milliseconds) taken to return to the default beta
   * position.
   * Negative value indicates camera should not return to default.
   */
  void setElevationReturnTime(float speed);

  /**
   * @brief Gets the time (in milliseconds) taken to return to the default beta
   * position.
   * Negative value indicates camera should not return to default.
   */
  float elevationReturnTime() const;

  /**
   * @brief Sets the delay (in milliseconds) taken before the camera returns to
   * the default beta position.
   */
  void setElevationReturnWaitTime(float time);

  /**
   * @brief Gets the delay (in milliseconds) taken before the camera returns to
   * the default beta position.
   */
  float elevationReturnWaitTime() const;

  /**
   * @brief Sets the flag that indicates if user zooming should stop animation.
   */
  void setZoomStopsAnimation(bool flag);

  /**
   * @brief Gets the flag that indicates if user zooming should stop animation.
   */
  bool zoomStopsAnimation() const;

  /**
   * Sets the transition time when framing the mesh, in milliseconds
   */
  void setFramingTime(float time);

  /**
   * @brief Gets the transition time when framing the mesh, in milliseconds
   */
  float framingTime() const;

  void init() override;

  void attach(ArcRotateCamera* camera) override;

  void detach() override;

  /**
   * @brief Targets the given mesh and updates zoom level accordingly.
   * @param mesh  The mesh to target.
   * @param radius Optional. If a cached radius position already exists,
   * overrides default.
   * @param framingPositionY Position on mesh to center camera focus where 0
   * corresponds bottom of its bounding box and 1, the top
   * @param focusOnOriginXZ Determines if the camera should focus on 0 in the X
   * and Z axis instead of the mesh
   * @param onAnimationEnd Callback triggered at the end of the framing
   * animation
   */
  void zoomOnMesh(AbstractMesh* mesh, bool focusOnOriginXZ = false,
                  const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Targets the given mesh with its children and updates zoom level
   * accordingly.
   * @param mesh  The mesh to target.
   * @param radius Optional. If a cached radius position already exists,
   * overrides default.
   * @param framingPositionY Position on mesh to center camera focus where 0
   * corresponds bottom of its bounding box and 1, the top
   * @param focusOnOriginXZ Determines if the camera should focus on 0 in the X
   * and Z axis instead of the mesh
   * @param onAnimationEnd Callback triggered at the end of the framing
   * animation
   */
  void zoomOnMeshHierarchy(AbstractMesh* mesh, bool focusOnOriginXZ = false,
                           const ::std::function<void()>& onAnimationEnd
                           = nullptr);

  /**
   * @brief Targets the given meshes with their children and updates zoom level
   * accordingly.
   * @param meshes  The mesh to target.
   * @param radius Optional. If a cached radius position already exists,
   * overrides default.
   * @param framingPositionY Position on mesh to center camera focus where 0
   * corresponds bottom of its bounding box and 1, the top
   * @param focusOnOriginXZ Determines if the camera should focus on 0 in the X
   * and Z axis instead of the mesh
   * @param onAnimationEnd Callback triggered at the end of the framing
   * animation
   */
  void zoomOnMeshesHierarchy(const vector_t<AbstractMesh*>& meshes,
                             bool focusOnOriginXZ = false,
                             const ::std::function<void()>& onAnimationEnd
                             = nullptr);

  /**
   * @brief Targets the given mesh and updates zoom level accordingly.
   * @param mesh  The mesh to target.
   * @param radius Optional. If a cached radius position already exists,
   * overrides default.
   * @param framingPositionY Position on mesh to center camera focus where 0
   * corresponds bottom of its bounding box and 1, the top
   * @param focusOnOriginXZ Determines if the camera should focus on 0 in the X
   * and Z axis instead of the mesh
   * @param onAnimationEnd Callback triggered at the end of the framing
   * animation
   */
  void
  zoomOnBoundingInfo(const Vector3& minimumWorld, const Vector3& maximumWorld,
                     bool focusOnOriginXZ                          = false,
                     const ::std::function<void()>& onAnimationEnd = nullptr);

protected:
  /**
   * @brief Calculates the lowest radius for the camera based on the bounding
   * box of the mesh.
   * @param mesh The mesh on which to base the calculation. mesh boundingInfo
   * used to estimate necessary frustum width.
   * @return The minimum distance from the primary mesh's center point at which
   * the camera must be kept in order to fully enclose the mesh in the viewing
   * frustum.
   */
  float
  _calculateLowerRadiusFromModelBoundingSphere(const Vector3& minimumWorld,
                                               const Vector3& maximumWorld);

private:
  /**
   * @brief Keeps the camera above the ground plane. If the user pulls the
   * camera below the ground plane, the camera is automatically returned to its
   * default position (expected to be above ground plane).
   */
  void _maintainCameraAboveGround();

  /**
   * @brief Returns the frustum slope based on the canvas ratio and camera FOV
   * @returns The frustum slope represented as a Vector2 with X and Y slopes
   */
  Vector2 _getFrustumSlope() const;

  /**
   * @brief Removes all animation locks. Allows new animations to be added to
   * any of the arcCamera properties.
   */
  void _clearAnimationLocks();

  /**
   * @brief Applies any current user interaction to the camera. Takes into
   * account
   * maximum alpha rotation.
   */
  void _applyUserInteraction();

  /**
   * @brief Stops and removes all animations that have been applied to the
   * camera
   */
  void stopAllAnimations();

  /**
   * @brief Gets a value indicating if the user is moving the camera
   */
  bool isUserIsMoving() const;

private:
  unsigned int _mode;
  float _radiusScale;
  float _positionScale;
  float _defaultElevation;
  float _elevationReturnTime;
  float _elevationReturnWaitTime;
  bool _zoomStopsAnimation;
  float _framingTime;

  // Default behavior functions
  Observer<PointerInfoPre>::Ptr _onPrePointerObservableObserver;
  Observer<Camera>::Ptr _onAfterCheckInputsObserver;
  Observer<AbstractMesh>::Ptr _onMeshTargetChangedObserver;
  ArcRotateCamera* _attachedCamera;
  bool _isPointerDown;
  Nullable<high_res_time_point_t> _lastFrameTime;
  high_res_time_point_t _lastInteractionTime;

  // Framing control
  vector_t<Animatable*> _animatables;
  bool _betaIsAnimating;
  Animation* _betaTransition;
  Animation* _radiusTransition;
  Animation* _vectorTransition;

}; // end of class FramingBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_CAMERAS_FRAMING_BEHAVIOR_H
