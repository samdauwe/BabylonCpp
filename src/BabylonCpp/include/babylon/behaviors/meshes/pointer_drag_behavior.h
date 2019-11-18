#ifndef BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_H

#include <babylon/babylon_api.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior_options.h>
#include <babylon/core/structs.h>
#include <babylon/culling/ray.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
class PointerInfo;
class Ray;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;
using NodePtr         = std::shared_ptr<Node>;

/**
 * @brief A behavior that when attached to a mesh will allow the mesh to be
 * dragged around the screen based on pointer events.
 */
class BABYLON_SHARED_EXPORT PointerDragBehavior : public Behavior<AbstractMesh> {

public:
  /**
   * @brief Creates a pointer drag behavior that can be attached to a mesh.
   * @param options The drag axis or normal of the plane that will be dragged
   * across. pointerObservableScene can be used to listen to drag events from
   * another scene(eg. if the attached mesh is in an overlay scene).
   */
  PointerDragBehavior(const PointerDragBehaviorOptions& options = PointerDragBehaviorOptions());
  ~PointerDragBehavior() override; // = default

  /**
   * @brief The name of the behavior.
   */
  [[nodiscard]] std::string name() const;

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the drag behavior the passed in mesh.
   * @param ownerNode The mesh that will be dragged around once attached
   * @param predicate Predicate to use for pick filtering
   */
  void attach(const AbstractMeshPtr& ownerNode,
              const std::function<bool(const AbstractMeshPtr& m)>& predicate = nullptr) override;

  /**
   * @brief Force relase the drag action by code.
   */
  void releaseDrag();

  /**
   * @brief Simulates the start of a pointer drag event on the behavior.
   * @param pointerId pointerID of the pointer that should be simulated
   * (Default: Any mouse pointer ID)
   * @param fromRay initial ray of the pointer to be simulated (Default: Ray
   * from camera to attached mesh)
   * @param startPickedPoint picked point of the pointer to be simulated
   * (Default: attached mesh position)
   */
  void startDrag(int pointerId                                  = PointerDragBehavior::_AnyMouseID,
                 const std::optional<Ray>& fromRay              = std::nullopt,
                 const std::optional<Vector3>& startPickedPoint = std::nullopt);

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

private:
  void _startDrag(int pointerId = 1, const std::optional<Ray>& fromRay = std::nullopt,
                  const std::optional<Vector3>& startPickedPoint = std::nullopt);
  void _moveDrag(const Ray& ray);
  std::optional<Vector3> _pickWithRayOnDragPlane(const std::optional<Ray>& ray);

  /**
   * @brief Position the drag plane based on the attached mesh position, for
   * single axis rotate the plane along the axis to face the camera.
   */
  void _updateDragPlanePosition(const Ray& ray, const Vector3& dragPlanePosition);

public:
  /**
   * Abstract mesh the behavior is set on
   */
  AbstractMeshPtr attachedNode;

  /**
   * The maximum tolerated angle between the drag plane and dragging pointer
   * rays to trigger pointer events. Set to 0 to allow any angle (default: 0)
   */
  float maxDragAngle;

  /**
   * Hidden
   */
  bool _useAlternatePickedPointAboveMaxDragAngle;

  /**
   * The id of the pointer that is currently interacting with the behavior (-1
   * when no pointer is active)
   */
  int currentDraggingPointerID;

  /**
   * The last position where the pointer hit the drag plane in world space
   */
  Vector3 lastDragPosition;

  /**
   * If the behavior is currently in a dragging state
   */
  bool dragging;

  /**
   * The distance towards the target drag position to move each frame. This
   * can be useful to avoid jitter. Set this to 1 for no delay. (Default: 0.2)
   */
  float dragDeltaRatio;

  /**
   * If the drag plane orientation should be updated during the dragging
   * (Default: true)
   */
  bool updateDragPlane;

  /**
   *  Fires each time the attached mesh is dragged with the pointer
   *  * delta between last drag position and current drag position in world
   *    space
   *  * dragDistance along the drag axis
   *  * dragPlaneNormal normal of the current drag plane used during the drag
   *  * dragPlanePoint in world space where the drag intersects the drag plane
   */
  Observable<DragMoveEvent> onDragObservable;

  /**
   * Fires each time a drag begins (eg. mouse down on mesh)
   */
  Observable<DragStartOrEndEvent> onDragStartObservable;

  /**
   * Fires each time a drag ends (eg. mouse release after drag)
   */
  Observable<DragStartOrEndEvent> onDragEndObservable;

  /**
   * If the attached mesh should be moved when dragged
   */
  bool moveAttached;

  /**
   *  If the drag behavior will react to drag events (Default: true)
   */
  bool enabled;

  /**
   * If pointer events should start and release the drag (Default: true)
   */
  bool startAndReleaseDragOnPointerEvents;

  /**
   * If camera controls should be detached during the drag
   */
  bool detachCameraControls;

  /**
   * If set, the drag plane/axis will be rotated based on the attached mesh's
   * world rotation (Default: true)
   */
  bool useObjectOrienationForDragging;

  /**
   * Predicate to determine if it is valid to move the object to a new position
   * when it is moved
   */
  std::function<bool(const Vector3& targetPosition)> validateDrag;

  static std::unique_ptr<Scene> _planeScene;

private:
  static int _AnyMouseID;
  MeshPtr _dragPlane;
  Scene* _scene;
  Observer<PointerInfo>::Ptr _pointerObserver;
  Observer<Scene>::Ptr _beforeRenderObserver;
  float _useAlternatePickedPointAboveMaxDragAngleDragSpeed;
  int _draggingID;
  // Debug mode will display drag planes to help visualize behavior
  bool _debugMode;
  bool _moving;

  /**
   * The drag axis or normal of the plane that will be dragged across.
   * pointerObservableScene can be used to listen to drag events from another
   * scene(eg. if the attached mesh is in an overlay scene)
   */
  PointerDragBehaviorOptions _options;

  Vector3 _tmpVector;
  Vector3 _alternatePickedPoint;
  Vector3 _worldDragAxis;
  Vector3 _targetPosition;
  ICanvas* _attachedElement;
  Ray _startDragRay;
  std::unordered_map<int, Ray> _lastPointerRay;
  Vector3 _dragDelta;

  // Variables to avoid instantiation in the method _updateDragPlanePosition
  Vector3 _pointA;
  Vector3 _pointB;
  Vector3 _pointC;
  Vector3 _lineA;
  Vector3 _lineB;
  Vector3 _localAxis;
  Vector3 _lookAt;

}; // end of class PointerDragBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESHES_POINTER_DRAG_BEHAVIOR_H
