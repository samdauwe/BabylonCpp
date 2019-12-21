#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/data_view.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>
#include <babylon/loading/glTF/igltf_loader.h>

using json = nlohmann::json;

template <typename T>
struct is_shared_ptr : std::false_type {
};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {
};

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
struct ArrayBufferView;
class BaseTexture;
class Bone;
class Buffer;
class Camera;
class Geometry;
class IAnimatable;
class Material;
class Matrix;
class Mesh;
class MorphTarget;
class Skeleton;
class TransformNode;
class VertexBuffer;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;
using BaseTexturePtr    = std::shared_ptr<BaseTexture>;
using BonePtr           = std::shared_ptr<Bone>;
using BufferPtr         = std::shared_ptr<Buffer>;
using CameraPtr         = std::shared_ptr<Camera>;
using GeometryPtr       = std::shared_ptr<Geometry>;
using IAnimatablePtr    = std::shared_ptr<IAnimatable>;
using MaterialPtr       = std::shared_ptr<Material>;
using MeshPtr           = std::shared_ptr<Mesh>;
using MorphTargetPtr    = std::shared_ptr<MorphTarget>;
using SkeletonPtr       = std::shared_ptr<Skeleton>;
using TransformNodePtr  = std::shared_ptr<TransformNode>;
using VertexBufferPtr   = std::shared_ptr<VertexBuffer>;

namespace GLTF2 {

struct IGLTFLoaderExtension;
class GLTFFileLoader;
class GLTFLoader;
using IGLTFLoaderExtensionPtr = std::shared_ptr<IGLTFLoaderExtension>;
using GLTFLoaderPtr           = std::shared_ptr<GLTFLoader>;

/**
 * @brief Helper class for working with arrays when loading the glTF asset.
 */
struct BABYLON_SHARED_EXPORT ArrayItem {

  /**
   * @brief Gets an item from the given array.
   * @param context The context when loading the asset
   * @param array The array to get the item from
   * @param index The index to the array
   * @returns The array item
   */
  template <typename T>
  static T& Get(const std::string& context, std::vector<T>& array, size_t index = 0)
  {
    if (array.empty() || index >= array.size()) {
      throw std::runtime_error(context + ": Failed to find index (" + std::to_string(index) + ")");
    }

    return array[index];
  }

  /**
   * @brief Assign an `index` field to each item of the given array.
   * @param array The array of items
   */
  template <typename IArrItem>
  static typename std::enable_if<!is_shared_ptr<decltype(IArrItem())>::value,
                                 void>::type // non shared_ptr version
  Assign(std::vector<IArrItem>& array)
  {
    if (!array.empty()) {
      for (size_t index = 0; index < array.size(); ++index) {
        array[index].index = index;
      }
    }
  }

  /**
   * @brief Assign an `index` field to each item of the given array.
   * @param array The array of items
   */
  template <typename IArrItem>
  static typename std::enable_if<!is_shared_ptr<decltype(IArrItem())>::value,
                                 void>::type // shared_ptr version
  Assign(std::vector<std::shared_ptr<IArrItem>>& array)
  {
    if (!array.empty()) {
      for (size_t index = 0; index < array.size(); ++index) {
        array[index]->index = index;
      }
    }
  }

}; // end of struct ArrayItem

/**
 * @brief The glTF 2.0 loader.
 */
class BABYLON_SHARED_EXPORT GLTFLoader : public IGLTFLoader {

public:
  /**
   * @brief Registers a loader extension.
   * @param name The name of the loader extension.
   * @param factory The factory function that creates the loader extension.
   */
  static void
  RegisterExtension(const std::string& name,
                    const std::function<IGLTFLoaderExtensionPtr(GLTFLoader& loader)>& factory);

  /**
   * @brief Unregisters a loader extension.
   * @param name The name of the loader extenion.
   * @returns A boolean indicating whether the extension has been unregistered
   */
  static bool UnregisterExtension(const std::string& name);

private:
  static std::vector<std::string> _ExtensionNames;
  static std::unordered_map<std::string, std::function<IGLTFLoaderExtensionPtr(GLTFLoader& loader)>>
    _ExtensionFactories;

public:
  template <typename... Ts>
  static GLTFLoaderPtr New(Ts&&... args)
  {
    return std::shared_ptr<GLTFLoader>(new GLTFLoader(std::forward<Ts>(args)...));
  }
  ~GLTFLoader() override; // = default

