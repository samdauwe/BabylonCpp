#ifndef BABYLON_MESH_TRANSFORM_NODE_H
#define BABYLON_MESH_TRANSFORM_NODE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>

namespace BABYLON {

/**
 * @brief TransformNode class.
 */
class BABYLON_SHARED_EXPORT TransformNode : public Node {

public:
  // Statics
  static constexpr unsigned int BILLBOARDMODE_NONE = 0;
  static constexpr unsigned int BILLBOARDMODE_X    = 1;
  static constexpr unsigned int BILLBOARDMODE_Y    = 2;
  static constexpr unsigned int BILLBOARDMODE_Z    = 4;
  static constexpr unsigned int BILLBOARDMODE_ALL  = 7;

public:
  TransformNode(const string_t& name, Scene* scene = nullptr,
                bool isPure = true);
  ~TransformNode();

  Vector3& position();
  const Vector3& position() const;
  void setPosition(const Vector3& newPosition);

  /**
   * @brief Rotation property : a Vector3 depicting the rotation value in
   * radians around each local axis X, Y, Z.
   * If rotation quaternion is set, this Vector3 will (almost always) be the
   * Zero vector!
   * Default : (0.0, 0.0, 0.0)
   */
  Vector3& rotation();
  const Vector3& rotation() const;
  void setRotation(const Vector3& newRotation);

  /**
   * @brief Scaling property : a Vector3 depicting the mesh scaling along each
   * local axis X, Y, Z.
   * Default : (1.0, 1.0, 1.0)
   */
  virtual Vector3& scaling();
  const Vector3& scaling() const;

  /**
   * @brief Scaling property : a Vector3 depicting the mesh scaling along each
   * local axis X, Y, Z.
   * Default : (1.0, 1.0, 1.0)
   */
  void setScaling(const Vector3& newScaling);

  /**
   * @brief Rotation Quaternion property : this a Quaternion object depicting
   * the mesh rotation by using a unit quaternion.
   * It's null by default.
   * If set, only the rotationQuaternion is then used to compute the mesh
   * rotation and its property `.rotation\ is then ignored and set to (0.0, 0.0,
   * 0.0)
   */
  unique_ptr_t<Quaternion>& rotationQuaternion();
  const unique_ptr_t<Quaternion>& rotationQuaternion() const;
  void setRotationQuaternion(const Nullable<Quaternion>& quaternion);

  /**
   * @brief Returns the latest update of the World matrix.
   * @returns a Matrix.
   */
  Matrix* getWorldMatrix() override;

  /**
   * @brief Returns directly the latest state of the mesh World matrix.
   * A Matrix is returned.
   */
  Matrix& worldMatrixFromCache();
  const Matrix& worldMatrixFromCache() const;

  /**
   * @brief Copies the paramater passed Matrix into the mesh Pose matrix.
   * @returns the AbstractMesh.
   */
  TransformNode& updatePoseMatrix(const Matrix& matrix);

  /**
   * @brief Returns the mesh Pose matrix.
   * Returned object : Matrix
   */
  Matrix& getPoseMatrix();
  const Matrix& getPoseMatrix() const;

  bool _isSynchronized() override;
  void _initCache() override;
  TransformNode& _markAsDirty(const string_t& property);

  /**
   * @brief Returns the current mesh absolute position.
   * @returns a Vector3.
   */
  Vector3& absolutePosition();
  const Vector3& absolutePosition() const;

  /**
   * @brief Sets a new pivot matrix to the mesh.
   * @returns the AbstractMesh.
   */
  TransformNode& setPivotMatrix(Matrix& matrix,
                                bool postMultiplyPivotMatrix = false);

  /**
   * @brief Returns the mesh pivot matrix.
   * Default : Identity.
   * A Matrix is returned.
   */
  Matrix& getPivotMatrix();
  const Matrix& getPivotMatrix() const;

  /**
   * @brief Prevents the World matrix to be computed any longer.
   * @returns the AbstractMesh.
   */
  TransformNode& freezeWorldMatrix();

  /**
   * @brief Allows back the World matrix computation.
   * @returns Returns the AbstractMesh.
   */
  TransformNode& unfreezeWorldMatrix();

  /**
   * @brief True if the World matrix has been frozen.
   * @returns a boolean.
   */
  bool isWorldMatrixFrozen() const;

  /**
   * @brief Retuns the mesh absolute position in the World.
   * @returns a Vector3.
   */
  Vector3& getAbsolutePosition();

