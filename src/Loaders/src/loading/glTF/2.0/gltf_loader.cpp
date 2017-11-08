#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/core/logging.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {
namespace GLTF2 {

void GLTFLoader::RegisterExtension(GLTFLoaderExtension* extension)
{
}

GLTFLoader::GLTFLoader(GLTFFileLoader* parent)
{
}

GLTFLoader::~GLTFLoader()
{
}

void GLTFLoader::dispose()
{
}

void GLTFLoader::importMeshAsync(
  const vector_t<string_t>& meshesNames, Scene* scene,
  const IGLTFLoaderData& data, const vector_t<string_t>& rootUrl,
  const ::std::function<void(
    const vector_t<AbstractMesh*>& meshes,
    const vector_t<ParticleSystem*>& particleSystems,
    const vector_t<Skeleton*>& skeletons,
    const ::std::function<void(const ProgressEvent& event)>& onProgress,
    const ::std::function<void(const string_t& message)>& onError)>& onSuccess)
{
}

void GLTFLoader::loadAsync(
  Scene* scene, const IGLTFLoaderData& data, const string_t& rootUrl,
  const ::std::function<void()>& onSuccess,
  const ::std::function<void(const ProgressEvent& event)>& onProgress,
  const ::std::function<void(const string_t& message)>& onError)
{
}

void GLTFLoader::_loadAsync(
  const vector_t<string_t>& nodeNames, Scene* scene,
  const IGLTFLoaderData& data, const string_t& rootUrl,
  const ::std::function<void()>& onSuccess,
  const ::std::function<void(const ProgressEvent& event)>& onProgress,
  const ::std::function<void(const string_t& message)>& onError)
{
}

void GLTFLoader::_onProgress(const ProgressEvent& event)
{
}

void GLTFLoader::_executeWhenRenderReady(const ::std::function<void()>& func)
{
}

void GLTFLoader::_onRenderReady()
{
}

void GLTFLoader::_onComplete()
{
}

void GLTFLoader::_loadData(const IGLTFLoaderData& data)
{
}

vector_t<AbstractMesh*> GLTFLoader::_getMeshes()
{
}

vector_t<Skeleton*> GLTFLoader::_getSkeletons()
{
}

vector_t<Node*> GLTFLoader::_getAnimationTargets()
{
}

void GLTFLoader::_startAnimations()
{
}

void GLTFLoader::_loadDefaultScene(const vector_t<string_t>& nodeNames)
{
}

void GLTFLoader::_loadScene(const string_t& context, const IGLTFScene& scene,
                            const vector_t<string_t>& nodeNames)
{
}

void GLTFLoader::_loadNode(const string_t& context, const IGLTFNode& node)
{
}

void GLTFLoader::_loadMesh(const string_t& context, const IGLTFNode& node,
                           const IGLTFMesh& mesh)
{
}

void GLTFLoader::_loadAllVertexDataAsync(
  const string_t& context, const IGLTFMesh& mesh,
  const ::std::function<void()>& onSuccess)
{
}

Float32Array
GLTFLoader::_convertToFloat4TextureCoordArray(const string_t& context,
                                              const ArrayBufferView& data,
                                              const IGLTFAccessor& accessor)
{
}

Float32Array
GLTFLoader::_convertToFloat4ColorArray(const string_t& context,
                                       const ArrayBufferView& data,
                                       const IGLTFAccessor& accessor)
{
}

void GLTFLoader::_loadVertexDataAsync(
  const string_t& context, const IGLTFMesh& mesh,
  const IGLTFMeshPrimitive& primitive,
  const ::std::function<void(const VertexData& vertexData)>& onSuccess)
{
}

void GLTFLoader::_createMorphTargets(const string_t& context,
                                     const IGLTFNode& node,
                                     const IGLTFMesh& mesh)
{
}

void GLTFLoader::_loadMorphTargets(const string_t& context,
                                   const IGLTFNode& node, const IGLTFMesh& mesh)
{
}

void GLTFLoader::_loadAllMorphTargetVertexDataAsync(
  const string_t& context, const IGLTFNode& node, const IGLTFMesh& mesh,
  const ::std::function<void()>& onSuccess)
{
}

void GLTFLoader::_loadMorphTargetVertexDataAsync(
  const string_t& context, const VertexData& vertexData,
  const unordered_map_t<string_t, int>& attributes,
  const ::std::function<void(const VertexData& vertexData)>& onSuccess)
{
}

void GLTFLoader::_loadTransform(IGLTFNode* node)
{
}

Skeleton* GLTFLoader::_loadSkin(const string_t& context, const IGLTFSkin& skin)
{
}

Bone* GLTFLoader::_createBone(IGLTFNode* node, const IGLTFSkin& skin,
                              Bone* parent, const Matrix& localMatrix,
                              const Matrix& baseMatrix, unsigned int index)
{
}

void GLTFLoader::_loadBone(const string_t& context, const IGLTFSkin& skin,
                           const Float32Array& inverseBindMatrixData)
{
}

Bone* GLTFLoader::_loadBone(IGLTFNode* node, const IGLTFSkin& skin,
                            const Float32Array& inverseBindMatrixData,
                            const unordered_map_t<int, Bone*>& babylonBones)
{
}

Matrix GLTFLoader::_getNodeMatrix(const IGLTFNode& node)
{
}

void GLTFLoader::_traverseNodes(
  const string_t& context, const Uint32Array& indices,
  ::std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action,
  IGLTFNode* parentNode)
{
}

void GLTFLoader::_traverseNode(
  const string_t& context, IGLTFNode* node,
  ::std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action)
{
}

void GLTFLoader::_loadAnimations()
{
}

void GLTFLoader::_loadAnimation(const string_t& context,
                                const IGLTFAnimation& animation)
{
}

void GLTFLoader::_loadAnimationChannel(const IGLTFAnimation& animation,
                                       const string_t& channelContext,
                                       const IGLTFAnimationChannel& channel,
                                       const string_t& samplerContext,
                                       const IGLTFAnimationSampler& sampler)
{
}

void GLTFLoader::_loadBufferAsync(
  const string_t& context, const IGLTFBuffer& buffer,
  const ::std::function<void(ArrayBuffer& data)>& onSucces)
{
}

void GLTFLoader::_loadBufferViewAsync(
  const string_t& context, const IGLTFBufferView& bufferView,
  const ::std::function<void(ArrayBuffer& data)>& onSucces)
{
}

void GLTFLoader::_loadAccessorAsync(
  const string_t& context, const IGLTFAccessor& accessor,
  const ::std::function<void(ArrayBuffer& data)>& onSuccess)
{
}

void GLTFLoader::_whenAction(const ::std::function<void()>& action,
                             const ::std::function<void()>& onComplete)
{
}

Material* GLTFLoader::_getDefaultMaterial()
{
}

void GLTFLoader::_loadMaterialMetallicRoughnessProperties(
  const string_t& context, const IGLTFMaterial& material)
{
}

void GLTFLoader::_loadMaterial(
  const string_t& context, IGLTFMaterial& material,
  const ::std::function<void(Material* babylonMaterial, bool isNew)>& assign)
{
}

void GLTFLoader::_createPbrMaterial(IGLTFMaterial& material)
{
}

void GLTFLoader::_loadMaterialBaseProperties(const string_t& context,
                                             const IGLTFMaterial& material)
{
}

void GLTFLoader::_loadMaterialAlphaProperties(const string_t& context,
                                              const IGLTFMaterial& material,
                                              const Float32Array& colorFactor)
{
}

Texture* GLTFLoader::_loadTexture(const string_t& context,
                                  const IGLTFTexture& texture,
                                  int coordinatesIndex)
{
}

void GLTFLoader::_loadImage(
  const string_t& context, const IGLTFImage& image,
  const ::std::function<void(ArrayBuffer& data)>& onSuccess)
{
}

void GLTFLoader::_loadUri(
  const string_t& context, const string_t& uri,
  const ::std::function<void(ArrayBuffer& data)>& onSuccess)
{
}

void GLTFLoader::_tryCatchOnError(const ::std::function<void()>& handler)
{
}

unsigned int
GLTFLoader::_GetTextureWrapMode(const Nullable<ETextureWrapMode>& iMode)
{
  const auto wrapModeToUint
    = [](const ETextureWrapMode val) { return static_cast<unsigned int>(val); };

  // Set defaults if undefined
  const unsigned int mode = iMode.isNull() ?
                              wrapModeToUint(ETextureWrapMode::REPEAT) :
                              wrapModeToUint(*iMode);

  if (mode == wrapModeToUint(ETextureWrapMode::CLAMP_TO_EDGE)) {
    return TextureConstants::CLAMP_ADDRESSMODE;
  }
  else if (mode == wrapModeToUint(ETextureWrapMode::MIRRORED_REPEAT)) {
    return TextureConstants::MIRROR_ADDRESSMODE;
  }
  else if (mode == wrapModeToUint(ETextureWrapMode::REPEAT)) {
    return TextureConstants::WRAP_ADDRESSMODE;
  }
  else {
    BABYLON_LOGF_WARN("_GetTextureWrapMode", "Invalid texture wrap mode (%ul)",
                      mode);
    return TextureConstants::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFLoader::_GetTextureSamplingMode(
  const Nullable<ETextureMagFilter>& iMagFilter,
  const Nullable<ETextureMinFilter>& iMinFilter)
{
  const auto magFilterToUint = [](const ETextureMagFilter val) {
    return static_cast<unsigned int>(val);
  };

  const auto minFilterToUint = [](const ETextureMinFilter val) {
    return static_cast<unsigned int>(val);
  };

  // Set defaults if undefined
  const unsigned int magFilter = iMagFilter.isNull() ?
                                   magFilterToUint(ETextureMagFilter::LINEAR) :
                                   magFilterToUint(*iMagFilter);
  const unsigned int minFilter
    = iMinFilter.isNull() ?
        minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR) :
        minFilterToUint(*iMinFilter);

  if (magFilter == magFilterToUint(ETextureMagFilter::LINEAR)) {
    if (minFilter == minFilterToUint(ETextureMinFilter::NEAREST)) {
      return TextureConstants::LINEAR_NEAREST;
    }
    else if (minFilter == minFilterToUint(ETextureMinFilter::LINEAR)) {
      return TextureConstants::LINEAR_LINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_NEAREST_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_LINEAR_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_NEAREST_MIPLINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
    else {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture minification filter (%ul)", minFilter);
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
  else {
    if (magFilter != magFilterToUint(ETextureMagFilter::NEAREST)) {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture magnification filter (%ul)",
                        magFilter);
    }

    if (minFilter == minFilterToUint(ETextureMinFilter::NEAREST)) {
      return TextureConstants::LINEAR_NEAREST;
    }
    else if (minFilter == minFilterToUint(ETextureMinFilter::LINEAR)) {
      return TextureConstants::LINEAR_LINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_NEAREST_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_NEAREST)) {
      return TextureConstants::LINEAR_LINEAR_MIPNEAREST;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::NEAREST_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_NEAREST_MIPLINEAR;
    }
    else if (minFilter
             == minFilterToUint(ETextureMinFilter::LINEAR_MIPMAP_LINEAR)) {
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
    else {
      BABYLON_LOGF_WARN("_GetTextureSamplingMode",
                        "Invalid texture minification filter (%ul)", minFilter);
      return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
}

unsigned int GLTFLoader::_GetNumComponents(const string_t& type)
{
  if (type == "SCALAR") {
    return 1;
  }
  else if (type == "VEC2") {
    return 2;
  }
  else if (type == "VEC3") {
    return 3;
  }
  else if (type == "VEC4") {
    return 4;
  }
  else if (type == "MAT2") {
    return 4;
  }
  else if (type == "MAT3") {
    return 9;
  }
  else if (type == "MAT4") {
    return 16;
  }

  return 0;
}

} // end of namespace GLTF2
} // end of namespace BABYLON
