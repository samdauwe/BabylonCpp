#ifndef BABYLON_MESHES_TRANSFORM_NODE_H
#define BABYLON_MESHES_TRANSFORM_NODE_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/engines/node.h>

namespace BABYLON {

class Bone;
class Camera;
using CameraPtr = std::shared_ptr<Camera>;

struct InstantiateHierarychyOptions {
  bool doNotInstantiate = false;
}; // end of struct InstantiateHierarychyOptions

/**
 * @brief A TransformNode is an object that is not rendered but can be used as a center of
 * transformation. This can decrease memory usage and increase rendering speed compared to using an
 * empty mesh as a parent and is less complicated than using a pivot matrix.
 * @see https://doc.babylonjs.com/how_to/transformnode
 */
class BABYLON_SHARED_EXPORT TransformNode : public Node {

public:
  // Statics

  /**
   * Object will not rotate to face the camera
   */
  static constexpr unsigned int BILLBOARDMODE_NONE = 0;

  /**
   * Object will rotate to face the camera but only on the x axis
   */
  static constexpr unsigned int BILLBOARDMODE_X = 1;

  /**
   * Object will rotate to face the camera but only on the y axis
   */
  static constexpr unsigned int BILLBOARDMODE_Y = 2;

  /**
   * Object will rotate to face the camera but only on the z axis
   */
  static constexpr unsigned int BILLBOARDMODE_Z = 4;

  /**
   * Object will rotate to face the camera
   */
  static constexpr unsigned int BILLBOARDMODE_ALL = 7;

  /**
   * Object will rotate to face the camera's position instead of orientation
   */
  static constexpr unsigned int BILLBOARDMODE_USE_POSITION = 128;

public:
  template <typename... Ts>
  static TransformNodePtr New(Ts&&... args)
  {
    auto transformNode
      = std::shared_ptr<TransformNode>(new TransformNode(std::forward<Ts>(args)...));
    transformNode->addToScene(transformNode);

    return transformNode;
  }
  ~TransformNode() override; // = default

  /**
   * @brief Adds the transform node to the scene.
   */
  void addToScene(const TransformNodePtr& transformNode);

  /**
   * @brief Hidden
   */
  TransformNodePtr _this() const;

  /**
   * @brief Returns the object type.
   */
  Type type() const override;

  /**
   * @brief Gets a property.
   */
  AnimationValue getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  void setProperty(const std::vector<std::string>& targetPropertyPath,
                   const AnimationValue& value) override;

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "TransformNode" string
   */
  std::string getClassName() const override;

  /**
   * @brief Copies the parameter passed Matrix into the mesh Pose matrix.
   * @param matrix the matrix to copy the pose from
   * @returns this TransformNode.
   */
  TransformNode& updatePoseMatrix(const Matrix& matrix);

  /**
   * @brief Returns the mesh Pose matrix.
   * @returns the pose matrix
   */
  Matrix& getPoseMatrix();

  /**
   * @brief Returns the mesh Pose matrix.
   * @returns the pose matrix
   */
  const Matrix& getPoseMatrix() const;

  /**
   * @brief Hidden
   */
  bool _isSynchronized() override;

  /**
   * @brief Hidden
   */
  void _initCache() override;

  /**
   * @brief Flag the transform node as dirty (Forcing it to update everything).
   * @param property if set to "rotation" the objects rotationQuaternion will be
   * set to null
   * @returns this transform node
   */
  TransformNode& markAsDirty(const std::string& property) override;

  /**
   * @brief Sets a new matrix to apply before all other transformation
   * @param matrix defines the transform matrix
   * @returns the current TransformNode
   */
  TransformNode& setPreTransformMatrix(Matrix& matrix);

  /**
   * @brief Sets a new pivot matrix to the current node.
   * @param matrix defines the new pivot matrix to use
   * @param postMultiplyPivotMatrix defines if the pivot matrix must be
   * cancelled in the world matrix. When this parameter is set to true
   * (default), the inverse of the pivot matrix is also applied at the end to
   * cancel the transformation effect
   * @returns the current TransformNode
   */
  TransformNode& setPivotMatrix(Matrix matrix, bool postMultiplyPivotMatrix = true);

  /**
   * @brief Returns the mesh pivot matrix.
   * Default : Identity.
   * @returns the matrix
   */
  Matrix& getPivotMatrix();