  /**
   * @brief Sets the mesh absolute position in the World from a Vector3 or an
   * Array(3).
   * @returns the AbstractMesh.
   */
  TransformNode& setAbsolutePosition(const Nullable<Vector3>& absolutePosition);

  /**
   * @brief Sets the mesh position in its local space.
   * @returns the AbstractMesh.
   */
  TransformNode& setPositionWithLocalVector(const Vector3& vector3);

  /**
   * @brief Returns the mesh position in the local space from the current World
   * matrix values.
   * @returns a new Vector3.
   */
  Vector3 getPositionExpressedInLocalSpace();

  /**
   * @brief Translates the mesh along the passed Vector3 in its local space.
   * @returns the AbstractMesh.
   */
  TransformNode& locallyTranslate(const Vector3& vector3);

  /**
   * @brief Orients a mesh towards a target point. Mesh must be drawn facing
   * user.
   * @param targetPoint the position (must be in same space as current mesh) to
   * look at
   * @param yawCor optional yaw (y-axis) correction in radians
   * @param pitchCor optional pitch (x-axis) correction in radians
   * @param rollCor optional roll (z-axis) correction in radians
   * @param space the choosen space of the target
   * @returns the TransformNode.
   */
  TransformNode& lookAt(const Vector3& targetPoint, float yawCor = 0.f,
                        float pitchCor = 0.f, float rollCor = 0.f,
                        Space space = Space::LOCAL);

  /**
   * @brief Returns a new Vector3 what is the localAxis, expressed in the mesh
   * local space, rotated like the mesh.
   * This Vector3 is expressed in the World space.
   */
  Vector3 getDirection(const Vector3& localAxis);

  /**
   * @brief Sets the Vector3 "result" as the rotated Vector3 "localAxis" in the
   * same rotation than the mesh.
   * localAxis is expressed in the mesh local space.
   * result is computed in the Wordl space from the mesh World matrix.
   * @returns the AbstractMesh.
   */
  TransformNode& getDirectionToRef(const Vector3& localAxis, Vector3& result);

  TransformNode& setPivotPoint(Vector3& point, Space space = Space::LOCAL);

  /**
   * @brief Returns a new Vector3 set with the mesh pivot point coordinates in
   * the local space.
   */
  Vector3 getPivotPoint();

  /**
   * @brief Sets the passed Vector3 "result" with the coordinates of the mesh
   * pivot point in the local space.
   * @returns the AbstractMesh.
   */
  TransformNode& getPivotPointToRef(Vector3& result);

  /**
   * @brief Returns a new Vector3 set with the mesh pivot point World
   * coordinates.
   */
  Vector3 getAbsolutePivotPoint();

  /**
   * @brief Sets the Vector3 "result" coordinates with the mesh pivot point
   * World coordinates.
   * @returns the AbstractMesh.
   */
  TransformNode& getAbsolutePivotPointToRef(Vector3& result);

  /**
   * @brief Defines the passed node as the parent of the current node.
   * The node will remain exactly where it is and its position / rotation will
   * be updated accordingly
   * @returns the TransformNode.
   */
  TransformNode& setParent(TransformNode* node);

  bool nonUniformScaling() const;

  virtual bool _updateNonUniformScalingState(bool value);

  /**
   * @brief Attach the current TransformNode to another TransformNode associated
   * with a bone.
   * @param bone Bone affecting the TransformNode
   * @param affectedTransformNode TransformNode associated with the bone
   */
  TransformNode& attachToBone(Bone* bone, TransformNode* affectedTransformNode);

  TransformNode& detachFromBone();

  /**
   * @brief  Rotates the mesh around the axis vector for the passed angle
   * (amount) expressed in radians, in the given space.
   * @param space (default LOCAL) can be either BABYLON.Space.LOCAL, either
   * BABYLON.Space.WORLD.
   * Note that the property `rotationQuaternion` is then automatically updated
   * and the property `rotation` is set to (0,0,0) and no longer used.
   * The passed axis is also normalized.
   * @returns the AbstractMesh.
   */
  TransformNode& rotate(Vector3& axis, float amount,
                        Space space = Space::LOCAL);

  /**
   * @brief Rotates the mesh around the axis vector for the passed angle
   * (amount) expressed in radians, in world space.
   * Note that the property `rotationQuaternion` is then automatically updated
   * and the property `rotation` is set to (0,0,0) and no longer used.
   * The passed axis is also normalized.
   * Returns the AbstractMesh.
   * Method is based on
   * http://www.euclideanspace.com/maths/geometry/affine/aroundPoint/index.htm
   */
  TransformNode& rotateAround(const Vector3& point, Vector3& axis,
                              float amount);

