#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>

namespace BABYLON {
namespace GLTF2 {

struct GLTFFileLoader;
struct GLTFLoaderExtension;
struct GLTFLoaderTracker;
struct IGLTFLoaderData;
struct ProgressEvent;

/**
 * @brief Babylon.js glTF File Loader. This loader supports only glTF 2.0 and
 * will fail to load glTF 1.0.
 */
class BABYLON_SHARED_EXPORT GLTFLoader {

public:
  static void RegisterExtension(GLTFLoaderExtension* extension);

public:
  GLTFLoader(GLTFFileLoader* parent);
  ~GLTFLoader();

  void dispose();
  void importMeshAsync(
    const vector_t<string_t>& meshesNames, Scene* scene,
    const IGLTFLoaderData& data, const string_t& rootUrl,
    const std::function<void(const vector_t<AbstractMesh*>& meshes,
                               const vector_t<ParticleSystem*>& particleSystems,
                               const vector_t<Skeleton*>& skeletons)>&
      onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    const std::function<void(const string_t& message)>& onError);
  void
  loadAsync(Scene* scene, const IGLTFLoaderData& data, const string_t& rootUrl,
            const std::function<void()>& onSuccess,
            const std::function<void(const ProgressEvent& event)>& onProgress,
            const std::function<void(const string_t& message)>& onError);
  void _executeWhenRenderReady(
    const std::function<void(GLTFLoader* loader, EventState& es)>& func);
  void _loadNode(const string_t& context, const IGLTFNode& node);
  void _traverseNode(
    const string_t& context, IGLTFNode* node,
    std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action,
    IGLTFNode* parentNode = nullptr);
  void _whenAction(const std::function<void()>& action,
                   const std::function<void()>& onComplete);
  void _loadMaterial(
    const string_t& context, IGLTFMaterial& material,
    const std::function<void(Material* babylonMaterial, bool isNew)>& assign);
  void _createPbrMaterial(IGLTFMaterial& material);
  void _loadMaterialBaseProperties(const string_t& context,
                                   const IGLTFMaterial& material);
  void _loadMaterialAlphaProperties(const string_t& context,
                                    const IGLTFMaterial& material,
                                    const Float32Array& colorFactor);
  Texture* _loadTexture(const string_t& context, const IGLTFTexture& texture,
                        unsigned int coordinatesIndex = 0);
  void _loadUri(const string_t& context, const string_t& uri,
                const std::function<void(ArrayBuffer& data)>& onSuccess);
  void _tryCatchOnError(const std::function<void()>& handler);

private:
  void _loadAsync(
    const vector_t<string_t>& nodeNames, Scene* scene,
    const IGLTFLoaderData& data, const string_t& rootUrl,
    const std::function<void()>& onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    const std::function<void(const string_t& message)>& onError);
  void _onProgress(const ProgressEvent& event);
  void _onRenderReady();
  void _onComplete();
  void _loadData(const IGLTFLoaderData& data);
  vector_t<AbstractMesh*> _getMeshes();
  vector_t<Skeleton*> _getSkeletons();
  vector_t<Node*> _getAnimationTargets();
  void _startAnimations();
  void _loadDefaultScene(const vector_t<string_t>& nodeNames);
  void _loadScene(const string_t& context, const IGLTFScene& scene,
                  const vector_t<string_t>& nodeNames);
  void _loadMesh(const string_t& context, const IGLTFNode& node,
                 IGLTFMesh& mesh);
  void _loadAllVertexDataAsync(const string_t& context, IGLTFMesh& mesh,
                               const std::function<void()>& onSuccess);

  /**
   * @brief Converts a data bufferview into a Float4 Texture Coordinate Array,
   * based on the accessor component type
   * @param {ArrayBufferView} data
   * @param {IGLTFAccessor} accessor
   */
  Float32Array _convertToFloat4TextureCoordArray(const string_t& context,
                                                 const ArrayBufferView& data,
                                                 const IGLTFAccessor& accessor);

  /**
   * @ brief Converts a data bufferview into a Float4 Color Array, based on the
   * accessor component type
   * @param {ArrayBufferView} data
   * @param {IGLTFAccessor} accessor
   */
  Float32Array _convertToFloat4ColorArray(const string_t& context,
                                          const ArrayBufferView& data,
                                          const IGLTFAccessor& accessor);