  /**
   * @brief Gets the loader state.
   */
  GLTFLoaderState& state();

  /**
   * The glTF object parsed from the JSON.
   */
  std::unique_ptr<IGLTF>& gltf();

  /**
   * The Babylon scene when loading the asset.
   */
  Scene* babylonScene();

  /**
   * The root Babylon mesh when loading the asset.
   */
  MeshPtr rootBabylonMesh();

  /** Hidden */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /** Hidden */
  ImportedMeshes
  importMeshAsync(const std::vector<std::string>& meshesNames, Scene* scene,
                  const IGLTFLoaderData& data, const std::string& rootUrl,
                  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
                  = nullptr,
                  const std::string& fileName = "") override;

  /** Hidden */
  void loadAsync(Scene* scene, const IGLTFLoaderData& data, const std::string& rootUrl,
                 const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress
                 = nullptr,
                 const std::string& fileName = "") override;

  /**
   * @brief Loads a glTF scene.
   * @param context The context when loading the asset
   * @param scene The glTF scene property
   * @returns A promise that resolves when the load is complete
   */
  bool loadSceneAsync(const std::string& context, const IScene& scene);

  /**
   * @brief Loads a glTF node.
   * @param context The context when loading the asset
   * @param node The glTF node property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon mesh when the load
   * is complete
   */
  TransformNodePtr
  loadNodeAsync(const std::string& context, INode& node,
                const std::function<void(const TransformNodePtr& babylonTransformNode)>& assign);

  /**
   * @brief Define this method to modify the default behavior when loading data
   * for mesh primitives.
   * @param context The context when loading the asset
   * @param name The mesh name when loading the asset
   * @param node The glTF node when loading the asset
   * @param mesh The glTF mesh when loading the asset
   * @param primitive The glTF mesh primitive property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded mesh when the load is
   * complete or null if not handled
   */
  AbstractMeshPtr
  _loadMeshPrimitiveAsync(const std::string& context, const std::string& name, INode& node,
                          const IMesh& mesh, IMeshPrimitive& primitive,
                          const std::function<void(const AbstractMeshPtr& babylonMesh)>& assign);

  /**
   * @brief Loads a glTF camera.
   * @param context The context when loading the asset
   * @param camera The glTF camera property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon camera when the
   * load is complete
   */
  CameraPtr loadCameraAsync(const std::string& context, const ICamera& camera,
                            const std::function<void(const CameraPtr& babylonCamera)>& assign);

  /**
   * @brief Loads a glTF animation.
   * @param context The context when loading the asset
   * @param animation The glTF animation property
   * @returns A promise that resolves with the loaded Babylon animation group
   * when the load is complete
   */
  AnimationGroupPtr loadAnimationAsync(const std::string& context, std::shared_ptr<IAnimation>& animation);

  /**
   * @brief Loads a glTF animation channel.
   * @param context The context when loading the asset
   * @param animationContext The context of the animation when loading the asset
   * @param animation The glTF animation property
   * @param channel The glTF animation channel property
   * @param babylonAnimationGroup The babylon animation group property
   * @param animationTargetOverride The babylon animation channel target
   * override property. My be null.
   * @returns A void promise when the channel load is complete
   */
  void _loadAnimationChannelAsync(const std::string& context, const std::string& animationContext,
                                  const std::shared_ptr<IAnimation>& animation, const IAnimationChannel& channel,
                                  const AnimationGroupPtr& babylonAnimationGroup,
                                  const IAnimatablePtr& animationTargetOverride = nullptr);

  /**
   * @brief Loads a glTF buffer view.
   * @param context The context when loading the asset
   * @param bufferView The glTF buffer view property
   * @returns A promise that resolves with the loaded data when the load is
   * complete
   */
  ArrayBufferView& loadBufferViewAsync(const std::string& context, IBufferView& bufferView);

