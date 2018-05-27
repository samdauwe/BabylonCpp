#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/arc_rotate_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/nullable.h>
#include <babylon/core/structs.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ArcRotateCamera : public TargetCamera {

public:
  template <typename... Ts>
  static ArcRotateCamera* New(Ts&&... args)
  {
    auto camera = new ArcRotateCamera(::std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~ArcRotateCamera() override;

  virtual IReflect::Type type() const override;

  // Cache
  void _initCache() override;
  void _updateCache(bool ignoreParentClass) override;

  // State
  Camera& storeState() override;

  /**
   * Restored camera state. You must call storeState() first
   */
  bool _restoreStateValues() override;

  // Synchronized
  bool _isSynchronizedViewMatrix();

  // Methods
  void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                     bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton
                     = MouseButtonType::RIGHT) override;
  void detachControl(ICanvas* canvas) override;
  void _checkInputs() override;
  void rebuildAnglesAndRadius();
  void setPosition(const Vector3& position);
  void setTarget(AbstractMesh* target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  void setTarget(const Vector3& target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  Matrix _getViewMatrix() override;
  void zoomOn(const vector_t<AbstractMesh*> meshes,
              bool doNotUpdateMaxZ = false);
  void focusOn(const MinMaxDistance& meshesOrMinMaxVectorAndDistance,
               bool doNotUpdateMaxZ = false);
  Camera* createRigCamera(const string_t& name, int cameraIndex) override;
  void _updateRigCameras() override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;
  const string_t getClassName() const override;
  Json::object serialize() const override;

protected:
  ArcRotateCamera(const string_t& name, float alpha, float beta, float radius,
                  const Nullable<Vector3>& target, Scene* scene,
                  bool setActiveOnSceneIfNoneActive = true);

  Vector3 _getTargetPosition();
  void _checkLimits();
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

private:
  Vector3& get_target();
  void set_target(const Vector3& value);
  unique_ptr_t<BouncingBehavior>& get_bouncingBehavior();
  bool get_useBouncingBehavior() const;
  void set_useBouncingBehavior(bool value);
  unique_ptr_t<FramingBehavior>& get_framingBehavior();
  bool get_useFramingBehavior() const;
  void set_useFramingBehavior(bool value);
  unique_ptr_t<AutoRotationBehavior>& get_autoRotationBehavior();
  bool get_useAutoRotationBehavior() const;
  void set_useAutoRotationBehavior(bool value);

public:
  float alpha;
  float beta;
  float radius;
  Property<ArcRotateCamera, Vector3> target;
  float inertialAlphaOffset;
  float inertialBetaOffset;
  float inertialRadiusOffset;
  float lowerAlphaLimit;
  float upperAlphaLimit;
  float lowerBetaLimit;
  float upperBetaLimit;
  float lowerRadiusLimit;
  float upperRadiusLimit;
  float inertialPanningX;
  float inertialPanningY;
  float pinchToPanMaxDistance;
  Nullable<float> panningDistanceLimit;
  Vector3 panningOriginTarget;
  float panningInertia;
  float zoomOnFactor;
  float wheelPrecision;
  Vector2 targetScreenOffset;
  bool allowUpsideDown;
  bool _useCtrlForPanning;
  MouseButtonType _panningMouseButton;
  unique_ptr_t<ArcRotateCameraInputsManager> inputs;
  // Panning
  unique_ptr_t<Vector3> panningAxis;
  // Behaviors
  ReadOnlyProperty<ArcRotateCamera, unique_ptr_t<BouncingBehavior>>
    bouncingBehavior;
  Property<ArcRotateCamera, bool> useBouncingBehavior;
  ReadOnlyProperty<ArcRotateCamera, unique_ptr_t<FramingBehavior>>
    framingBehavior;
  Property<ArcRotateCamera, bool> useFramingBehavior;
  ReadOnlyProperty<ArcRotateCamera, unique_ptr_t<AutoRotationBehavior>>
    autoRotationBehavior;
  Property<ArcRotateCamera, bool> useAutoRotationBehavior;
  // Behaviors
  Observable<AbstractMesh> onMeshTargetChangedObservable;
  // Collisions
  ::std::function<void(AbstractMesh* collidedMesh)> onCollide;
  bool checkCollisions;
  unique_ptr_t<Vector3> collisionRadius;

protected:
  Vector3 _target;
  AbstractMesh* _targetHost;
  // Panning
  unique_ptr_t<Vector3> _localDirection;
  Vector3 _transformedDirection;
  // Collisions
  unique_ptr_t<Collider> _collider;
  Vector3 _previousPosition;
  Vector3 _collisionVelocity;
  Vector3 _newPosition;
  float _previousAlpha;
  float _previousBeta;
  float _previousRadius;
  // due to async collision inspection
  bool _collisionTriggered;
  unique_ptr_t<Vector3> _targetBoundingCenter;
  // Behaviors
  unique_ptr_t<BouncingBehavior> _bouncingBehavior;
  unique_ptr_t<FramingBehavior> _framingBehavior;
  unique_ptr_t<AutoRotationBehavior> _autoRotationBehavior;

private:
  Vector3 _computationVector;

  /**
   * Store current camera state (fov, position, etc..)
   */
  float _storedAlpha;
  float _storedBeta;
  float _storedRadius;
  Vector3 _storedTarget;

}; // end of class ArcRotateCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