  /**
   * @brief Returns the mesh pivot matrix.
   * Default : Identity.
   * @returns the matrix
   */
  const Matrix& getPivotMatrix() const;

  /**
   * @brief Instantiate (when possible) or clone that node with its hierarchy.
   * @param newParent defines the new parent to use for the instance (or clone)
   * @param options defines options to configure how copy is done
   * @param onNewNodeCreated defines an option callback to call when a clone or an instance is
   * created
   * @returns an instance (or a clone) of the current node with its hiearchy
   */
  TransformNodePtr instantiateHierarchy(
    TransformNode* newParent                                   = nullptr,
    const std::optional<InstantiateHierarychyOptions>& options = std::nullopt,
    const std::function<void(TransformNode* source, TransformNode* clone)>& onNewNodeCreated
    = nullptr);

  /**
   * @brief Prevents the World matrix to be computed any longer.
   * @param newWorldMatrix defines an optional matrix to use as world matrix
   * @returns the TransformNode.
   */
  TransformNode& freezeWorldMatrix(const std::optional<Matrix>& newWorldMatrix = std::nullopt);

  /**
   * @brief Allows back the World matrix computation.
   * @returns the TransformNode.
   */
  TransformNode& unfreezeWorldMatrix();

  /**
   * @brief Retuns the mesh absolute position in the World.
   * @returns a Vector3.
   */
  Vector3& getAbsolutePosition();

  /**
   * @brief Sets the mesh absolute position in the World from a Vector3 or an
   * Array(3).
   * @param absolutePosition the absolute position to set
   * @returns the TransformNode.
   */
  TransformNode& setAbsolutePosition(const std::optional<Vector3>& absolutePosition);

  /**
   * @brief Sets the mesh position in its local space.
   * @param vector3 the position to set in localspace
   * @returns the TransformNode.
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
   * @param vector3 the distance to translate in localspace
   * @returns the TransformNode.
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
  TransformNode& lookAt(const Vector3& targetPoint, float yawCor = 0.f, float pitchCor = 0.f,
                        float rollCor = 0.f, Space space = Space::LOCAL);

  /**
   * @brief Returns a new Vector3 that is the localAxis, expressed in the mesh
   * local space, rotated like the mesh. This Vector3 is expressed in the World
   * space.
   * @param localAxis axis to rotate
   * @returns a new Vector3 that is the localAxis, expressed in the mesh local
   * space, rotated like the mesh.
   */
  Vector3 getDirection(const Vector3& localAxis);

  /**
   * @brief Sets the Vector3 "result" as the rotated Vector3 "localAxis" in the
   * same rotation than the mesh. localAxis is expressed in the mesh local
   * space. result is computed in the Wordl space from the mesh World matrix.
   * @param localAxis axis to rotate
   * @param result the resulting transformnode
   * @returns this TransformNode.
   */
  TransformNode& getDirectionToRef(const Vector3& localAxis, Vector3& result);

  /**
   * @brief Sets this transform node rotation to the given local axis.
   * @param localAxis the axis in local space
   * @param yawCor optional yaw (y-axis) correction in radians
   * @param pitchCor optional pitch (x-axis) correction in radians
   * @param rollCor optional roll (z-axis) correction in radians
   * @returns this TransformNode
   */
  TransformNode& setDirection(const Vector3& localAxis, float yawCor = 0.f, float pitchCor = 0.f,
                              float rollCor = 0.f);

  /**
   * @brief Sets a new pivot point to the current node
   * @param point defines the new pivot point to use
   * @param space defines if the point is in world or local space (local by
   * default)
   * @returns the current TransformNode
   */
  TransformNode& setPivotPoint(Vector3& point, Space space = Space::LOCAL);

  /**
   * @brief Returns a new Vector3 set with the mesh pivot point coordinates in
   * the local space.
   * @returns the pivot point
   */
  Vector3 getPivotPoint();

  /**
   * @brief Sets the passed Vector3 "result" with the coordinates of the mesh
   * pivot point in the local space.
   * @param result the vector3 to store the result
   * @returns this TransformNode.
   */
  TransformNode& getPivotPointToRef(Vector3& result);

  /**
   * @brief Returns a new Vector3 set with the mesh pivot point World
   * coordinates.
   * @returns a new Vector3 set with the mesh pivot point World coordinates.
   */
  Vector3 getAbsolutePivotPoint();

