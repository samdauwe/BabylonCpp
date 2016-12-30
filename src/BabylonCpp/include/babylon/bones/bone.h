#ifndef BABYLON_BONES_BONE_H
#define BABYLON_BONES_BONE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/engine/node.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Bone : public Node {

public:
  template <typename... Ts>
  static Bone* New(Ts&&... args);
  ~Bone();

  virtual Node::Type type() const override;

  /** Members **/
  Bone* getParent();
  Matrix& getLocalMatrix();
  const Matrix& getLocalMatrix() const;
  Matrix& getBaseMatrix();
  Matrix& getRestPose();
  void returnToRest();
  Matrix* getWorldMatrix() override;
  Matrix& getInvertedAbsoluteTransform();
  Matrix& getAbsoluteTransform();
  const Matrix& getAbsoluteTransform() const;

  /** Methods **/
  std::vector<Animation*> getAnimations() override;
  void updateMatrix(const Matrix& matrix, bool updateDifferenceMatrix = true);
  void _updateDifferenceMatrix();
  void _updateDifferenceMatrix(Matrix& rootMatrix);
  void markAsDirty(const std::string& property = "") override;
  bool copyAnimationRange(Bone* source, const std::string& rangeName,
                          int frameOffset, bool rescaleAsRequired = false,
                          const Vector3& skelDimensionsRatio = Vector3(),
                          bool hasSkelDimensionsRatio        = false);
  void translate(const Vector3& vec, Space space = Space::LOCAL,
                 AbstractMesh* mesh = nullptr);
  void setPosition(const Vector3& position, Space space = Space::LOCAL,
                   AbstractMesh* mesh = nullptr);
  void setAbsolutePosition(const Vector3& position,
                           AbstractMesh* mesh = nullptr);
  void setScale(float x, float y, float z, bool scaleChildren = false);
  void scale(float x, float y, float z, bool scaleChildren = false);
  void setYawPitchRoll(float yaw, float pitch, float roll,
                       Space space        = Space::LOCAL,
                       AbstractMesh* mesh = nullptr);
  void rotate(Vector3& axis, float amount, Space space = Space::LOCAL,
              AbstractMesh* mesh = nullptr);
  void setAxisAngle(Vector3& axis, float angle, Space space = Space::LOCAL,
                    AbstractMesh* mesh = nullptr);
  void setRotationMatrix(const Matrix& rotMat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);
  Vector3 getScale() const;
  void getScaleToRef(Vector3& result) const;
  Vector3 getPosition(Space space        = Space::LOCAL,
                      AbstractMesh* mesh = nullptr) const;
  void getPositionToRef(Vector3& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr) const;
  Vector3 getAbsolutePosition(AbstractMesh* mesh) const;
  void getAbsolutePositionToRef(AbstractMesh* mesh, Vector3& result) const;
  void computeAbsoluteTransforms();
  Vector3 getDirection(const Vector3& localAxis, AbstractMesh* mesh = nullptr);
  void getDirectionToRef(const Vector3& localAxis, Vector3& result,
                         AbstractMesh* mesh = nullptr);
  Quaternion getRotation(Space space        = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);
  void getRotationToRef(Quaternion& result, Space space = Space::LOCAL,
                        AbstractMesh* mesh = nullptr);

protected:
  Bone(const std::string& name, Skeleton* skeleton, Bone* parentBone,
       const Matrix& matrix);
  Bone(const std::string& name, Skeleton* skeleton, Bone* parentBone,
       const Matrix& matrix, const Matrix& restPose);

private:
  void _rotateWithMatrix(const Matrix& rmat, Space space = Space::LOCAL,
                         AbstractMesh* mesh = nullptr);
  void _getNegativeRotationToRef(Matrix& rotMatInv, Space space = Space::LOCAL,
                                 AbstractMesh* mesh = nullptr);
  void _syncScaleVector();

public:
  std::vector<Bone*> children;
  std::vector<Animation*> animations;
  Matrix _matrix;
  int length;

private:
  Skeleton* _skeleton;
  Matrix _restPose;
  Matrix _baseMatrix;
  std::unique_ptr<Matrix> _worldTransform;
  Matrix _absoluteTransform;
  std::unique_ptr<Matrix> _invertedAbsoluteTransform;
  Bone* _parent;

  Matrix _scaleMatrix;
  Vector3 _scaleVector;
  Vector3 _negateScaleChildren;
  float _scalingDeterminant;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_BONE_H