  /**
   * @brief Hidden
   */
  MaterialPtr
  _loadMaterialAsync(const std::string& context, IMaterial& material, const MeshPtr& babylonMesh,
                     unsigned int babylonDrawMode,
                     const std::function<void(const MaterialPtr& babylonMaterial)>& assign);

  /**
   * @brief Creates a Babylon material from a glTF material.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonDrawMode The draw mode for the Babylon material
   * @returns The Babylon material
   */
  MaterialPtr createMaterial(const std::string& context, const IMaterial& material,
                             unsigned int babylonDrawMode);

  /**
   * @brief Loads properties from a glTF material into a Babylon material.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonMaterial The Babylon material
   * @returns A promise that resolves when the load is complete
   */
  bool loadMaterialPropertiesAsync(const std::string& context, const IMaterial& material,
                                   const MaterialPtr& babylonMaterial);

  /**
   * @brief Loads the normal, occlusion, and emissive properties from a glTF
   * material into a Babylon material.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonMaterial The Babylon material
   * @returns A promise that resolves when the load is complete
   */
  void loadMaterialBasePropertiesAsync(const std::string& context, const IMaterial& material,
                                       const MaterialPtr& babylonMaterial);

  /**
   * @brief Loads the alpha properties from a glTF material into a Babylon
   * material. Must be called after the setting the albedo texture of the
   * Babylon material when the material has an albedo texture.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonMaterial The Babylon material
   */
  void loadMaterialAlphaProperties(const std::string& context, const IMaterial& material,
                                   const MaterialPtr& babylonMaterial);

  /**
   * @brief Loads a glTF texture info.
   * @param context The context when loading the asset
   * @param textureInfo The glTF texture info property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon texture when the
   * load is complete
   */
  BaseTexturePtr
  loadTextureInfoAsync(const std::string& context, const IGLTF2::ITextureInfo& textureInfo,
                       const std::function<void(const BaseTexturePtr& babylonTexture)>& assign);

  /**
   * @brief Loads a glTF image.
   * @param context The context when loading the asset
   * @param image The glTF image property
   * @returns A promise that resolves with the loaded data when the load is
   * complete
   */
  ArrayBufferView& loadImageAsync(const std::string& context, IImage& image);
  /**
   * @brief Loads a glTF uri.
   * @param context The context when loading the asset
   * @param uri The base64 or relative uri
   * @returns A promise that resolves with the loaded data when the load is
   * complete
   */
  ArrayBufferView loadUriAsync(const std::string& context, const std::string& uri);

  /**
   * @brief Adds a JSON pointer to the metadata of the Babylon object at
   * `<object>.metadata.gltf.pointers`.
   * @param babylonObject the Babylon object with metadata
   * @param pointer the JSON pointer
   */
  void AddPointerMetadata(const BaseTexturePtr& babylonObject, const std::string& pointer);

  /**
   * @brief Adds a JSON pointer to the metadata of the Babylon object at
   * `<object>.metadata.gltf.pointers`.
   * @param babylonObject the Babylon object with metadata
   * @param pointer the JSON pointer
   */
  void AddPointerMetadata(const TransformNodePtr& babylonObject, const std::string& pointer);

  /**
   * @brief Adds a JSON pointer to the metadata of the Babylon object at
   * `<object>.metadata.gltf.pointers`.
   * @param babylonObject the Babylon object with metadata
   * @param pointer the JSON pointer
   */
  void AddPointerMetadata(const MaterialPtr& babylonObject, const std::string& pointer);

  /**
   * @brief Adds a JSON pointer to the metadata of the Babylon object at
   * `<object>.metadata.gltf.pointers`.
   * @param babylonObject the Babylon object with metadata
   * @param pointer the JSON pointer
   */
  void AddPointerMetadata(const CameraPtr& babylonObject, const std::string& pointer);

  /**
   * @brief Increments the indentation level and logs a message.
   * @param message The message to log
   */
  void logOpen(const std::string& message);

