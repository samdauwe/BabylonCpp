#ifndef BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_H

#include <babylon/babylon_global.h>

#include <babylon/behaviors/behavior.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior_options.h>
#include <babylon/core/structs.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief A behavior that when attached to a mesh will allow the mesh to be
 * dragged around the screen based on pointer events.
 */
class BABYLON_SHARED_EXPORT PointerDragBehavior : public Behavior<Node> {

public:
  /**
   * @brief Creates a pointer drag behavior that can be attached to a mesh.
   * @param options The drag axis or normal of the plane that will be dragged
   * across. pointerObservableScene can be used to listen to drag events from
   * another scene(eg. if the attached mesh is in an overlay scene).
   */
  PointerDragBehavior(const PointerDragBehaviorOptions& options);
  virtual ~PointerDragBehavior();

  /**
   * @brief The name of the behavior.
   */
  const string_t name() const;

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the drag behavior the passed in mesh.
   * @param ownerNode The mesh that will be dragged around once attached
   */
  void attach(Node* ownerNode) override;

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

private:
  Nullable<Vector3> _pickWithRayOnDragPlane(const Nullable<Ray>& ray);

  /**
   * @brief Position the drag plane based on the attached mesh position, for
   * single axis rotate the plane along the axis to face the camera.
   */
  void _updateDragPlanePosition(const Ray& ray);

public:
  /**
   * Fires each time the attached mesh is dragged with the pointer
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
   *  Mesh with the position where the drag plane should be placed
   */
  Mesh* _dragPlaneParent;

private:
  Node* _attachedNode;
  Mesh* _dragPlane;
  Scene* _scene;
  Observer<PointerInfoPre>::Ptr _pointerObserver;
  static unique_ptr_t<Scene> _planeScene;
  int _draggingID;

  /**
   * The drag axis or normal of the plane that will be dragged across.
   * pointerObservableScene can be used to listen to drag events from another
   * scene(eg. if the attached mesh is in an overlay scene)
   */
  PointerDragBehaviorOptions options;

}; // end of class PointerDragBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_POINTER_DRAG_BEHAVIOR_H