  /**
   * @brief Translates the mesh along the axis vector for the passed distance in
   * the given space.
   * @param space (default LOCAL) can be either BABYLON.Space.LOCAL, either
   * BABYLON.Space.WORLD.
   * @returns the AbstractMesh.
   */
  TransformNode& translate(const Vector3& axis, float distance,
                           Space space = Space::LOCAL);
  /**
   * @brief Adds a rotation step to the mesh current rotation.
   * x, y, z are Euler angles expressed in radians.
   * This methods updates the current mesh rotation, either mesh.rotation,
   * either mesh.rotationQuaternion if it's set.
   * This means this rotation is made in the mesh local space only.
   * It's useful to set a custom rotation order different from the BJS standard
   * one YXZ.
   * Example : this rotates the mesh first around its local X axis, then around
   * its local Z axis, finally around its local Y axis.
   * ```javascript
   * mesh.addRotation(x1, 0, 0).addRotation(0, 0, z2).addRotation(0, 0, y3);
   * ```
   * Note that `addRotation()` accumulates the passed rotation values to the
   * current ones and computes the .rotation or .rotationQuaternion updated
   * values.
   * Under the hood, only quaternions are used. So it's a little faster is you
   * use .rotationQuaternion because it doesn't need to translate them back to
   * Euler angles.
   * @returns the AbstractMesh.
   */
  TransformNode& addRotation(float x, float y, float z);

  /**
   * @brief Computes the mesh World matrix and returns it.
   * If the mesh world matrix is frozen, this computation does nothing more than
   * returning the last frozen values.
   * If the parameter `force` is let to `false` (default), the current cached
   * World matrix is returned.
   * If the parameter `force`is set to `true`, the actual computation is done.
   * @returns the mesh World Matrix.
   */
  Matrix& computeWorldMatrix(bool force = false) override;

  /**
   * @brief If you'd like to be called back after the mesh position, rotation or
   * scaling has been updated.
   * @param func: callback function to add
   * @returns the TransformNode.
   */
  TransformNode& registerAfterWorldMatrixUpdate(
    const ::std::function<void(TransformNode* mesh, EventState& es)>& func);

  /**
   * @brief Removes a registered callback function.
   * @returns the TransformNode.
   */
  TransformNode& unregisterAfterWorldMatrixUpdate(
    const ::std::function<void(TransformNode* mesh, EventState& es)>& func);

  /**
   * @brief Clone the current transform node.
   * @param name Name of the new clone
   * @param newParent New parent for the clone
   * @param doNotCloneChildren Do not clone children hierarchy
   * @returns the new transform node
   */
  TransformNode* clone(const string_t& name, Node* newParent,
                       bool doNotCloneChildren = false);

  Json::object serialize(Json::object& currentSerializationObject);

  // Statics
  /**
   * @brief Returns a new TransformNode object parsed from the source provided.
   * @param `parsedMesh` is the source.
   * @param `rootUrl` is a string, it's the root URL to prefix the
   * `delayLoadingFile` property with
   */
  static TransformNode* Parse(const Json::value& parsedTransformNode,
                              Scene* scene, const string_t& rootUrl);

  /**
   * @brief Disposes the TransformNode.
   * By default, all the children are also disposed unless the parameter
   * `doNotRecurse` is set to `true`.
   * @returns nothing.
   */
  void dispose(bool doNotRecurse = false) override;

protected:
  void _afterComputeWorldMatrix();

public:
  unsigned int billboardMode;
  float scalingDeterminant;
  bool infiniteDistance;
  // Cache
  unique_ptr_t<Matrix> _poseMatrix;
  unique_ptr_t<Matrix> _worldMatrix;

protected:
  // Properties
  Vector3 _scaling;
  bool _isDirty;
  // Cache
  bool _isWorldMatrixFrozen;

  /**
   * An event triggered after the world matrix is updated
   */
  Observable<TransformNode> onAfterWorldMatrixUpdateObservable;

private:
  static unique_ptr_t<Vector3> _lookAtVectorCache;
  static unique_ptr_t<Quaternion> _rotationAxisCache;

private:
  // Properties
  Vector3 _position;
  Vector3 _rotation;
  unique_ptr_t<Quaternion> _rotationQuaternion;
  TransformNode* _transformToBoneReferal;
  Matrix _localWorld;
  Vector3 _absolutePosition;
  Matrix _pivotMatrix;
  Matrix _pivotMatrixInverse;
  bool _postMultiplyPivotMatrix;
  bool _nonUniformScaling;

}; // end of class TransformNode

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_TRANSFORM_NODE_H
