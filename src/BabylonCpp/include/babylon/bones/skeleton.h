#ifndef BABYLON_BONES_SKELETON_H
#define BABYLON_BONES_SKELETON_H

#include <nlohmann/json_fwd.hpp>
#include <unordered_map>

#include <babylon/animations/animation_range.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/matrix.h>
#include <babylon/misc/iinspectable.h>
#include <babylon/misc/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class Animatable;
struct AnimationPropertiesOverride;
class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Bone)
FWD_CLASS_SPTR(IAnimatable)
FWD_CLASS_SPTR(RawTexture)
FWD_CLASS_SPTR(Skeleton)

/**
 * @brief Class used to handle skinning animations.
 * @see https://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
 */
class BABYLON_SHARED_EXPORT Skeleton : public IAnimatable, public IDisposable {

public:
  template <typename... Ts>
  static SkeletonPtr New(Ts&&... args)
  {
    auto skeleton = std::shared_ptr<Skeleton>(new Skeleton(std::forward<Ts>(args)...));
    skeleton->addToScene(skeleton);

    return skeleton;
  }
  ~Skeleton() override; // = default

  [[nodiscard]] Type type() const override;

  void addToScene(const SkeletonPtr& newSkeleton);

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  [[nodiscard]] std::string getClassName() const;

  /**
   * @brief Returns an array containing the root bones.
   * @returns an array containing the root bones
   */
  [[nodiscard]] std::vector<BonePtr> getChildren() const;

  /** Members **/

  /**
   * @brief Gets the list of transform matrices to send to shaders (one matrix per bone).
   * @param mesh defines the mesh to use to get the root matrix (if needInitialSkinMatrix === true)
   * @returns a Float32Array containing matrices data
   */
  Float32Array& getTransformMatrices(AbstractMesh* mesh);

  /**
   * @brief Gets the list of transform matrices to send to shaders inside a texture (one matrix per
   * bone).
   * @param mesh defines the mesh to use to get the root matrix (if needInitialSkinMatrix === true)
   * @returns a raw texture containing the data
   */
  RawTexturePtr& getTransformMatrixTexture(AbstractMesh* mesh);

  /**
   * @brief Gets the current hosting scene.
   * @returns a scene object
   */
  Scene* getScene();

  /** Methods **/

  /**
   * @brief Gets a string representing the current skeleton data.
   * @param fullDetails defines a boolean indicating if we want a verbose version
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
  AnimationRangePtr getAnimationRange(const std::string& name);

  /**
   * @brief Gets the list of all animation ranges defined on this skeleton.
   * @returns an array
   */
  std::vector<AnimationRangePtr> getAnimationRanges() override;

  /**
   * @brief Copy animation range from a source skeleton.
   * This is not for a complete retargeting, only between very similar skeleton's with only possible
   * bone length differences
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
   * @param onAnimationEnd defines a callback which will be called when animation will end
   * @returns a new animatable
   */
  Animatable* beginAnimation(const std::string& name, bool loop = false, float speedRatio = 1.f,
                             const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Convert the keyframes for a range of animation on a skeleton to be relative to a given
   * reference frame.
   * @param skeleton defines the Skeleton containing the animation range to convert
   * @param referenceFrame defines the frame that keyframes in the range will be relative to
   * @param range defines the name of the AnimationRange belonging to the Skeleton to convert
   * @returns the original skeleton
   */
  static SkeletonPtr MakeAnimationAdditive(const SkeletonPtr& skeleton, int referenceFrame = 0,
                                           const std::string& range = "");

  void _markAsDirty();
  void _registerMeshWithPoseMatrix(AbstractMesh* mesh);
  void _unregisterMeshWithPoseMatrix(AbstractMesh* mesh);

  /**
   * @brief Build all resources required to render a skeleton.
   */
  void prepare();

  /**
   * @brief Gets the list of animatables currently running for this skeleton.
   * @returns an array of animatables
   */
  std::vector<IAnimatablePtr> getAnimatables() override;

  /**
   * @brief Hidden
   */
  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Clone the current skeleton.
   * @param name defines the name of the new skeleton
   * @param id defines the id of the new skeleton
   * @returns the new skeleton
   */
  [[nodiscard]] std::unique_ptr<Skeleton> clone(const std::string& name,
                                                const std::string& id) const;

  /**
   * @brief Enable animation blending for this skeleton.
   * @param blendingSpeed defines the blending speed to apply
   * @see https://doc.babylonjs.com/babylon101/animations#animation-blending
   */
  void enableBlending(float blendingSpeed = 0.01f);

  /**
   * @brief Releases all resources associated with the current skeleton.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the skeleton in a JSON object.
   * @returns a JSON object
   */
  [[nodiscard]] json serialize() const;

  // Statics

  /**
   * @brief Creates a new skeleton from serialized data.
   * @param parsedSkeleton defines the serialized data
   * @param scene defines the hosting scene
   * @returns a new skeleton
   */
  static SkeletonPtr Parse(const json& parsedSkeleton, Scene* scene);

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
  [[nodiscard]] Matrix* getPoseMatrix() const;

  /**
   * @brief Sorts bones per internal index.
   */
  void sortBones();

  /**
   * @brief Set the current local matrix as the restPose for all bones in the skeleton.
   */
  void setCurrentPoseAsRest();

protected:
  /**
   * @brief Creates a new skeleton.
   * @param name defines the skeleton name
   * @param id defines the skeleton Id
   * @param scene defines the hosting scene
   */
  Skeleton(const std::string& name, const std::string& id, Scene* scene);

  /**
   * @brief Gets or sets a boolean indicating that bone matrices should be stored as a texture
   * instead of using shader uniforms (default is true).
   * Please note that this option is not available if the hardware does not support it
   */
  [[nodiscard]] bool get_useTextureToStoreBoneMatrices() const;

  /**
   * @brief Sets a boolean indicating that bone matrices should be stored as a texture instead of
   * using shader uniforms (default is true). Please note that this option is not available if the
   * hardware does not support it
   */
  void set_useTextureToStoreBoneMatrices(bool value);

  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverridePtr& get_animationPropertiesOverride() override;

  /**
   * @brief Sets the animation properties override.
   */
  void set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value) override;

