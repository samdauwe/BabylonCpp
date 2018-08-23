#ifndef BABYLON_BEHAVIORS_MESH_MULTI_POINTER_SCALE_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESH_MULTI_POINTER_SCALE_BEHAVIOR_H

#include <babylon/babylon_global.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief A behavior that when attached to a mesh will allow the mesh to be
 * scaled.
 */
class BABYLON_SHARED_EXPORT MultiPointerScaleBehavior : public Behavior<Mesh> {

public:
  /**
   * @brief Creates the behavior.
   */
  MultiPointerScaleBehavior();
  virtual ~MultiPointerScaleBehavior();

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
  void attach(const MeshPtr& ownerNode) override;

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

private:
  float _getCurrentDistance();

private:
  unique_ptr_t<PointerDragBehavior> _dragBehaviorA;
  unique_ptr_t<PointerDragBehavior> _dragBehaviorB;
  float _startDistance;
  Vector3 _initialScale;
  Vector3 _targetScale;
  MeshPtr _ownerNode;
  Observer<Scene>::Ptr _sceneRenderObserver;

}; // end of class MultiPointerScaleBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_MULTI_POINTER_SCALE_BEHAVIOR_H