  /**
   * @brief Sets the Vector3 "result" coordinates with the mesh pivot point
   * World coordinates.
   * @param result vector3 to store the result
   * @returns this TransformNode.
   */
  TransformNode& getAbsolutePivotPointToRef(Vector3& result);

  /**
   * @brief Defines the passed node as the parent of the current node.
   * The node will remain exactly where it is and its position / rotation will
   * be updated accordingly
   * @see https://doc.babylonjs.com/how_to/parenting
   * @param node the node ot set as the parent
   * @returns this TransformNode.
   */
  TransformNode& setParent(Node* node);

  /**
   * @brief Hidden
   */
  virtual bool _updateNonUniformScalingState(bool value);

  /**
   * @brief Attach the current TransformNode to another TransformNode associated
   * with a bone.
   * @param bone Bone affecting the TransformNode
   * @param affectedTransformNode TransformNode associated with the bone
   * @returns this object
   */
  TransformNode& attachToBone(Bone* bone, TransformNode* affectedTransformNode);

  /**
   * @brief Detach the transform node if its associated with a bone.
   * @returns this object
   */
  TransformNode& detachFromBone();

  /**
   * @brief Rotates the mesh around the axis vector for the passed angle
   * (amount) expressed in radians, in the given space. space (default LOCAL)
   * can be either Space.LOCAL, either Space.WORLD. Note that the property
   * `rotationQuaternion` is then automatically updated and the property
   * `rotation` is set to (0,0,0) and no longer used. The passed axis is also
   * normalized.
   * @param axis the axis to rotate around
   * @param amount the amount to rotate in radians
   * @param space Space to rotate in (Default: local)
   * @returns the TransformNode.
   */
  TransformNode& rotate(Vector3 axis, float amount, Space space = Space::LOCAL);

  /**
   * @brief Rotates the mesh around the axis vector for the passed angle
   * (amount) expressed in radians, in world space. Note that the property
   * `rotationQuaternion` is then automatically updated and the property
   * `rotation` is set to (0,0,0) and no longer used. The passed axis is also
   * normalized. . Method is based on
   * http://www.euclideanspace.com/maths/geometry/affine/aroundPoint/index.htm
   * @param point the point to rotate around
   * @param axis the axis to rotate around
   * @param amount the amount to rotate in radians
   * @returns the TransformNode
   */
  TransformNode& rotateAround(const Vector3& point, Vector3& axis, float amount);

  /**
   * @brief Translates the mesh along the axis vector for the passed distance in
   * the given space. space (default LOCAL) can be either Space.LOCAL, either
   * Space.WORLD.
   * @param axis the axis to translate in
   * @param distance the distance to translate
   * @param space Space to rotate in (Default: local)
   * @returns the TransformNode.
   */
  TransformNode& translate(const Vector3& axis, float distance, Space space = Space::LOCAL);
  /**
   * @brief Adds a rotation step to the mesh current rotation.
   * x, y, z are Euler angles expressed in radians.
   * This methods updates the current mesh rotation, either mesh.rotation,
   * either mesh.rotationQuaternion if it's set. This means this rotation is
   * made in the mesh local space only. It's useful to set a custom rotation
   * order different from the BJS standard one YXZ. Example : this rotates the
   * mesh first around its local X axis, then around its local Z axis, finally
   * around its local Y axis.
   * ```javascript
   * mesh.addRotation(x1, 0, 0).addRotation(0, 0, z2).addRotation(0, 0, y3);
   * ```
   * Note that `addRotation()` accumulates the passed rotation values to the
   * current ones and computes the .rotation or .rotationQuaternion updated
   * values. Under the hood, only quaternions are used. So it's a little faster
   * is you use .rotationQuaternion because it doesn't need to translate them
   * back to Euler angles.
   * @param x Rotation to add
   * @param y Rotation to add
   * @param z Rotation to add
   * @returns the TransformNode.
   */
  TransformNode& addRotation(float x, float y, float z);

  /**
   * @brief Computes the world matrix of the node
   * @param force defines if the cache version should be invalidated forcing the
   * world matrix to be created from scratch
   * @returns the world matrix
   */
  Matrix& computeWorldMatrix(bool force = false, bool useWasUpdatedFlag = false) override;

