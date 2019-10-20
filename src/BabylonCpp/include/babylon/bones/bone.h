#ifndef BABYLON_BONES_BONE_H
#define BABYLON_BONES_BONE_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/engines/node.h>

namespace BABYLON {

class Bone;
class Skeleton;
using BonePtr = std::shared_ptr<Bone>;

/**
 * @brief Class used to store bone information.
 * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
 */
class BABYLON_SHARED_EXPORT Bone : public Node {

public:
  template <typename... Ts>
  static BonePtr New(const std::string& iName, Skeleton* skeleton,
                     Bone* parentBone                         = nullptr,
                     const std::optional<Matrix>& localMatrix = std::nullopt,
                     const std::optional<Matrix>& restPose    = std::nullopt,
                     const std::optional<Matrix>& baseMatrix  = std::nullopt,
                     std::optional<int> index                 = std::nullopt)
  {
    auto bone = std::shared_ptr<Bone>(new Bone(
      iName, skeleton, parentBone, localMatrix, restPose, baseMatrix, index));
    bone->addToRootNodes();
    bone->addToSkeleton(bone);

    bone->setParent(parentBone, false);

    if (baseMatrix || localMatrix) {
      bone->_updateDifferenceMatrix();
    }

    return bone;
  }
  ~Bone() override;

  virtual Type type() const override;
  void addToSkeleton(const BonePtr& newBone);

  /** Members **/

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Gets the parent skeleton.
   * @returns a skeleton
   */
  Skeleton* getSkeleton() const;

  /**
   * @brief Gets parent bone.
   * @returns a bone or null if the bone is the root of the bone hierarchy
   */
  Bone* getParent() const;

  /**
   * @brief Returns an array containing the root bones.
   * @returns an array containing the root bones
   */
  std::vector<BonePtr>& getChildren();

  /**
   * @brief Sets the parent bone.
   * @param parent defines the parent (can be null if the bone is the root)
   * @param updateDifferenceMatrix defines if the difference matrix must be
   * updated
   */
  void setParent(Bone* parent, bool updateDifferenceMatrix = true);

  /**
   * @brief Gets the local matrix.
   * @returns a matrix
   */
  Matrix& getLocalMatrix();

  /**
   * @brief Gets the local matrix.
   * @returns a matrix
   */
  const Matrix& getLocalMatrix() const;

  /**
   * @brief Gets the base matrix (initial matrix which remains unchanged).
   * @returns a matrix
   */
  Matrix& getBaseMatrix();

  /**
   * @brief Gets the rest pose matrix.
   * @returns a matrix
   */
  std::optional<Matrix>& getRestPose() override;

  /**
   * @brief Gets a matrix used to store world matrix (ie. the matrix sent to
   * shaders).
   */
  Matrix& getWorldMatrix() override;

  /**
   * @brief Sets the local matrix to rest pose matrix.
   */
  void returnToRest();

  /**
   * @brief Gets the inverse of the absolute transform matrix.
   * This matrix will be multiplied by local matrix to get the difference matrix
   * (ie. the difference between original state and current state)
   * @returns a matrix
   */
  Matrix& getInvertedAbsoluteTransform();

  /**
   * @brief Gets the absolute transform matrix (ie base matrix * parent world
   * matrix).
   * @returns a matrix
   */
  Matrix& getAbsoluteTransform();

  /**
   * @brief Links with the given transform node.
   * The local matrix of this bone is copied from the transform node every
   * frame.
   * @param transformNode defines the transform node to link to
   */
  void linkTransformNode(const TransformNodePtr& transformNode);

  /**
   * @brief Gets the absolute transform matrix (ie base matrix * parent world
   * matrix).
   * @returns a matrix
   */
  const Matrix& getAbsoluteTransform() const;

  /**
   * @brief Gets the animations.
   */
  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Update the base and local matrices.
   * @param matrix defines the new base or local matrix
   * @param updateDifferenceMatrix defines if the difference matrix must be
   * updated
   * @param updateLocalMatrix defines if the local matrix should be updated
   */
  void updateMatrix(const Matrix& matrix, bool updateDifferenceMatrix = true,
                    bool updateLocalMatrix = true);

  /**
   * @brief Hidden
   */
  void _updateDifferenceMatrix(const std::optional<Matrix>& rootMatrix
                               = std::nullopt,
                               bool updateChildren = true);

