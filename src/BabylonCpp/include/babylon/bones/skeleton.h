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
    : public ::std::enable_shared_from_this<Skeleton>,
      public IAnimatable,
      public IDisposable {

public:
  Skeleton(const string_t& name, const string_t& id, Scene* scene);
  virtual ~Skeleton() override;

  virtual IReflect::Type type() const override;

  /** Members **/
  Float32Array& getTransformMatrices(AbstractMesh* mesh);
  Scene* getScene();

  /** Methods **/

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  string_t toString(bool fullDetails = false);

  /**
   * Get bone's index searching by name
   * @param {string} name is bone's name to search for
   * @return {number} Indice of the bone. Returns -1 if not found
   */
  int getBoneIndexByName(const string_t& name);

  void createAnimationRange(const string_t& name, float from, float to);
  void deleteAnimationRange(const string_t& name, bool deleteFrames = true);
  AnimationRange* getAnimationRange(const string_t& name);

  /**
   *  Returns as an Array, all AnimationRanges defined on this skeleton
   */
  vector_t<AnimationRange> getAnimationRanges();

  /**
   *  note: This is not for a complete retargeting, only between very similar
   * skeleton's with only possible bone length differences
   */
  bool copyAnimationRange(Skeleton* source, const string_t& name,
                          bool rescaleAsRequired = false);

  void returnToRest();
  Animatable* beginAnimation(const string_t& name, bool loop = false,
                             float speedRatio = 1.f,
                             const ::std::function<void()>& onAnimationEnd
                             = nullptr);

  void _markAsDirty();
  void _registerMeshWithPoseMatrix(AbstractMesh* mesh);
  void _unregisterMeshWithPoseMatrix(AbstractMesh* mesh);
  void _computeTransformMatrices(Float32Array& targetMatrix);
  void _computeTransformMatrices(Float32Array& targetMatrix,
                                 const Matrix& initialSkinMatrix,
                                 bool initialSkinMatrixSet = true);
  void prepare();
  vector_t<IAnimatable*> getAnimatables();
  vector_t<Animation*> getAnimations() override;
  unique_ptr_t<Skeleton> clone(const string_t& name, const string_t& id) const;
  void enableBlending(float blendingSpeed = 0.01f);
  void dispose(bool doNotRecurse = false) override;
  Json::object serialize() const;

  // Statics
  static Skeleton* Parse(const Json::value& parsedSkeleton, Scene* scene);

  void computeAbsoluteTransforms(bool forceUpdate = false);
  Matrix* getPoseMatrix() const;
  void sortBones();

private:
  int _getHighestAnimationFrame();
  void _sortBones(unsigned int index, vector_t<Bone*>& bones,
                  vector_t<bool>& visited);

public:
  vector_t<unique_ptr_t<Bone>> bones;
  unique_ptr_t<Vector3> dimensionsAtRest;
  bool needInitialSkinMatrix;
  vector_t<Animation*> animations;
  string_t name;
  string_t id;
  // Events
  /**
   * An event triggered before computing the skeleton's matrices
   */
  Observable<Skeleton> onBeforeComputeObservable;

private:
  Scene* _scene;
  bool _isDirty;
  Float32Array _transformMatrices;
  vector_t<AbstractMesh*> _meshesWithPoseMatrix;
  vector_t<IAnimatable*> _animatables;
  Matrix _identity;
  AbstractMesh* _synchronizedWithMesh;
  unordered_map_t<string_t, AnimationRange> _ranges;
  int _lastAbsoluteTransformsUpdateId;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_SKELETON_H
