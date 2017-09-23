#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/arc_rotate_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
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
  ~ArcRotateCamera();

  virtual IReflect::Type type() const override;

  // Cache
  void _initCache() override;
  void _updateCache(bool ignoreParentClass) override;
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
  Vector3& target();
  const Vector3& target() const;
  void setTarget(AbstractMesh* target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  void setTarget(const Vector3& target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  BouncingBehavior* bouncingBehavior() const;
  bool useBouncingBehavior() const;
  void setUseBouncingBehavior(bool value);
  FramingBehavior* framingBehavior() const;
  bool useFramingBehavior() const;
  void setUseFramingBehavior(bool value);
  AutoRotationBehavior* autoRotationBehavior() const;
  bool useAutoRotationBehavior() const;
  void setUseAutoRotationBehavior(bool value);
  Matrix _getViewMatrix() override;
  void zoomOn(const vector_t<AbstractMesh*> meshes,
              bool doNotUpdateMaxZ = false);
  void focusOn(const MinMaxDistance& meshesOrMinMaxVectorAndDistance,
               bool doNotUpdateMaxZ = false);
  Camera* createRigCamera(const string_t& name, int cameraIndex) override;
  void _updateRigCameras() override;
  void dispose(bool doNotRecurse = false) override;
  const char* getClassName() const override;
  Json::object serialize() const override;

protected:
  ArcRotateCamera(const string_t& name, float alpha, float beta, float radius,
                  Scene* scene);
  ArcRotateCamera(const string_t& name, float alpha, float beta, float radius,
                  const Vector3& target, Scene* scene);

  Vector3 _getTargetPosition();
  void _checkLimits();
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

public:
  float alpha;
  float beta;
  float radius;
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
  float panningInertia;
  float zoomOnFactor;
  float wheelPrecision;
  Vector2 targetScreenOffset;
  bool allowUpsideDown;
  Matrix _viewMatrix;
  bool _useCtrlForPanning;
  MouseButtonType _panningMouseButton;
  unique_ptr_t<ArcRotateCameraInputsManager> inputs;
  ::std::function<void()> _reset;
  // Panning
  unique_ptr_t<Vector3> panningAxis;
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

}; // end of class ArcRotateCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
