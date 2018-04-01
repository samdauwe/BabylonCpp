#ifndef BABYLON_BONES_BONE_H
#define BABYLON_BONES_BONE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/nullable.h>
#include <babylon/engine/node.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Bone : public Node {

public:
  template <typename... Ts>
  static Bone* New(Ts&&... args)
  {
    auto bone = new Bone(std::forward<Ts>(args)...);
    bone->addToSkeleton(static_cast<unique_ptr_t<Bone>>(bone));
    return bone;
  }
  ~Bone() override;

  virtual IReflect::Type type() const override;
  void addToSkeleton(unique_ptr_t<Bone>&& newBone);

  /** Members **/
  Matrix& _matrix();
  const Matrix& _matrix() const;
  void setMatrix(const Matrix& val);
  Skeleton* getSkeleton() const;
  Bone* getParent();
  void setParent(Bone* parent, bool updateDifferenceMatrix = true);
  Matrix& getLocalMatrix();
  const Matrix& getLocalMatrix() const;
  Matrix& getBaseMatrix();
  Matrix& getRestPose();
  void returnToRest();
  Matrix* getWorldMatrix() override;
  Matrix& getInvertedAbsoluteTransform();
  Matrix& getAbsoluteTransform();
  const Matrix& getAbsoluteTransform() const;

  /** Properties (matches AbstractMesh properties) **/
  Vector3 position() const;
  Vector3 rotation() const;
  Quaternion rotationQuaternion() const;
  Vector3 scaling() const;
  void setScaling(const Vector3& newScaling);

  /** Methods **/
  vector_t<Animation*> getAnimations() override;

  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverride* animationPropertiesOverride() const;

  void updateMatrix(const Matrix& matrix, bool updateDifferenceMatrix = true);
  void _updateDifferenceMatrix();
  void _updateDifferenceMatrix(Matrix& rootMatrix);
  void markAsDirty(unsigned int property = 0) override;
  bool copyAnimationRange(Bone* source, const string_t& rangeName,
                          int frameOffset, bool rescaleAsRequired = false,
                          const Vector3& skelDimensionsRatio = Vector3(),
                          bool hasSkelDimensionsRatio        = false);

  /**
   * @brief Translate the bone in local or world space.
   * @param vec The amount to translate the bone.
   * @param space The space that the translation is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void translate(const Vector3& vec, Space space = Space::LOCAL,
                 AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the postion of the bone in local or world space.
   * @param position The position to set the bone.
   * @param space The space that the position is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setPosition(const Vector3& position, Space space = Space::LOCAL,
                   AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the absolute postion of the bone (world space).
   * @param position The position to set the bone.
   * @param mesh The mesh that this bone is attached to.
   */
  void setAbsolutePosition(const Vector3& position,
                           AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the scale of the bone on the x, y and z axes.
   * @param x The scale of the bone on the x axis.
   * @param x The scale of the bone on the y axis.
   * @param z The scale of the bone on the z axis.
   * @param scaleChildren Set this to true if children of the bone should be
   * scaled.
   */
  void setScale(float x, float y, float z, bool scaleChildren = false);

  /**
   * @brief Scale the bone on the x, y and z axes.
   * @param x The amount to scale the bone on the x axis.
   * @param x The amount to scale the bone on the y axis.
   * @param z The amount to scale the bone on the z axis.
   * @param scaleChildren Set this to true if children of the bone should be
   * scaled.
   */
  void scale(float x, float y, float z, bool scaleChildren = false);

  /**
   * @brief Set the yaw, pitch, and roll of the bone in local or world space.
   * @param yaw The rotation of the bone on the y axis.
   * @param pitch The rotation of the bone on the x axis.
   * @param roll The rotation of the bone on the z axis.
   * @param space The space that the axes of rotation are in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setYawPitchRoll(float yaw, float pitch, float roll,
                       Space space        = Space::LOCAL,
                       AbstractMesh* mesh = nullptr);

  /**
   * @brief Rotate the bone on an axis in local or world space.
   * @param axis The axis to rotate the bone on.
   * @param amount The amount to rotate the bone.
   * @param space The space that the axis is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void rotate(Vector3& axis, float amount, Space space = Space::LOCAL,
              AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the rotation of the bone to a particular axis angle in local or
   * world space.
   * @param axis The axis to rotate the bone on.
   * @param angle The angle that the bone should be rotated to.
   * @param space The space that the axis is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setAxisAngle(Vector3& axis, float angle, Space space = Space::LOCAL,
                    AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the euler rotation of the bone in local of world space.
   * @param rotation The euler rotation that the bone should be set to.
   * @param space The space that the rotation is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setRotation(const Vector3& rotation, Space space = Space::LOCAL,
                   AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the quaternion rotation of the bone in local of world space.
   * @param quat The quaternion rotation that the bone should be set to.
   * @param space The space that the rotation is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setRotationQuaternion(const Quaternion& quat, Space space = Space::LOCAL,
                             AbstractMesh* mesh = nullptr);

  /**
   * @brief Set the rotation matrix of the bone in local of world space.
   * @param rotMat The rotation matrix that the bone should be set to.
   * @param space The space that the rotation is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void setRotationMatrix(const Matrix& rotMat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);

  /**
   * @brief Get the scale of the bone
   * @returns the scale of the bone
   */
  Vector3 getScale() const;

  /**
   * @brief Copy the scale of the bone to a vector3.
   * @param result The vector3 to copy the scale to
   */
  void getScaleToRef(Vector3& result) const;

  /**
   * @brief Get the position of the bone in local or world space.
   * @param space The space that the returned position is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @returns The position of the bone
   */
  Vector3 getPosition(Space space        = Space::LOCAL,
                      AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the position of the bone to a vector3 in local or world space.
   * @param space The space that the returned position is in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @param result The vector3 to copy the position to.
   */
  void getPositionToRef(Vector3& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the absolute position of the bone (world space).
   * @param mesh The mesh that this bone is attached to.
   * @returns The absolute position of the bone
   */
  Vector3 getAbsolutePosition(AbstractMesh* mesh) const;

  /**
   * @brief Copy the absolute position of the bone (world space) to the result
   * param.
   * @param mesh The mesh that this bone is attached to.
   * @param result The vector3 to copy the absolute position to.
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
   * direction.
   * @param mesh The mesh that this bone is attached to.
   * @returns The world direction
   */
  Vector3 getDirection(const Vector3& localAxis,
                       AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the world direction to a vector3 from an axis that is in the
   * local space of the bone.
   * @param localAxis The local direction that is used to compute the world
   * direction.
   * @param mesh The mesh that this bone is attached to.
   * @param result The vector3 that the world direction will be copied to.
   */
  void getDirectionToRef(const Vector3& localAxis, Vector3& result,
                         AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the euler rotation of the bone in local or world space.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @returns The euler rotation
   */
  Vector3 getRotation(Space space        = Space::LOCAL,
                      AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the euler rotation of the bone to a vector3.  The rotation can
   * be in either local or world space.
   * @param result The vector3 that the rotation should be copied to.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void getRotationToRef(Vector3& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the quaternion rotation of the bone in either local or world
   * space.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @returns The quaternion rotation
   */
  Quaternion getRotationQuaternion(Space space        = Space::LOCAL,
                                   AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the quaternion rotation of the bone to a quaternion.  The
   * rotation can be in either local or world space.
   * @param result The quaternion that the rotation should be copied to.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   */
  void getRotationQuaternionToRef(Quaternion& result,
                                  const Space& space = Space::LOCAL,
                                  AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the rotation matrix of the bone in local or world space.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @returns The rotation matrix
   */
  Matrix getRotationMatrix(Space space        = Space::LOCAL,
                           AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Copy the rotation matrix of the bone to a matrix.  The rotation can
   * be in either local or world space.
   * @param space The space that the rotation should be in.
   * @param mesh The mesh that this bone is attached to.  This is only used in
   * world space.
   * @param result The quaternion that the rotation should be copied to.
   */
  void getRotationMatrixToRef(Matrix& result, Space space = Space::LOCAL,
                              AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the world position of a point that is in the local space of the
   * bone.
   * @param position The local position
   * @param mesh The mesh that this bone is attached to.
   * @returns The world position
   */
  Vector3 getAbsolutePositionFromLocal(const Vector3& position,
                                       AbstractMesh* mesh) const;

  /**
   * @brief Get the world position of a point that is in the local space of the
   * bone and copy it to the result param.
   * @param position The local position
   * @param mesh The mesh that this bone is attached to.
   * @param result The vector3 that the world position should be copied to.
   */
  void getAbsolutePositionFromLocalToRef(const Vector3& position,
                                         AbstractMesh* mesh,
                                         Vector3& result) const;

  /**
   * @brief Get the local position of a point that is in world space.
   * @param position The world position
   * @param mesh The mesh that this bone is attached to.
   * @returns The local position
   */
  Vector3 getLocalPositionFromAbsolute(const Vector3& position,
                                       AbstractMesh* mesh = nullptr) const;

  /**
   * @brief Get the local position of a point that is in world space and copy it
   * to the result param.
   * @param position The world position
   * @param mesh The mesh that this bone is attached to.
   * @param result The vector3 that the local position should be copied to.
   */
  void getLocalPositionFromAbsoluteToRef(const Vector3& position,
                                         AbstractMesh* mesh,
                                         Vector3& result) const;

protected:
  Bone(const string_t& name, Skeleton* skeleton, Bone* parentBone = nullptr,
       const Nullable<Matrix>& localMatrix = nullptr,
       const Nullable<Matrix>& restPose    = nullptr,
       const Nullable<Matrix>& baseMatrix  = nullptr,
       Nullable<int> index                 = nullptr);

private:
  void _rotateWithMatrix(const Matrix& rmat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);
  void _getNegativeRotationToRef(Matrix& rotMatInv, Space space = Space::LOCAL,
                                 AbstractMesh* mesh = nullptr);
  void _syncScaleVector();

public:
  vector_t<Bone*> children;
  int length;

  // Set this value to map this bone to a different index in the transform
  // matrices.
  // Set this value to -1 to exclude the bone from the transform matrices.
  Nullable<int> _index;

private:
  static array_t<Vector3, 2> _tmpVecs;
  static Quaternion _tmpQuat;
  static array_t<Matrix, 5> _tmpMats;

private:
  Skeleton* _skeleton;
  Matrix _localMatrix;
  Matrix _restPose;
  Matrix _baseMatrix;
  unique_ptr_t<Matrix> _worldTransform;
  Matrix _absoluteTransform;
  unique_ptr_t<Matrix> _invertedAbsoluteTransform;
  Bone* _parent;

  Matrix _scaleMatrix;
  Vector3 _scaleVector;
  Vector3 _negateScaleChildren;
  float _scalingDeterminant;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_BONE_H
