#ifndef BABYLON_BEHAVIORS_MESH_ATTACH_TO_BOX_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESH_ATTACH_TO_BOX_BEHAVIOR_H

#include <babylon/babylon_api.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/behaviors/mesh/face_direction_info.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class Mesh;
class Scene;
class TransformNode;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief A behavior that when attached to a mesh will will place a specified
 * node on the meshes face pointing towards the camera.
 */
class BABYLON_SHARED_EXPORT AttachToBoxBehavior : public Behavior<Mesh> {

public:
  /**
   * A behavior that when attached to a mesh will will place a specified node on
   * the meshes face pointing towards the camera
   */
  static constexpr const char* name = "AttachToBoxBehavior";

public:
  /**
   * @brief Creates the AttachToBoxBehavior, used to attach UI to the closest
   * face of the box to a camera
   * @param ui The transform node that should be attched to the mesh
   */
  AttachToBoxBehavior(TransformNode* ui);
  virtual ~AttachToBoxBehavior();

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the AttachToBoxBehavior to the passed in mesh
   * @param target The mesh that the specified node will be attached to
   */
  void attach(const MeshPtr& target) override;

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

private:
  FaceDirectionInfo _closestFace(const Vector3& targetDirection);
  void _lookAtToRef(const Vector3& pos, Quaternion& ref,
                    const Vector3 up = Vector3(0.f, 1.f, 0.f));

public:
  /**
   * The distance away from the face of the mesh that the UI should be attached
   * to (default: 0.15)
   */
  float distanceAwayFromFace;

  /**
   * The distance from the bottom of the face that the UI should be attached to
   * (default: 0.15)
   */
  float distanceAwayFromBottomOfFace;

private:
  TransformNode* ui;
  std::array<FaceDirectionInfo, 6> _faceVectors;
  MeshPtr _target;
  Scene* _scene;
  Observer<Scene>::Ptr _onRenderObserver;
  Matrix _tmpMatrix;
  Vector3 _tmpVector;

  Vector3 _zeroVector;
  Matrix _lookAtTmpMatrix;

}; // end of class AttachToBoxBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESH_ATTACH_TO_BOX_BEHAVIOR_H