  void _loadVertexDataAsync(
    const string_t& context, const IGLTFMesh& mesh,
    const IGLTFMeshPrimitive& primitive,
    const std::function<void(const VertexData& vertexData)>& onSuccess);
  void _createMorphTargets(const string_t& context, const IGLTFNode& node,
                           const IGLTFMesh& mesh);
  void _loadMorphTargets(const string_t& context, const IGLTFNode& node,
                         const IGLTFMesh& mesh);
  void _loadAllMorphTargetVertexDataAsync(
    const string_t& context, const IGLTFNode& node, const IGLTFMesh& mesh,
    const std::function<void()>& onSuccess);
  void _loadMorphTargetVertexDataAsync(
    const string_t& context, const VertexData& vertexData,
    const unordered_map_t<string_t, int>& attributes,
    const std::function<void(const VertexData& vertexData)>& onSuccess);
  void _loadTransform(IGLTFNode& node);
  Skeleton* _loadSkin(const string_t& context, IGLTFSkin& skin);
  Bone* _createBone(IGLTFNode& node, const IGLTFSkin& skin, Bone* parent,
                    const Matrix& localMatrix, const Matrix& baseMatrix,
                    unsigned int index);
  void _loadBones(const string_t& context, const IGLTFSkin& skin,
                  const Float32Array& inverseBindMatrixData);
  Bone* _loadBone(const IGLTFNode& node, const IGLTFSkin& skin,
                  const Float32Array& inverseBindMatrixData,
                  unordered_map_t<int, Bone*>& babylonBones);
  Matrix _getNodeMatrix(const IGLTFNode& node);
  void _traverseNodes(
    const string_t& context, const Uint32Array& indices,
    const std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action,
    IGLTFNode* parentNode = nullptr);
  void _loadAnimations();
  void _loadAnimation(const string_t& context, IGLTFAnimation& animation);
  void _loadAnimationChannel(IGLTFAnimation& animation,
                             const string_t& channelContext,
                             const IGLTFAnimationChannel& channel,
                             const string_t& samplerContext,
                             const IGLTFAnimationSampler& sampler);
  void _loadBufferAsync(
    const string_t& context, IGLTFBuffer& buffer,
    const std::function<void(IGLTFBufferView& data)>& onSuccess);
  void _loadBufferViewAsync(
    const string_t& context, const IGLTFBufferView& bufferView,
    const std::function<void(ArrayBufferView& data)>& onSuccess);
  void _loadAccessorAsync(
    const string_t& context, const IGLTFAccessor& accessor,
    const std::function<void(ArrayBufferView& data)>& onSuccess);
  template <typename T>
  T _buildArrayBuffer(ArrayBufferView& data, unsigned int byteOffset,
                      unsigned int count, unsigned int numComponents,
                      unsigned int byteStride);
  Material* _getDefaultMaterial();
  void _loadMaterialMetallicRoughnessProperties(const string_t& context,
                                                const IGLTFMaterial& material);
  void _loadImage(const string_t& context, const IGLTFImage& image,
                  const std::function<void(ArrayBuffer& data)>& onSuccess);
  static unsigned int
  _GetTextureWrapMode(const Nullable<ETextureWrapMode>& mode);
  static unsigned int
  _GetTextureSamplingMode(const Nullable<ETextureMagFilter>& magFilter,
                          const Nullable<ETextureMinFilter>& minFilter);
  static unsigned int _GetNumComponents(const string_t& type);

public:
  IGLTF* _gltf;
  Scene* _babylonScene;

public:
  static unordered_map_t<string_t, GLTFLoaderExtension> Extensions;

private:
  bool _disposed;
  GLTFFileLoader* _parent;
  string_t _rootUrl;
  PBRMaterial* _defaultMaterial;
  IGLTFNode* _rootNode;
  std::function<void()> _successCallback;
  std::function<void(const ProgressEvent& event)> _progressCallback;
  std::function<void(const string_t& message)> _errorCallback;
  bool _renderReady;
  Observable<GLTFLoader> _renderReadyObservable;
  // Count of pending work that needs to complete before the asset is rendered.
  size_t _renderPendingCount;
  // Count of pending work that needs to complete before the loader is disposed.
  size_t _loaderPendingCount;
  vector_t<GLTFLoaderTracker> _loaderTrackers;

}; // end of class GLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
