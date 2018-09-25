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

class BABYLON_SHARED_EXPORT ArcRotateCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static ArcRotateCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<ArcRotateCamera>(
      new ArcRotateCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~ArcRotateCamera() override;

  virtual IReflect::Type type() const override;

  // Cache
  /** Hidden */
  void _initCache() override;
  /** Hidden */
  void _updateCache(bool ignoreParentClass) override;

  // State
  Camera& storeState() override;

  /**
   * Restored camera state. You must call storeState() first
   * Hidden
   */
  bool _restoreStateValues() override;

  // Synchronized
  /** Hidden */
  bool _isSynchronizedViewMatrix() override;

  // Methods
  void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                     bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton
                     = MouseButtonType::RIGHT) override;
  void detachControl(ICanvas* canvas) override;
  /** Hidden */
  void _checkInputs() override;
  void rebuildAnglesAndRadius();
  void setPosition(const Vector3& position);
  void setTarget(AbstractMesh* target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  void setTarget(const Vector3& target, bool toBoundingCenter = false,
                 bool allowSamePosition = false);
  /** Hidden */
  Matrix _getViewMatrix() override;
  void zoomOn(const std::vector<AbstractMeshPtr> meshes,
              bool doNotUpdateMaxZ = false);
  void focusOn(const MinMaxDistance& meshesOrMinMaxVectorAndDistance,
               bool doNotUpdateMaxZ = false);
  /** Hidden */
  CameraPtr createRigCamera(const std::string& name, int cameraIndex) override;
  void _updateRigCameras() override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;
  const std::string getClassName() const override;
  Json::object serialize() const override;

protected:
  ArcRotateCamera(const std::string& name, float alpha, float beta, float radius,
                  const std::optional<Vector3>& target, Scene* scene,
                  bool setActiveOnSceneIfNoneActive = true);

  Vector3 _getTargetPosition();
  void _checkLimits();
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

private:
  Vector3& get_target();
  void set_target(const Vector3& value);
  std::unique_ptr<BouncingBehavior>& get_bouncingBehavior();
  bool get_useBouncingBehavior() const;
  void set_useBouncingBehavior(bool value);
  std::unique_ptr<FramingBehavior>& get_framingBehavior();
  bool get_useFramingBehavior() const;
  void set_useFramingBehavior(bool value);
  std::unique_ptr<AutoRotationBehavior>& get_autoRotationBehavior();
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
  std::optional<float> lowerAlphaLimit;
  std::optional<float> upperAlphaLimit;
  float lowerBetaLimit;
  float upperBetaLimit;
  std::optional<float> lowerRadiusLimit;
  std::optional<float> upperRadiusLimit;
  float inertialPanningX;
  float inertialPanningY;
  float pinchToPanMaxDistance;
  std::optional<float> panningDistanceLimit;
  Vector3 panningOriginTarget;
  float panningInertia;
  float zoomOnFactor;
  float wheelPrecision;
  Vector2 targetScreenOffset;
  bool allowUpsideDown;
  /** Hidden */
  bool _useCtrlForPanning;
  /** Hidden */
  MouseButtonType _panningMouseButton;
  std::unique_ptr<ArcRotateCameraInputsManager> inputs;
  // Panning
  std::unique_ptr<Vector3> panningAxis;
  // Behaviors
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<BouncingBehavior>>
    bouncingBehavior;
  Property<ArcRotateCamera, bool> useBouncingBehavior;
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<FramingBehavior>>
    framingBehavior;
  Property<ArcRotateCamera, bool> useFramingBehavior;
  ReadOnlyProperty<ArcRotateCamera, std::unique_ptr<AutoRotationBehavior>>
    autoRotationBehavior;
  Property<ArcRotateCamera, bool> useAutoRotationBehavior;
  // Behaviors
  Observable<AbstractMesh> onMeshTargetChangedObservable;
  // Collisions
  std::function<void(AbstractMesh* collidedMesh)> onCollide;
  bool checkCollisions;
  std::unique_ptr<Vector3> collisionRadius;

protected:
  Vector3 _target;
  AbstractMesh* _targetHost;
  // Panning
  std::unique_ptr<Vector3> _localDirection;
  Vector3 _transformedDirection;
  // Collisions
  std::unique_ptr<Collider> _collider;
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

  /**
   * Store current camera state (fov, position, etc..)
   */
  float _storedAlpha;
  float _storedBeta;
  float _storedRadius;
  Vector3 _storedTarget;

  static bool NodeConstructorAdded;

}; // end of class ArcRotateCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_H
