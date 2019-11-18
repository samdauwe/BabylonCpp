#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/structs.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class ArcRotateCamera;
class AutoRotationBehavior;
class BouncingBehavior;
class Collider;
class FramingBehavior;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using ColliderPtr        = std::shared_ptr<Collider>;

/**
 * @brief This represents an orbital type of camera.
 *
 * This camera always points towards a given target position and can be rotated
 * around that target with the target as the centre of rotation. It can be
 * controlled with cursors and mouse, or with touch events. Think of this camera
 * as one orbiting its target position, or more imaginatively as a spy satellite
 * orbiting the earth. Its position relative to the target (earth) can be set by
 * three parameters, alpha (radians) the longitudinal rotation, beta (radians)
 * the latitudinal rotation and radius the distance from the target position.
 * @see http://doc.babylonjs.com/babylon101/cameras#arc-rotate-camera
 */
class BABYLON_SHARED_EXPORT ArcRotateCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static ArcRotateCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<ArcRotateCamera>(new ArcRotateCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~ArcRotateCamera() override; // = default

  Type type() const override;

  /**
   * @brief Sets the Y-up to camera up-vector rotation matrix, and the up-vector
   * to Y-up rotation matrix.
   */
  void setMatUp();

  /**
   * @brief Hidden
   */
  void _initCache() override;

  /**
   * @brief Hidden
   */
  void _updateCache(bool ignoreParentClass) override;

  /**
   * @brief Stores the current state of the camera (alpha, beta, radius and
   * target).
   * @returns the camera itself
   */
  Camera& storeState() override;

  /**
   * @brief Restored camera state. You must call storeState() first
   * Hidden
   */
  bool _restoreStateValues() override;

  /**
   * @brief Hidden
   */
  bool _isSynchronizedViewMatrix() override;

  /**
   * @brief Attached controls to the current camera.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   * @param useCtrlForPanning  Defines whether ctrl is used for paning within
   * the controls
   * @param panningMouseButton Defines whether panning is allowed through mouse
   * click button
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false, bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton = MouseButtonType::RIGHT) override;

  /**
   * @brief Detach the current controls from the camera.
   * The camera will stop reacting to inputs.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Rebuilds angles (alpha, beta) and radius from the give position and
   * target.
   */
  void rebuildAnglesAndRadius();

  /**
   * @brief Use a position to define the current camera related information like
   * aplha, beta and radius
   * @param position Defines the position to set the camera at
   */
  void setPosition(const Vector3& position);

  /**
   * @brief Defines the target the camera should look at.
   * This will automatically adapt alpha beta and radius to fit within the new
   * target.
   * @param target Defines the new target as a Vector or a mesh
   * @param toBoundingCenter In case of a mesh target, defines wether to target
   * the mesh position or its bounding information center
   * @param allowSamePosition If false, prevents reapplying the new computed
   * position if it is identical to the current one (optim)
   */
  void setTarget(const std::variant<AbstractMeshPtr, Vector3>& target,
                 bool toBoundingCenter = false, bool allowSamePosition = false);

  /**
   * @brief Hidden
   */
  Matrix _getViewMatrix() override;

  /**
   * @brief Zooms on a mesh to be at the min distance where we could see it
   * fully in the current viewport.
   * @param meshes Defines the mesh to zoom on
   * @param doNotUpdateMaxZ Defines whether or not maxZ should be updated whilst
   * zooming on the mesh (this can happen if the mesh is big and the maxradius
   * pretty small for instance)
   */
  void zoomOn(const std::vector<AbstractMeshPtr> meshes, bool doNotUpdateMaxZ = false);

  /**
   * @brief Focus on a mesh or a bounding box. This adapts the target and
   * maxRadius if necessary but does not update the current radius. The target
   * will be changed but the radius
   * @param meshesOrMinMaxVectorAndDistance Defines the mesh or bounding info to
   * focus on
   * @param doNotUpdateMaxZ Defines whether or not maxZ should be updated whilst
   * zooming on the mesh (this can happen if the mesh is big and the maxradius
   * pretty small for instance)
   */
  void focusOn(const MinMaxDistance& meshesOrMinMaxVectorAndDistance, bool doNotUpdateMaxZ = false);

  /**
   * @brief Hidden
   */
  CameraPtr createRigCamera(const std::string& name, int cameraIndex) override;

  /**
   * @brief Hidden
   */
  void _updateRigCameras() override;

  /**
   * @brief Destroy the camera and release the current resources hold by it.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  std::string getClassName() const override;

  json serialize() const override;

protected:
  /**
   * @brief Instantiates a new ArcRotateCamera in a given scene.
   * @param name Defines the name of the camera
   * @param alpha Defines the camera rotation along the logitudinal axis
   * @param beta Defines the camera rotation along the latitudinal axis
   * @param radius Defines the camera distance from its target
   * @param target Defines the camera target
   * @param scene Defines the scene the camera belongs to
   * @param setActiveOnSceneIfNoneActive Defines wheter the camera should be
   * marked as active if not other active cameras have been defined
   */
  ArcRotateCamera(const std::string& name, float alpha, float beta, float radius,
                  const std::optional<Vector3>& target, Scene* scene,
                  bool setActiveOnSceneIfNoneActive = true);

  Vector3 _getTargetPosition();
  void _checkLimits();
  void _onCollisionPositionChange(size_t collisionId, Vector3& newPosition,
                                  const AbstractMeshPtr& collidedMesh = nullptr);

  /**
   * @brief Defines the target point of the camera.
   * The camera looks towards it form the radius distance.
   */
  Vector3& get_target();
  void set_target(const Vector3& value);

  /**
   * @brief The vector the camera should consider as up. (default is Vector3(0,
   * 1, 0) as returned by Vector3.Up()) Setting this will copy the given vector
   * to the camera's upVector, and set rotation matrices to and from Y up. DO
   * NOT set the up vector using copyFrom or copyFromFloats, as this bypasses
   * setting the above matrices.
   */
  Vector3& get_upVector();
  void set_upVector(const Vector3& vec);

  /**
   * @brief Gets the bouncing behavior of the camera if it has been enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#bouncing-behavior
   */
  std::unique_ptr<BouncingBehavior>& get_bouncingBehavior();

  /**
   * @brief Defines if the bouncing behavior of the camera is enabled on the
   * camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#bouncing-behavior
   */
  bool get_useBouncingBehavior() const;
  void set_useBouncingBehavior(bool value);

  /**
   * @brief Gets the framing behavior of the camera if it has been enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#framing-behavior
   */
  std::unique_ptr<FramingBehavior>& get_framingBehavior();

  /**
   * @brief Defines if the framing behavior of the camera is enabled on the
   * camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#framing-behavior
   */
  bool get_useFramingBehavior() const;
  void set_useFramingBehavior(bool value);

  /**
   * @brief Gets the auto rotation behavior of the camera if it has been
   * enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#autorotation-behavior
   */
  std::unique_ptr<AutoRotationBehavior>& get_autoRotationBehavior();

  /**
   * @brief Defines if the auto rotation behavior of the camera is enabled on
   * the camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#autorotation-behavior
   */
  bool get_useAutoRotationBehavior() const;
  void set_useAutoRotationBehavior(bool value);

public:
  /**
   * Defines the rotation angle of the camera along the longitudinal axis.
   */
  float alpha;

  /**
   * Defines the rotation angle of the camera along the latitudinal axis.
   */
  float beta;

  /**
   * Defines the radius of the camera from it s target point.
   */
  float radius;

  /**
   * Defines the target point of the camera.
   * The camera looks towards it form the radius distance.
   */
  Property<ArcRotateCamera, Vector3> target;

  /**
   * The vector the camera should consider as up. (default is Vector3(0, 1, 0)
   * as returned by Vector3.Up()) Setting this will copy the given vector to the
   * camera's upVector, and set rotation matrices to and from Y up. DO NOT set
   * the up vector using copyFrom or copyFromFloats, as this bypasses setting
   * the above matrices.
   */
  Property<ArcRotateCamera, Vector3> upVector;

  /**
   * Current inertia value on the longitudinal axis.
   * The bigger this number the longer it will take for the camera to stop.
   */
  float inertialAlphaOffset;

  /**
   * Current inertia value on the latitudinal axis.
   * The bigger this number the longer it will take for the camera to stop.
   */
  float inertialBetaOffset;

  /**
   * Current inertia value on the radius axis.
   * The bigger this number the longer it will take for the camera to stop.
   */
  float inertialRadiusOffset;

  /**
   * Minimum allowed angle on the longitudinal axis.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> lowerAlphaLimit;

  /**
   * Maximum allowed angle on the longitudinal axis.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> upperAlphaLimit;

  /**
   * Minimum allowed angle on the latitudinal axis.
   * This can help limiting how the Camera is able to move in the scene.
   */
  float lowerBetaLimit;

  /**
   * Maximum allowed angle on the latitudinal axis.
   * This can help limiting how the Camera is able to move in the scene.
   */
  float upperBetaLimit;

  /**
   * Minimum allowed distance of the camera to the target (The camera can not
   * get closer). This can help limiting how the Camera is able to move in the
   * scene.
   */
  std::optional<float> lowerRadiusLimit;

  /**
   * Maximum allowed distance of the camera to the target (The camera can not
   * get further). This can help limiting how the Camera is able to move in the
   * scene.
   */
  std::optional<float> upperRadiusLimit;

  /**
   * Defines the current inertia value used during panning of the camera along
   * the X axis.
   */
  float inertialPanningX;

  /**
   * Defines the current inertia value used during panning of the camera along
   * the Y axis.
   */
  float inertialPanningY;

  /**
   * Defines the distance used to consider the camera in pan mode vs pinch/zoom.
   * Basically if your fingers moves away from more than this distance you will
   * be considered in pinch mode.
   */
  float pinchToPanMaxDistance;

  /**
   * Defines the maximum distance the camera can pan.
   * This could help keeping the cammera always in your scene.
   */
  std::optional<float> panningDistanceLimit;

  /**
   * Defines the target of the camera before paning.
   */
  Vector3 panningOriginTarget;

  /**
   * Defines the value of the inertia used during panning.
   * 0 would mean stop inertia and one would mean no decelleration at all.
   */
  float panningInertia;

  /**
   * Defines how much the radius should be scaled while zomming on a particular
   * mesh (through the zoomOn function)
   */
  float zoomOnFactor;

  /**
   * Gets or Set the mouse wheel precision or how fast is the camera zooming.
   */
  float wheelPrecision;

  /**
   * Defines a screen offset for the camera position.
   */
  Vector2 targetScreenOffset;

  /**
   * Allows the camera to be completely reversed.
   * If false the camera can not arrive upside down.
   */
  bool allowUpsideDown;

  /**
   * Define if double tap/click is used to restore the previously saved state of
   * the camera.
   */
  bool useInputToRestoreState;

  /**
   * Hidden
   */
  bool _useCtrlForPanning;

  /**
   * Hidden
   */
  MouseButtonType _panningMouseButton;

  /**
   * Defines the input associated to the camera.
   */
  std::unique_ptr<ArcRotateCameraInputsManager> inputs;

  /**
   * Defines the allowed panning axis.
   */
  std::unique_ptr<Vector3> panningAxis;

  /**
   * Gets the bouncing behavior of the camera if it has been enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#bouncing-behavior
   */
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<BouncingBehavior>> bouncingBehavior;

  /**
   * Defines if the bouncing behavior of the camera is enabled on the camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#bouncing-behavior
   */
  Property<ArcRotateCamera, bool> useBouncingBehavior;

  /**
   * Gets the framing behavior of the camera if it has been enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#framing-behavior
   */
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<FramingBehavior>> framingBehavior;

  /**
   * Defines if the framing behavior of the camera is enabled on the camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#framing-behavior
   */
  Property<ArcRotateCamera, bool> useFramingBehavior;

  /**
   * Gets the auto rotation behavior of the camera if it has been enabled.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#autorotation-behavior
   */
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<AutoRotationBehavior>> autoRotationBehavior;

  /**
   * Defines if the auto rotation behavior of the camera is enabled on the
   * camera.
   * @see http://doc.babylonjs.com/how_to/camera_behaviors#autorotation-behavior
   */
  Property<ArcRotateCamera, bool> useAutoRotationBehavior;

  /**
   * Observable triggered when the mesh target has been changed on the camera.
   */
  Observable<AbstractMesh> onMeshTargetChangedObservable;

  /**
   * Event raised when the camera is colliding with a mesh.
   */
  std::function<void(const AbstractMeshPtr& collidedMesh)> onCollide;

  /**
   * Defines whether the camera should check collision with the objects oh the
   * scene.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#how-can-i-do-this
   */
  bool checkCollisions;

  /**
   * Defines the collision radius of the camera.
   * This simulates a sphere around the camera.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#arcrotatecamera
   */
  std::unique_ptr<Vector3> collisionRadius;

protected:
  Vector3 _target;
  AbstractMeshPtr _targetHost;
  Vector3 _upVector;
  Matrix _upToYMatrix;
  Matrix _YToUpMatrix;
  // Panning
  std::unique_ptr<Vector3> _localDirection;
  Vector3 _transformedDirection;
  // Collisions
  ColliderPtr _collider;
  Vector3 _previousPosition;
  Vector3 _collisionVelocity;
  Vector3 _newPosition;
  float _previousAlpha;
  float _previousBeta;
  float _previousRadius;
  // due to async collision inspection
  bool _collisionTriggered;
  std::unique_ptr<Vector3> _targetBoundingCenter;
  // Behaviors
  std::unique_ptr<BouncingBehavior> _bouncingBehavior;
  std::unique_ptr<FramingBehavior> _framingBehavior;
  std::unique_ptr<AutoRotationBehavior> _autoRotationBehavior;

private:
  Vector3 _computationVector;
  float _storedAlpha;
  float _storedBeta;
  float _storedRadius;
  Vector3 _storedTarget;
  Vector2 _storedTargetScreenOffset;

  static bool NodeConstructorAdded;

}; // end of class ArcRotateCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
