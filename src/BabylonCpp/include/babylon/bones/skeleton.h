#ifndef BABYLON_BONES_SKELETON_H
#define BABYLON_BONES_SKELETON_H

#include <babylon/animations/animation_range.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Skeleton
  : public std::enable_shared_from_this<Skeleton>,
    public IAnimatable,
    public IDisposable {

public:
  Skeleton(const std::string& name, const std::string& id, Scene* scene);
  virtual ~Skeleton();

  virtual IReflect::Type type() const override;

  /** Members **/
  Float32Array& getTransformMatrices(AbstractMesh* mesh);
  Scene* getScene();

  /** Methods **/

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  std::string toString(bool fullDetails = false);

  /**
   * Get bone's index searching by name
   * @param {string} name is bone's name to search for
   * @return {number} Indice of the bone. Returns -1 if not found
   */
  int getBoneIndexByName(const std::string& name);

  void createAnimationRange(const std::string& name, int from, int to);
  void deleteAnimationRange(const std::string& name, bool deleteFrames = true);
  AnimationRange* getAnimationRange(const std::string& name);

  /**
   *  Returns as an Array, all AnimationRanges defined on this skeleton
   */
  std::vector<AnimationRange> getAnimationRanges();

  /**
   *  note: This is not for a complete retargeting, only between very similar
   * skeleton's with only possible bone length differences
   */
  bool copyAnimationRange(Skeleton* source, const std::string& name,
                          bool rescaleAsRequired = false);

  void returnToRest();
  Animatable* beginAnimation(const std::string& name, bool loop = false,
                             float speedRatio = 1.f,
                             const std::function<void()>& onAnimationEnd
                             = nullptr);

  void _markAsDirty();
  void _registerMeshWithPoseMatrix(AbstractMesh* mesh);
  void _unregisterMeshWithPoseMatrix(AbstractMesh* mesh);
  void _computeTransformMatrices(Float32Array& targetMatrix);
  void _computeTransformMatrices(Float32Array& targetMatrix,
                                 const Matrix& initialSkinMatrix,
                                 bool initialSkinMatrixSet = true);
  void prepare();
  std::vector<IAnimatable*> getAnimatables();
  std::vector<Animation*> getAnimations() override;
  std::unique_ptr<Skeleton> clone(const std::string& name,
                                  const std::string& id) const;
  void enableBlending(float blendingSpeed = 0.01f);
  void dispose(bool doNotRecurse = false) override;
  Json::object serialize() const;

  // Statics
  static Skeleton* Parse(const Json::value& parsedSkeleton, Scene* scene);

  void computeAbsoluteTransforms(bool forceUpdate = false);
  Matrix* getPoseMatrix() const;

private:
  int _getHighestAnimationFrame();

public:
  std::vector<std::unique_ptr<Bone>> bones;
  std::unique_ptr<Vector3> dimensionsAtRest;
  bool needInitialSkinMatrix;
  std::string name;
  std::string id;
  // Events
  /**
   * An event triggered before computing the skeleton's matrices
   */
  Observable<Skeleton> onBeforeComputeObservable;

private:
  Scene* _scene;
  bool _isDirty;
  Float32Array _transformMatrices;
  std::vector<AbstractMesh*> _meshesWithPoseMatrix;
  std::vector<IAnimatable*> _animatables;
  Matrix _identity;
  AbstractMesh* _synchronizedWithMesh;
  std::unordered_map<std::string, AnimationRange> _ranges;
  int _lastAbsoluteTransformsUpdateId;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_SKELETON_H