  /**
   * @brief If you'd like to be called back after the mesh position, rotation or
   * scaling has been updated.
   * @param func callback function to add
   * @returns the TransformNode.
   */
  TransformNode& registerAfterWorldMatrixUpdate(
    const std::function<void(TransformNode* mesh, EventState& es)>& func);

  /**
   * @brief Removes a registered callback function.
   * @param func callback function to remove
   * @returns the TransformNode.
   */
  TransformNode& unregisterAfterWorldMatrixUpdate(
    const std::function<void(TransformNode* mesh, EventState& es)>& func);

  /**
   * @brief Gets the position of the current mesh in camera space.
   * @param camera defines the camera to use
   * @returns a position
   */
  Vector3 getPositionInCameraSpace(const CameraPtr& camera = nullptr) const;

  /**
   * @brief Returns the distance from the mesh to the active camera.
   * @param camera defines the camera to use
   * @returns the distance
   */
  float getDistanceToCamera(const CameraPtr& camera = nullptr);

  /**
   * @brief Clone the current transform node
   * @param name Name of the new clone
   * @param newParent New parent for the clone
   * @param doNotCloneChildren Do not clone children hierarchy
   * @returns the new transform node
   */
  TransformNodePtr clone(const std::string& name, Node* newParent, bool doNotCloneChildren = false);

  /**
   * @brief Serializes the objects information.
   * @param currentSerializationObject defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& currentSerializationObject);

  // Statics
  /**
   * @brief Returns a new TransformNode object parsed from the source provided.
   * @param parsedTransformNode is the source.
   * @param scene the scne the object belongs to
   * @param rootUrl is a string, it's the root URL to prefix the
   * `delayLoadingFile` property with
   * @returns a new TransformNode object parsed from the source provided.
   */
  static TransformNodePtr Parse(const json& parsedTransformNode, Scene* scene,
                                const std::string& rootUrl);

  /**
   * @brief Get all child-transformNodes of this node.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of TransformNode
   */
  virtual std::vector<TransformNodePtr>
  getChildTransformNodes(bool directDescendantsOnly                                = false,
                         const std::function<bool(const NodePtr& node)>& predicate = nullptr);

  /**
   * @brief Releases resources associated with this transform node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Uniformly scales the mesh to fit inside of a unit cube (1 X 1 X 1
   * units).
   * @param includeDescendants Use the hierarchy's bounding box instead of the
   * mesh's bounding box. Default is false
   * @param ignoreRotation ignore rotation when computing the scale (ie. object
   * will be axis aligned). Default is false
   * @param predicate predicate that is passed in to getHierarchyBoundingVectors
   * when selecting which object should be included when scaling
   * @returns the current mesh
   */
  TransformNode&
  normalizeToUnitCube(bool includeDescendants = true, bool ignoreRotation = false,
                      const std::function<bool(const AbstractMeshPtr& node)>& predicate = nullptr);

protected:
  TransformNode(const std::string& name, Scene* scene = nullptr, bool isPure = true);

  virtual void _afterComputeWorldMatrix();

  /**
   * @brief Gets the billboard mode. Default is 0.
   *
   * | Value | Type | Description |
   * | --- | --- | --- |
   * | 0 | BILLBOARDMODE_NONE |  |
   * | 1 | BILLBOARDMODE_X |  |
   * | 2 | BILLBOARDMODE_Y |  |
   * | 4 | BILLBOARDMODE_Z |  |
   * | 7 | BILLBOARDMODE_ALL |  |
   *
   */
  unsigned int get_billboardMode() const;

  /**
   * @brief Sets the billboard mode. Default is 0.
   */
  void set_billboardMode(unsigned int value);

  /**
   * @brief Gets or sets a boolean indicating that parent rotation should be
   * preserved when using billboards. This could be useful for glTF objects
   * where parent rotation helps converting from right handed to left handed.
   */
  bool get_preserveParentRotationForBillboard() const;

  /**
   * @brief Sets a boolean indicating that parent rotation should be preserved
   * when using billboards. This could be useful for glTF objects where parent
   * rotation helps converting from right handed to left handed.
   */
  void set_preserveParentRotationForBillboard(bool value);

  /**
   * @brief Gets the distance of the object to max, often used by skybox.
   */
  bool get_infiniteDistance() const;

  /**
   * @brief Sets the distance of the object to max, often used by skybox
   */
  void set_infiniteDistance(bool value);