  /**
   * @brief Flag the bone as dirty (Forcing it to update everything).
   */
  Bone& markAsDirty(const std::string& property = "") override;

  /**
   * @brief Hidden
   */
  void _markAsDirtyAndCompose();

  /**
   * @brief Copy an animation range from another bone.
   * @param source defines the source bone
   * @param rangeName defines the range name to copy
   * @param frameOffset defines the frame offset
   * @param rescaleAsRequired defines if rescaling must be applied if required
   * @param skelDimensionsRatio defines the scaling ratio
   * @returns true if operation was successful
   */
  bool copyAnimationRange(Bone* source, const std::string& rangeName,
                          int frameOffset, bool rescaleAsRequired = false,
                          const std::optional<Vector3>& skelDimensionsRatio
                          = std::nullopt);

  /**
   * @brief Translate the bone in local or world space.
   * @param vec The amount to translate the bone
   * @param space The space that the translation is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   */
  void translate(const Vector3& vec, Space space = Space::LOCAL,
                 AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the postion of the bone in local or world space.
   * @param position The position to set the bone
   * @param space The space that the position is in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   */
  void setPosition(const Vector3& position, Space space = Space::LOCAL,
                   AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the absolute position of the bone (world space).
   * @param position The position to set the bone
   * @param mesh The mesh that this bone is attached to
   */
  void setAbsolutePosition(const Vector3& position,
                           AbstractMesh* mesh = nullptr);

  /**
   * @brief Scale the bone on the x, y and z axes (in local space).
   * @param x The amount to scale the bone on the x axis
   * @param y The amount to scale the bone on the y axis
   * @param z The amount to scale the bone on the z axis
   * @param scaleChildren sets this to true if children of the bone should be
   * scaled as well (false by default)
   */
  void scale(float x, float y, float z, bool scaleChildren = false);

  /**
   * @brief Set the bone scaling in local space.
   * @param scale defines the scaling vector
   */
  void setScale(const Vector3& scale);

  /**
   * @brief Gets the current scaling in local space.
   * @returns the current scaling vector
   */
  std::optional<Vector3>& getScale();

  /**
   * @brief Gets the current scaling in local space and stores it in a target
   * vector.
   * @param result defines the target vector
   */
  void getScaleToRef(Vector3& result);

  /**
   * @brief Set the yaw, pitch, and roll of the bone in local or world space.
   * @param yaw The rotation of the bone on the y axis
   * @param pitch The rotation of the bone on the x axis
   * @param roll The rotation of the bone on the z axis
   * @param space The space that the axes of rotation are in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   */
  void setYawPitchRoll(float yaw, float pitch, float roll,
                       Space space        = Space::LOCAL,
                       AbstractMesh* mesh = nullptr);

  /**
   * @brief Add a rotation to the bone on an axis in local or world space.
   * @param axis The axis to rotate the bone on
   * @param amount The amount to rotate the bone
   * @param space The space that the axis is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   */
  void rotate(Vector3& axis, float amount, Space space = Space::LOCAL,
              AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the rotation of the bone to a particular axis angle in local or
   * world space.
   * @param axis The axis to rotate the bone on
   * @param angle The angle that the bone should be rotated to
   * @param space The space that the axis is in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   */
  void setAxisAngle(Vector3& axis, float angle, Space space = Space::LOCAL,
                    AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the euler rotation of the bone in local of world space.
   * @param rotation The euler rotation that the bone should be set to
   * @param space The space that the rotation is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   */
  void setRotation(const Vector3& rotation, Space space = Space::LOCAL,
                   AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the quaternion rotation of the bone in local of world space.
   * @param quat The quaternion rotation that the bone should be set to
   * @param space The space that the rotation is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   */
  void setRotationQuaternion(const Quaternion& quat, Space space = Space::LOCAL,
                             AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the rotation matrix of the bone in local of world space.
   * @param rotMat The rotation matrix that the bone should be set to
   * @param space The space that the rotation is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   */
  void setRotationMatrix(const Matrix& rotMat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);

  /**
   * @brief Get the position of the bone in local or world space.
   * @param space The space that the returned position is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   * @returns The position of the bone
   */
  Vector3 getPosition(Space space        = Space::LOCAL,
                      AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the position of the bone to a vector3 in local or world space.
   * @param space The space that the returned position is in
   * @param mesh The mesh that this bone is attached to. This is only used in
   * world space
   * @param result The vector3 to copy the position to
   */
  void getPositionToRef(Vector3& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the absolute position of the bone (world space).
   * @param mesh The mesh that this bone is attached to
   * @returns The absolute position of the bone
   */
  Vector3 getAbsolutePosition(AbstractMesh* mesh) const;

  /**
   * @brief Copy the absolute position of the bone (world space) to the result
   * param.
   * @param mesh The mesh that this bone is attached to
   * @param result The vector3 to copy the absolute position to
   */
  void getAbsolutePositionToRef(AbstractMesh* mesh, Vector3& result) const;

  /**
   * @brief Compute the absolute transforms of this bone and its children.
   */
  void computeAbsoluteTransforms();

  /**
   * @brief Get the world direction from an axis that is in the local space of
   * the bone.
   * @param localAxis The local direction that is used to compute the world
   * direction
   * @param mesh The mesh that this bone is attached to
   * @returns The world direction
   */
  Vector3 getDirection(const Vector3& localAxis,
                       AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the world direction to a vector3 from an axis that is in the
   * local space of the bone.
   * @param localAxis The local direction that is used to compute the world
   * direction
   * @param mesh The mesh that this bone is attached to
   * @param result The vector3 that the world direction will be copied to
   */
  void getDirectionToRef(const Vector3& localAxis, Vector3& result,
                         AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the euler rotation of the bone in local or world space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @returns The euler rotation
   */
  Vector3& getRotation(Space space        = Space::LOCAL,
                       AbstractMesh* mesh = nullptr);

  /**
   * @brief Copy the euler rotation of the bone to a vector3.  The rotation can
   * be in either local or world space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @param result The vector3 that the rotation should be copied to
   */
  void getRotationToRef(Vector3& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr);

  /**
   * @brief Get the quaternion rotation of the bone in either local or world
   * space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @returns The quaternion rotation
   */
  Quaternion getRotationQuaternion(Space space        = Space::LOCAL,
                                   AbstractMesh* mesh = nullptr);

  /**
   * @brief Copy the quaternion rotation of the bone to a quaternion.  The
   * rotation can be in either local or world space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @param result The quaternion that the rotation should be copied to
   */
  void getRotationQuaternionToRef(Quaternion& result,
                                  const Space& space = Space::LOCAL,
                                  AbstractMesh* mesh = nullptr);

  /**
   * @brief Get the rotation matrix of the bone in local or world space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @returns The rotation matrix
   */
  Matrix getRotationMatrix(Space space        = Space::LOCAL,
                           AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the rotation matrix of the bone to a matrix.  The rotation can
   * be in either local or world space.
   * @param space The space that the rotation should be in
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space
   * @param result The quaternion that the rotation should be copied to
   */
  void getRotationMatrixToRef(Matrix& result, Space space = Space::LOCAL,
                              AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the world position of a point that is in the local space of the
   * bone.
   * @param position The local position
   * @param mesh The mesh that this bone is attached to
   * @returns The world position
   */
  Vector3 getAbsolutePositionFromLocal(const Vector3& position,
                                       AbstractMesh* mesh) const;

  /**
   * @brief Get the world position of a point that is in the local space of the
   * bone and copy it to the result param.
   * @param position The local position
   * @param mesh The mesh that this bone is attached to
   * @param result The vector3 that the world position should be copied to
   */
  void getAbsolutePositionFromLocalToRef(const Vector3& position,
                                         AbstractMesh* mesh,
                                         Vector3& result) const;

  /**
   * @brief Get the local position of a point that is in world space.
   * @param position The world position
   * @param mesh The mesh that this bone is attached to
   * @returns The local position
   */
  Vector3 getLocalPositionFromAbsolute(const Vector3& position,
                                       AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the local position of a point that is in world space and copy it
   * to the result param.
   * @param position The world position
   * @param mesh The mesh that this bone is attached to
   * @param result The vector3 that the local position should be copied to
   */
  void getLocalPositionFromAbsoluteToRef(const Vector3& position,
                                         AbstractMesh* mesh,
                                         Vector3& result) const;

protected:
  /**
   * @brief Create a new bone.
   * @param name defines the bone name
   * @param skeleton defines the parent skeleton
   * @param parentBone defines the parent (can be null if the bone is the root)
   * @param localMatrix defines the local matrix
   * @param restPose defines the rest pose matrix
   * @param baseMatrix defines the base matrix
   * @param index defines index of the bone in the hiearchy
   */
  Bone(const std::string& name, Skeleton* skeleton, Bone* parentBone = nullptr,
       const std::optional<Matrix>& localMatrix = std::nullopt,
       const std::optional<Matrix>& restPose    = std::nullopt,
       const std::optional<Matrix>& baseMatrix  = std::nullopt,
       std::optional<int> index                 = std::nullopt);

  /**
   * @brief Hidden
   */
  Matrix& get__matrix();

  /**
   * @brief Hidden
   */
  void set__matrix(const Matrix& value);

  /** Properties (matches AbstractMesh properties) **/

  /**
   * @brief Gets the node used to drive the bone's transformation.
   * @returns a transform node or null
   */
  TransformNodePtr& getTransformNode();

  /**
   * @brief Gets current position (in local space).
   */
  Vector3& get_position();

  /**
   * @brief Sets current position (in local space).
   */
  void set_position(const Vector3& newPosition);

  /**
   * @brief Gets current rotation (in local space).
   */
  Vector3& get_rotation();

  /**
   * @brief Sets current rotation (in local space).
   */
  void set_rotation(const Vector3& newRotation);

  /**
   * @brief Gets current rotation quaternion (in local space).
   */
  std::optional<Quaternion>& get_rotationQuaternion();

  /**
   * @brief Sets current rotation quaternion (in local space).
   */
  void set_rotationQuaternion(const std::optional<Quaternion>& newRotation);

  /**
   * @brief Gets current scaling (in local space).
   */
  std::optional<Vector3>& get_scaling();

  /**
   * @brief Sets current scaling (in local space).
   */
  void set_scaling(const std::optional<Vector3>& newScaling);

  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverridePtr& get_animationPropertiesOverride() override;

private:
  void _decompose();
  void _compose();
  void _markAsDirtyAndDecompose();
  void _rotateWithMatrix(const Matrix& rmat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);
  bool _getNegativeRotationToRef(Matrix& rotMatInv,
                                 AbstractMesh* mesh = nullptr);

public:
  /**
   * Gets the list of child bones
   */
  std::vector<BonePtr> children;

  /**
   * Gets or sets bone length
   */
  float length;

  /**
   * Hidden Internal only
   * Set this value to map this bone to a different index in the transform
   * matrices Set this value to -1 to exclude the bone from the transform
   * matrices
   */
  std::optional<int> _index;

  /** Hidden  */
  TransformNodePtr _linkedTransformNode;

  /** Hidden  */
  std::string _waitingTransformNodeId;

  /** Hidden  */
  Property<Bone, Matrix> _matrix;

  /**
   * Current position (in local space)
   */
  Property<Bone, Vector3> position;

  /**
   * Current rotation (in local space)
   */
  Property<Bone, Vector3> rotation;

  /**
   * Current rotation quaternion (in local space)
   */
  Property<Bone, std::optional<Quaternion>> rotationQuaternion;

  /**
   * Current scaling (in local space)
   */
  Property<Bone, std::optional<Vector3>> scaling;

private:
  static std::array<Vector3, 2> _tmpVecs;
  static Quaternion _tmpQuat;
  static std::array<Matrix, 5> _tmpMats;

private:
  Skeleton* _skeleton;
  Matrix _localMatrix;
  std::optional<Matrix> _restPose;
  Matrix _baseMatrix;
  Matrix _absoluteTransform;
  std::unique_ptr<Matrix> _invertedAbsoluteTransform;
  Bone* _parent;

  Matrix _scaleMatrix;
  Vector3 _scaleVector;
  Vector3 _negateScaleChildren;
  float _scalingDeterminant;
  Matrix _worldTransform;
  std::optional<Vector3> _localScaling;
  std::optional<Quaternion> _localRotation;
  std::optional<Vector3> _localPosition;
  bool _needToDecompose;
  bool _needToCompose;
  Vector3 _rotationTmp;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_BONE_H
