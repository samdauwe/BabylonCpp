#ifndef BABYLON_BONES_SKELETON_H
#define BABYLON_BONES_SKELETON_H

#include <map>

#include <babylon/animations/animation_range.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/observable.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class Animatable;
struct AnimationPropertiesOverride;
class Bone;
class IAnimatable;
class Scene;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;
using BonePtr        = std::shared_ptr<Bone>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief Class used to handle skinning animations.
 * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
 */
class BABYLON_SHARED_EXPORT Skeleton
    : public std::enable_shared_from_this<Skeleton>,
      public IAnimatable,
      public IDisposable {

public:
  /**
   * @brief Creates a new skeleton.
   * @param name defines the skeleton name
   * @param id defines the skeleton Id
   * @param scene defines the hosting scene
   */
  Skeleton(const std::string& name, const std::string& id, Scene* scene);
  virtual ~Skeleton() override;

  virtual IReflect::Type type() const override;

  /** Members **/

  /**
   * @brief Gets the list of transform matrices to send to shaders (one matrix
   * per bone).
   * @param mesh defines the mesh to use to get the root matrix (if
   * needInitialSkinMatrix === true)
   * @returns a Float32Array containing matrices data
   */
  Float32Array& getTransformMatrices(AbstractMesh* mesh);

  /**
   * @brief Gets the current hosting scene.
   * @returns a scene object
   */
  Scene* getScene();

  /** Methods **/

  /**
   * @brief Gets a string representing the current skeleton data.
   * @param fullDetails defines a boolean indicating if we want a verbose
   * version
   * @returns a string representing the current skeleton data
   */
  std::string toString(bool fullDetails = false);

  /**
   * @brief Get bone's index searching by name.
   * @param name defines bone's name to search for
   * @return the indice of the bone. Returns -1 if not found
   */
  int getBoneIndexByName(const std::string& name);

  /**
   * @brief Creater a new animation range.
   * @param name defines the name of the range
   * @param from defines the start key
   * @param to defines the end key
   */
  void createAnimationRange(const std::string& name, float from, float to);

  /**
   * @brief Delete a specific animation range.
   * @param name defines the name of the range
   * @param deleteFrames defines if frames must be removed as well
   */
  void deleteAnimationRange(const std::string& name, bool deleteFrames = true);

  /**
   * @brief Gets a specific animation range.
   * @param name defines the name of the range to look for
   * @returns the requested animation range or null if not found
   */
  AnimationRange* getAnimationRange(const std::string& name);

  /**
   * @brief Gets the list of all animation ranges defined on this skeleton.
   * @returns an array
   */
  std::vector<AnimationRange> getAnimationRanges();

  /**
   * @brief Copy animation range from a source skeleton.
   * This is not for a complete retargeting, only between very similar
   * skeleton's with only possible bone length differences
   * @param source defines the source skeleton
   * @param name defines the name of the range to copy
   * @param rescaleAsRequired defines if rescaling must be applied if required
   * @returns true if operation was successful
   */
  bool copyAnimationRange(Skeleton* source, const std::string& name,
                          bool rescaleAsRequired = false);

  /**
   * @brief Forces the skeleton to go to rest pose.
   */
  void returnToRest();

  /**
   * @brief Begin a specific animation range.
   * @param name defines the name of the range to start
   * @param loop defines if looping must be turned on (false by default)
   * @param speedRatio defines the speed ratio to apply (1 by default)
   * @param onAnimationEnd defines a callback which will be called when
   * animation will end
   * @returns a new animatable
   */
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

  /**
   * @brief Build all resources required to render a skeleton.
   */
  void prepare();

  /**
   * @brief Gets the list of animatables currently running for this skeleton.
   * @returns an array of animatables
   */
  std::vector<IAnimatablePtr> getAnimatables();

  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Clone the current skeleton.
   * @param name defines the name of the new skeleton
   * @param id defines the id of the enw skeleton
   * @returns the new skeleton
   */
  std::unique_ptr<Skeleton> clone(const std::string& name,
                                  const std::string& id) const;

  /**
   * @brief Enable animation blending for this skeleton.
   * @param blendingSpeed defines the blending speed to apply
   * @see http://doc.babylonjs.com/babylon101/animations#animation-blending
   */
  void enableBlending(float blendingSpeed = 0.01f);

  /**
   * @brief Releases all resources associated with the current skeleton.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the skeleton in a JSON object.
   * @returns a JSON object
   */
  Json::object serialize() const;

  // Statics

  /**
   * @brief Creates a new skeleton from serialized data.
   * @param parsedSkeleton defines the serialized data
   * @param scene defines the hosting scene
   * @returns a new skeleton
   */
  static Skeleton* Parse(const Json::value& parsedSkeleton, Scene* scene);

  /**
   * @brief Compute all node absolute transforms.
   * @param forceUpdate defines if computation must be done even if cache is up
   * to date
   */
  void computeAbsoluteTransforms(bool forceUpdate = false);

  /**
   * @brief Gets the root pose matrix.
   * @returns a matrix
   */
  Matrix* getPoseMatrix() const;

  /**
   * @brief Sorts bones per internal index.
   */
  void sortBones();

protected:
  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverride*& get_animationPropertiesOverride();

  /**
   * @brief Sets the animation properties override.
   */
  void
  set_animationPropertiesOverride(AnimationPropertiesOverride* const& value);

private:
  float _getHighestAnimationFrame();
  void _sortBones(unsigned int index, std::vector<Bone*>& bones,
                  std::vector<bool>& visited);

public:
  /**
   * Gets the list of child bones
   */
  std::vector<BonePtr> bones;

  /**
   * Gets an estimate of the dimension of the skeleton at rest
   */
  std::unique_ptr<Vector3> dimensionsAtRest;

  /**
   * Gets a boolean indicating if the root matrix is provided by meshes or by
   * the current skeleton (this is the default value)
   */
  bool needInitialSkinMatrix;

  std::vector<Animation*> animations;
  std::string name;
  std::string id;

  /**
   * Specifies if the skeleton should be serialized
   */
  bool doNotSerialize;

  /**
   * Animation properties override
   */
  Property<Skeleton, AnimationPropertiesOverride*> animationPropertiesOverride;

  // Events
  /**
   * An observable triggered before computing the skeleton's matrices
   */
  Observable<Skeleton> onBeforeComputeObservable;

private:
  Scene* _scene;
  bool _isDirty;
  Float32Array _transformMatrices;
  std::vector<AbstractMesh*> _meshesWithPoseMatrix;
  std::vector<IAnimatablePtr> _animatables;
  Matrix _identity;
  AbstractMesh* _synchronizedWithMesh;
  std::unordered_map<std::string, AnimationRange> _ranges;
  int _lastAbsoluteTransformsUpdateId;
  AnimationPropertiesOverride* _animationPropertiesOverride;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_SKELETON_H