  /**
   * @brief Gets the position property.
   */
  Vector3& get_position();

  /**
   * @brief Sets the position property.
   */
  void set_position(const Vector3& newPosition);

  /**
   * @brief Gets the rotation property : a Vector3 defining the rotation value
   * in radians around each local axis X, Y, Z  (default is (0.0, 0.0, 0.0)). If
   * rotation quaternion is set, this Vector3 will be ignored and copy from the
   * quaternion.
   */
  Vector3& get_rotation();

  /**
   * @brief Sets the rotation property : a Vector3 defining the rotation value
   * in radians around each local axis X, Y, Z  (default is (0.0, 0.0, 0.0)). If
   * rotation quaternion is set, this Vector3 will be ignored and copy from the
   * quaternion.
   */
  void set_rotation(const Vector3& newRotation);

  /**
   * @brief Gets the scaling property : a Vector3 defining the node scaling
   * along each local axis X, Y, Z (default is (0.0, 0.0, 0.0)).
   */
  virtual Vector3& get_scaling();

  /**
   * @brief Sets the scaling property : a Vector3 defining the node scaling
   * along each local axis X, Y, Z (default is (0.0, 0.0, 0.0)).
   */
  virtual void set_scaling(const Vector3& newScaling);

  /**
   * @brief Gets the rotation Quaternion property : this a Quaternion object
   * defining the node rotation by using a unit quaternion (undefined by
   * default, but can be null). If set, only the rotationQuaternion is then used
   * to compute the node rotation (ie. node.rotation will be ignored).
   */
  std::optional<Quaternion>& get_rotationQuaternion();

  /**
   * @brief Sets the rotation Quaternion property : this a Quaternion object
   * defining the node rotation by using a unit quaternion (undefined by
   * default, but can be null). If set, only the rotationQuaternion is then used
   * to compute the node rotation (ie. node.rotation will be ignored).
   */
  void set_rotationQuaternion(const std::optional<Quaternion>& quaternion);

  /**
   * @brief The forward direction of that transform in world space.
   */
  Vector3& get_forward();

  /**
   * @brief The up direction of that transform in world space.
   */
  Vector3& get_up();

  /**
   * @brief The right direction of that transform in world space.
   */
  Vector3& get_right();

  /**
   * @brief Retuns the mesh absolute position in the World.
   * @returns a Vector3.
   */
  Vector3& get_absolutePosition();

  /**
   * @brief Returns the current mesh absolute scaling.
   * @returns a Vector3.
   */
  Vector3& get_absoluteScaling();

  /**
   * @brief Returns the current mesh absolute scaling.
   * @returns a Vector3.
   */
  Quaternion& get_absoluteRotationQuaternion();

  /**
   * @brief True if the World matrix has been frozen.
   * @returns a boolean.
   */
  bool get_isWorldMatrixFrozen() const;

  /**
   * @brief True if the scaling property of this object is non uniform eg.
   * (1,2,1)
   */
  bool get_nonUniformScaling() const;

  /**
   * @brief Hidden
   */
  Node* _getEffectiveParent() const;

private:
  void _syncAbsoluteScalingAndRotation();

public:
  /**
   * Gets or sets the billboard mode. Default is 0.
   *
   * | Value | Type | Description |
   * | --- | --- | --- |
   * | 0 | BILLBOARDMODE_NONE |  |
   * | 1 | BILLBOARDMODE_X |  |
   * | 2 | BILLBOARDMODE_Y |  |
   * | 4 | BILLBOARDMODE_Z |  |
   * | 7 | BILLBOARDMODE_ALL |  |
   *
   */
  Property<TransformNode, unsigned int> billboardMode;

  /**
   * Gets or sets a boolean indicating that parent rotation should be preserved
   * when using billboards. This could be useful for glTF objects where parent
   * rotation helps converting from right handed to left handed
   */
  Property<TransformNode, bool> preserveParentRotationForBillboard;

  /**
   * Gets or sets the distance of the object to max, often used by skybox
   */
  Property<TransformNode, bool> infiniteDistance;

  /**
   * Multiplication factor on scale x/y/z when computing the world matrix. Eg.
   * for a 1x1x1 cube setting this to 2 will make it a 2x2x2 cube
   */
  float scalingDeterminant;

