#ifndef BABYLON_BEHAVIORS_MESH_SIX_DOF_DRAG_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESH_SIX_DOF_DRAG_BEHAVIOR_H

#include <babylon/babylon_api.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/core/structs.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
class PointerInfo;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;

/**
 * @brief A behavior that when attached to a mesh will allow the mesh to be
 * dragged around based on directions and origin of the pointer's ray.
 */
class BABYLON_SHARED_EXPORT SixDofDragBehavior : public Behavior<Mesh> {

public:
  /**
   * @brief Instantiates a behavior that when attached to a mesh will allow the
   * mesh to be dragged around based on directions and origin of the pointer's
   * ray.
   */
  SixDofDragBehavior();
  virtual ~SixDofDragBehavior();

  /**
   * @brief The name of the behavior.
   */
  const std::string name() const;

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the drag behavior the passed in mesh.
   * @param ownerNode The mesh that will be dragged around once attached
   */
  void attach(const MeshPtr& ownerNode) override;

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

public:
  /**
   * If the behavior is currently in a dragging state
   */
  bool dragging;

  /**
   * The distance towards the target drag position to move each frame. This can
   * be useful to avoid jitter. Set this to 1 for no delay. (Default: 0.2)
   */
  float dragDeltaRatio;

  /**
   * The id of the pointer that is currently interacting with the behavior (-1
   * when no pointer is active)
   */
  int currentDraggingPointerID;

  /**
   * If camera controls should be detached during the drag
   */
  bool detachCameraControls;

  /**
   * Fires each time a drag starts
   */
  Observable<DragStartOrEndEvent> onDragStartObservable;

  /**
   * Fires each time a drag ends (eg. mouse release after drag)
   */
  Observable<DragStartOrEndEvent> onDragEndObservable;

private:
  static std::unique_ptr<Scene> _virtualScene;
  MeshPtr _ownerNode;
  Observer<Scene>::Ptr _sceneRenderObserver;
  Scene* _scene;
  Vector3 _targetPosition;
  AbstractMeshPtr _virtualOriginMesh;
  AbstractMeshPtr _virtualDragMesh;
  Observer<PointerInfo>::Ptr _pointerObserver;
  bool _moving;
  Quaternion _startingOrientation;

  /**
   * How much faster the object should move when the controller is moving
   * towards it. This is useful to bring objects that are far away from the user
   * to them faster. Set this to 0 to avoid any speed increase. (Default: 3)
   */
  float zDragFactor;

}; // end of class SixDofDragBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_SIX_DOF_DRAG_BEHAVIOR_H