  /**
   * @brief Gets a boolean indicating that the skeleton effectively stores
   * matrices into a texture.
   */
  [[nodiscard]] bool get_isUsingTextureForMatrices() const;

  /**
   * @brief Gets the unique ID of this skeleton.
   */
  [[nodiscard]] size_t get_uniqueId() const;

private:
  float _getHighestAnimationFrame();
  void _computeTransformMatrices(Float32Array& targetMatrix,
                                 const std::optional<Matrix>& initialSkinMatrix = std::nullopt);
  void _sortBones(unsigned int index, std::vector<BonePtr>& bones, std::vector<bool>& visited);

public:
  /**
   * Defines the list of child bones
   */
  std::vector<BonePtr> bones;

  /**
   * Defines an estimate of the dimension of the skeleton at rest
   */
  std::unique_ptr<Vector3> dimensionsAtRest;

  /**
   * Defines a boolean indicating if the root matrix is provided by meshes or by the current
   * skeleton (this is the default value)
   */
  bool needInitialSkinMatrix;

  /**
   * Defines a mesh that override the matrix used to get the world matrix (null by default).
   */
  AbstractMeshPtr overrideMesh;

  /**
   * Gets the list of animations attached to this skeleton
   */
  std::vector<Animation*> animations;

  std::string name;
  std::string id;

  /** Hidden */
  size_t _numBonesWithLinkedTransformNode;

  /** Hidden */
  std::optional<bool> _hasWaitingData;

  /** @hidden */
  std::string _waitingOverrideMeshId;

  /**
   * Specifies if the skeleton should be serialized
   */
  bool doNotSerialize;

  /**
   * Gets or sets a boolean indicating that bone matrices should be stored as a texture instead of
   * using shader uniforms (default is true). Please note that this option is not available if the
   * hardware does not support it
   */
  Property<Skeleton, bool> useTextureToStoreBoneMatrices;

  /**
   * List of inspectable custom properties (used by the Inspector)
   * @see https://doc.babylonjs.com/how_to/debug_layer#extensibility
   */
  std::vector<IInspectable> inspectableCustomProperties;

  // Events
  /**
   * An observable triggered before computing the skeleton's matrices
   */
  Observable<Skeleton> onBeforeComputeObservable;

  /**
   * Gets a boolean indicating that the skeleton effectively stores matrices
   * into a texture.
   */
  ReadOnlyProperty<Skeleton, bool> isUsingTextureForMatrices;

  /**
   * Gets the unique ID of this skeleton.
   */
  ReadOnlyProperty<Skeleton, size_t> uniqueId;

private:
  Scene* _scene;
  bool _isDirty;
  Float32Array _transformMatrices;
  RawTexturePtr _transformMatrixTexture;
  std::vector<AbstractMesh*> _meshesWithPoseMatrix;
  std::vector<IAnimatablePtr> _animatables;
  Matrix _identity;
  AbstractMesh* _synchronizedWithMesh;
  std::unordered_map<std::string, AnimationRangePtr> _ranges;
  int _lastAbsoluteTransformsUpdateId;
  bool _canUseTextureForBones;
  size_t _uniqueId;
  bool _useTextureToStoreBoneMatrices;
  AnimationPropertiesOverridePtr _animationPropertiesOverride;

}; // end of class Bone

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_SKELETON_H