  /**
   * @brief Decrements the indentation level.
   */
  void logClose();

  /**
   * @brief Logs a message
   * @param message The message to log
   */
  void log(const std::string& message);

  /**
   * @brief Starts a performance counter.
   * @param counterName The name of the performance counter
   */
  void startPerformanceCounter(const std::string& counterName);

  /**
   * @brief Ends a performance counter.
   * @param counterName The name of the performance counter
   */
  void endPerformanceCounter(const std::string& counterName);

protected:
  /** Hidden */
  GLTFLoader(GLTFFileLoader& parent);

private:
  /** Hidden */
  void _loadAsync(const std::vector<size_t>& nodes, const std::function<void()>& resultFunc);
  void _loadData(const IGLTFLoaderData& data);
  void _setupData();
  void _loadExtensions();
  void _checkExtensions();
  void _setState(const GLTFLoaderState& state);
  INodePtr _createRootNode();
  void _forEachPrimitive(const INode& node,
                         const std::function<void(const AbstractMeshPtr& babylonMesh)>& callback);
  std::vector<AbstractMeshPtr> _getMeshes();
  std::vector<SkeletonPtr> _getSkeletons();
  std::vector<AnimationGroupPtr> _getAnimationGroups();
  void _startAnimations();
  TransformNodePtr
  _loadMeshAsync(const std::string& context, INode& node, IMesh& mesh,
                 const std::function<void(const TransformNodePtr& babylonTransformNode)>& assign);
  GeometryPtr _loadVertexDataAsync(const std::string& context, IMeshPrimitive& primitive,
                                   const MeshPtr& babylonMesh);
  void _createMorphTargets(const std::string& context, INode& node, const IMesh& mesh,
                           const IMeshPrimitive& primitive, const MeshPtr& babylonMesh);
  void _loadMorphTargetsAsync(const std::string& context, const IMeshPrimitive& primitive,
                              const MeshPtr& babylonMesh, const GeometryPtr& babylonGeometry);
  void _loadMorphTargetVertexDataAsync(const std::string& context,
                                       const GeometryPtr& babylonGeometry,
                                       const std::unordered_map<std::string, size_t>& attributes,
                                       const MorphTargetPtr& babylonMorphTarget);
  static void _LoadTransform(const INode& node, const TransformNodePtr& babylonNode);
  void _loadSkinAsync(const std::string& context, const INode& node, ISkin& skin);
  void _loadBones(const std::string& context, const ISkin& skin,
                  const SkeletonPtr& babylonSkeleton);
  BonePtr _loadBone(INode& node, const ISkin& skin, const SkeletonPtr& babylonSkeleton,
                    std::unordered_map<size_t, BonePtr>& babylonBones);
  Float32Array _loadSkinInverseBindMatricesDataAsync(const std::string& context, const ISkin& skin);
  void _updateBoneMatrices(const SkeletonPtr& babylonSkeleton,
                           const Float32Array& inverseBindMatricesData);
  Matrix _getNodeMatrix(const INode& node);
  void _loadAnimationsAsync();
  _IAnimationSamplerData _loadAnimationSamplerAsync(const std::string& context,
                                                    IAnimationSampler& sampler);
  ArrayBufferView& _loadBufferAsync(const std::string& context, IBuffer& buffer);
  template <typename T>
  ArrayBufferView& _loadAccessorAsync(const std::string& context, IAccessor& accessor);
  Float32Array& _loadFloatAccessorAsync(const std::string& context, IAccessor& accessor);
  IndicesArray& _getConverted32bitIndices(IAccessor& accessor);
  IndicesArray _castIndicesTo32bit(const IGLTF2::AccessorComponentType& type,
                                   const ArrayBufferView& buffer);
  IndicesArray& _loadIndicesAccessorAsync(const std::string& context, IAccessor& accessor);
  BufferPtr _loadVertexBufferViewAsync(IBufferView& bufferView, const std::string& kind);
  VertexBufferPtr& _loadVertexAccessorAsync(const std::string& context, IAccessor& accessor,
                                            const std::string& kind);
  void _loadMaterialMetallicRoughnessPropertiesAsync(
    const std::string& context,
    std::optional<IMaterialPbrMetallicRoughness> properties = std::nullopt,
    const MaterialPtr& babylonMaterial                      = nullptr);
  MaterialPtr _createDefaultMaterial(const std::string& name, unsigned int babylonDrawMode);
  BaseTexturePtr
  _loadTextureAsync(const std::string& context, const ITexture& texture,
                    const std::function<void(const BaseTexturePtr& babylonTexture)>& assign);
  _ISamplerData _loadSampler(const std::string& context, ISampler& sampler);
  void _onProgress();
  static unsigned int _GetTextureWrapMode(const std::string& context,
                                          std::optional<IGLTF2::TextureWrapMode> mode
                                          = std::nullopt);
  static unsigned int _GetTextureSamplingMode(const std::string& context, const ISampler& sampler);
  static ArrayBufferView _GetTypedArray(const std::string& context,
                                        IGLTF2::AccessorComponentType componentType,
                                        const ArrayBufferView& bufferView,
                                        std::optional<size_t> byteOffset = std::nullopt,
                                        size_t length                    = 0);
  static unsigned int _GetNumComponents(const std::string& context, IGLTF2::AccessorType type);
  static unsigned int _GetNumComponents(const std::string& context, const std::string& type);
  static bool _ValidateUri(const std::string& uri);
  static unsigned int _GetDrawMode(const std::string& context,
                                   std::optional<IGLTF2::MeshPrimitiveMode> mode = std::nullopt);
  void _compileMaterialsAsync();
  void _compileShadowGeneratorsAsync();
  void _forEachExtensions(const std::function<void(const IGLTFLoaderExtension& extension)>& action);
  void _extensionsOnLoading();
  void _extensionsOnReady();
  bool _extensionsLoadSceneAsync(const std::string& context, const IScene& scene);
  TransformNodePtr _extensionsLoadNodeAsync(
    const std::string& context, const INode& node,
    const std::function<void(const TransformNodePtr& babylonTransformNode)>& assign);
  CameraPtr
  _extensionsLoadCameraAsync(const std::string& context, const ICamera& camera,
                             const std::function<void(const CameraPtr& babylonCamera)>& assign);
  GeometryPtr _extensionsLoadVertexDataAsync(const std::string& context,
                                             const IMeshPrimitive& primitive,
                                             const MeshPtr& babylonMesh);
  MaterialPtr _extensionsLoadMaterialAsync(
    const std::string& context, const IMaterial& material, const MeshPtr& babylonMesh,
    unsigned int babylonDrawMode,
    const std::function<void(const MaterialPtr& babylonMaterial)>& assign);
  MaterialPtr _extensionsCreateMaterial(const std::string& context, const IMaterial& material,
                                        unsigned int babylonDrawMode);
  bool _extensionsLoadMaterialPropertiesAsync(const std::string& context, const IMaterial& material,
                                              const MaterialPtr& babylonMaterial);
  BaseTexturePtr _extensionsLoadTextureInfoAsync(
    const std::string& context, const IGLTF2::ITextureInfo& textureInfo,
    const std::function<void(const BaseTexturePtr& babylonTexture)>& assign);
  AnimationGroupPtr _extensionsLoadAnimationAsync(const std::string& context,
                                                  const std::shared_ptr<IAnimation>& animation);
  std::optional<ArrayBufferView> _extensionsLoadUriAsync(const std::string& context,
                                                         const std::string& uri);

private:
  bool _disposed;
  GLTFFileLoader& _parent;
  GLTFLoaderState _state;
  std::unordered_map<std::string, IGLTFLoaderExtensionPtr> _extensions;
  std::string _rootUrl;
  std::string _fileName;
  std::string _uniqueRootUrl;
  std::unique_ptr<IGLTF> _gltf;
  Scene* _babylonScene;
  MeshPtr _rootBabylonMesh;
  std::unordered_map<unsigned int, MaterialPtr> _defaultBabylonMaterialData;
  std::function<void(const SceneLoaderProgressEvent& event)> _progressCallback;

}; // end of class GLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