  /**
   * Gets or sets a boolean indicating that non uniform scaling (when at least
   * one component is different from others) should be ignored. By default the
   * system will update normals to compensate
   */
  bool ignoreNonUniformScaling;

  /**
   * Gets or sets a boolean indicating that even if rotationQuaternion is
   * defined, you can keep updating rotation property and Babylon.js will just
   * mix both
   */
  bool reIntegrateRotationIntoRotationQuaternion;

  // Cache
  /** Hidden */
  std::unique_ptr<Matrix> _poseMatrix;
  /** Hidden */
  Matrix _localMatrix;
  /** Hidden */
  int _indexInSceneTransformNodesArray;

  /**
   * Gets or set the node position (default is (0.0, 0.0, 0.0))
   */
  Property<TransformNode, Vector3> position;

  /**
   * Gets or sets the rotation property : a Vector3 defining the rotation value
   * in radians around each local axis X, Y, Z  (default is (0.0, 0.0, 0.0)). If
   * rotation quaternion is set, this Vector3 will be ignored and copy from the
   * quaternion
   */
  Property<TransformNode, Vector3> rotation;

  /**
   * Gets or sets the scaling property : a Vector3 defining the node scaling
   * along each local axis X, Y, Z (default is (0.0, 0.0, 0.0)).
   */
  Property<TransformNode, Vector3> scaling;

  /**
   * Gets or sets the rotation Quaternion property : this a Quaternion object
   * defining the node rotation by using a unit quaternion (undefined by
   * default, but can be null). If set, only the rotationQuaternion is then used
   * to compute the node rotation (ie. node.rotation will be ignored)
   */
  Property<TransformNode, std::optional<Quaternion>> rotationQuaternion;

  /**
   * The forward direction of that transform in world space.
   */
  ReadOnlyProperty<TransformNode, Vector3> forward;

  /**
   * The up direction of that transform in world space.
   */
  ReadOnlyProperty<TransformNode, Vector3> up;

  /**
   * The right direction of that transform in world space.
   */
  ReadOnlyProperty<TransformNode, Vector3> right;

  /**
   * Retuns the mesh absolute position in the World.
   */
  ReadOnlyProperty<TransformNode, Vector3> absolutePosition;

  /**
   * Returns the current mesh absolute scaling.
   * Returns a Vector3.
   */
  ReadOnlyProperty<TransformNode, Vector3> absoluteScaling;

  /**
   * Returns the current mesh absolute rotation.
   * Returns a Quaternion.
   */
  ReadOnlyProperty<TransformNode, Quaternion> absoluteRotationQuaternion;

  /**
   * True if the World matrix has been frozen.
   */
  ReadOnlyProperty<TransformNode, bool> isWorldMatrixFrozen;

  /**
   * True if the scaling property of this object is non uniform eg. (1,2,1)
   */
  ReadOnlyProperty<TransformNode, bool> nonUniformScaling;

protected:
  // Properties
  Vector3 _scaling;
  bool _isDirty;
  bool _postMultiplyPivotMatrix;
  // Cache
  bool _isWorldMatrixFrozen;

  /**
   * An event triggered after the world matrix is updated
   */
  Observable<TransformNode> onAfterWorldMatrixUpdateObservable;

private:
  static std::unique_ptr<Vector3> _lookAtVectorCache;
  static std::unique_ptr<Quaternion> _rotationAxisCache;

private:
  bool _isPure;
  Vector3 _forward;
  Vector3 _forwardNormalized;
  Vector3 _forwardInverted;
  Vector3 _up;
  Vector3 _upNormalized;
  Vector3 _right;
  Vector3 _rightNormalized;
  Vector3 _rightInverted;

  // Properties
  Vector3 _position;
  Vector3 _rotation;
  std::optional<Quaternion> _rotationQuaternion;
  TransformNode* _transformToBoneReferal;
  bool _isAbsoluteSynced;
  unsigned int _billboardMode;
  bool _preserveParentRotationForBillboard;
  bool _infiniteDistance;
  Matrix _localWorld;
  bool _usePivotMatrix;
  Vector3 _absolutePosition;
  Vector3 _absoluteScaling;
  Quaternion _absoluteRotationQuaternion;
  Matrix _pivotMatrix;
  std::unique_ptr<Matrix> _pivotMatrixInverse;
  bool _nonUniformScaling;

}; // end of class TransformNode

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_TRANSFORM_NODE_H
