#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace GLTF2 {

class AbstractMesh;
class AnimationGroup;
class Bone;
class Geometry;
struct GLTFFileLoader;
struct GLTFLoaderExtension;
struct GLTFLoaderTracker;
struct IGLTFLoaderData;
struct IMesh;
struct IMeshPrimitive;
struct INode;
struct IScene;
struct ISkin;
class Matrix;
class Mesh;
class MorphTarget;
struct ProgressEvent;
class Skeleton;
class TransformNode;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;
using BonePtr           = std::shared_ptr<Bone>;
using GeometryPtr       = std::shared_ptr<Geometry>;
using GLTFFileLoaderPtr = std::shared_ptr<GLTFFileLoader>;
using MeshPtr           = std::shared_ptr<Mesh>;
using MorphTargetPtr    = std::shared_ptr<MorphTarget>;
using SkeletonPtr       = std::shared_ptr<Skeleton>;
using TransformNodePtr  = std::shared_ptr<TransformNode>;

/**
 * @brief The glTF 2.0 loader.
 */
class BABYLON_SHARED_EXPORT GLTFLoader {

public:
  /**
   * @brief Registers a loader extension.
   * @param name The name of the loader extension.
   * @param factory The factory function that creates the loader extension.
   */
  static void RegisterExtension(const std::string& name);

  /**
   * @brief Unregisters a loader extension.
   * @param name The name of the loader extenion.
   * @returns A boolean indicating whether the extension has been unregistered
   */
  static bool UnregisterExtension(const std::string& name);

public:
  ~GLTFLoader();

  /** Hidden */
  void dispose();

  /** Hidden */
  void importMeshAsync();

  /** Hidden */
  void loadAsync();

  /**
   * @brief Loads a glTF scene.
   * @param context The context when loading the asset
   * @param scene The glTF scene property
   * @returns A promise that resolves when the load is complete
   */
  void loadSceneAsync(const std::string& context, const IScene& scene);

  /**
   * @brief Loads a glTF node.
   * @param context The context when loading the asset
   * @param node The glTF node property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon mesh when the load
   * is complete
   */
  TransformNodePtr loadNodeAsync(
    const std::string& context, const INode& node,
    const std::function<void(const TransformNodePtr& babylonTransformNode)>&
      assign);

protected:
  /** Hidden */
  GLTFLoader(const GLTFFileLoaderPtr& parent);

private:
  /** Hidden */
  void _loadAsync();

  void _loadData();
  void _setupData();
  void _loadExtensions();
  void _checkExtensions();
  void _setState();
  INode _createRootNode();
  void _forEachPrimitive(
    const INode& node,
    const std::function<void(const AbstractMeshPtr& babylonMesh)>& callback);
  std::vector<AbstractMeshPtr> _getMeshes();
  std::vector<SkeletonPtr> _getSkeletons();
  std::vector<AnimationGroupPtr> _getAnimationGroups();
  void _startAnimations();
  TransformNodePtr _loadMeshAsync(
    const std::string& context, const INode& node, const IMesh& mesh,
    const std::function<void(const TransformNodePtr& babylonTransformNode)>&
      callback);
  AbstractMeshPtr _loadMeshPrimitiveAsync(
    const std::string& context, const std::string& name, const INode& node,
    const IMesh& mesh, const IMeshPrimitive& primitive,
    const std::function<void(const AbstractMeshPtr& babylonMesh)>& assign);
  GeometryPtr _loadVertexDataAsync(const std::string& context,
                                   IMeshPrimitive& primitive,
                                   const MeshPtr& babylonMesh);
  void _createMorphTargets(const std::string& context, const INode& node,
                           const IMesh& mesh, const IMeshPrimitive& primitive,
                           const MeshPtr& babylonMesh);
  void _loadMorphTargetsAsync(const std::string& context,
                              const IMeshPrimitive& primitive,
                              const MeshPtr& babylonMesh,
                              const GeometryPtr& babylonGeometry);
  void _loadMorphTargetVertexDataAsync(
    const std::string& context, const GeometryPtr& babylonGeometry,
    const std::unordered_map<std::string, size_t>& attributes,
    const MorphTargetPtr& babylonMorphTarget);
  static void _LoadTransform(const INode& node,
                             const TransformNodePtr& babylonNode);
  void _loadSkinAsync(const std::string& context, const INode& node,
                      const ISkin& skin);
  void _loadBones(const std::string& context, const ISkin& skin,
                  const SkeletonPtr& babylonSkeleton);
  BonePtr _loadBone(const INode& node, const ISkin& skin,
                    const Skeleton& babylonSkeleton,
                    const std::unordered_map<size_t, BonePtr>& babylonBones);
  Float32Array _loadSkinInverseBindMatricesDataAsync(const std::string& context,
                                                     const ISkin& skin);
  void _updateBoneMatrices(const SkeletonPtr& babylonSkeleton,
                           const Float32Array& inverseBindMatricesData);
  Matrix _getNodeMatrix(const INode& node);

}; // end of class GLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
