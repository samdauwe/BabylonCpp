#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/core/logging.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader_utils.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/tools.h>

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
  if (!_defaultMaterial) {
    const string_t id = "__gltf_default";
    auto material
      = static_cast<PBRMaterial*>(_babylonScene->getMaterialByName(id));
    if (!material) {
      material                  = PBRMaterial::New(id, _babylonScene);
      material->sideOrientation = Material::CounterClockWiseSideOrientation;
      material->metallic        = 1.f;
      material->roughness       = 1.f;
    }

    _defaultMaterial = material;
  }

  return _defaultMaterial;
}

void GLTFLoader::_loadMaterialMetallicRoughnessProperties(
  const string_t& context, const IGLTFMaterial& material)
{
  auto babylonMaterial = static_cast<PBRMaterial*>(material.babylonMaterial);
  if (!babylonMaterial) {
    return;
  }

  // Ensure metallic workflow
  babylonMaterial->metallic  = 1;
  babylonMaterial->roughness = 1;

  if (!material.pbrMetallicRoughness) {
    return;
  }
  const auto& properties = *material.pbrMetallicRoughness;

  babylonMaterial->albedoColor
    = !properties.baseColorFactor.empty() ?
        Color3::FromArray(properties.baseColorFactor) :
        Color3(1.f, 1.f, 1.f);
  babylonMaterial->metallic
    = properties.metallicFactor.isNull() ? 1.f : *properties.metallicFactor;
  babylonMaterial->roughness
    = properties.roughnessFactor.isNull() ? 1.f : *properties.roughnessFactor;

  if (properties.baseColorTexture) {
    auto& baseColorTexture = *properties.baseColorTexture;
    if (baseColorTexture.index >= _gltf->textures.size()) {
      throw ::std::runtime_error(context
                                 + ": Failed to find base color texture "
                                 + ::std::to_string(baseColorTexture.index));
    }

    auto& texture = _gltf->textures[baseColorTexture.index];
    babylonMaterial->albedoTexture
      = _loadTexture("#/textures/" + ::std::to_string(texture.index), texture,
                     baseColorTexture.texCoord);
  }

  if (properties.metallicRoughnessTexture) {
    auto& metallicRoughnessTexture = *properties.metallicRoughnessTexture;
    if (metallicRoughnessTexture.index >= _gltf->textures.size()) {
      throw ::std::runtime_error(
        context + ": Failed to find metallic roughness texture "
        + ::std::to_string(metallicRoughnessTexture.index));
    }

    auto& texture = _gltf->textures[metallicRoughnessTexture.index];
    babylonMaterial->metallicTexture
      = _loadTexture("#/textures/" + ::std::to_string(texture.index), texture,
                     metallicRoughnessTexture.texCoord);
    babylonMaterial->useMetallnessFromMetallicTextureBlue = true;
    babylonMaterial->useRoughnessFromMetallicTextureGreen = true;
    babylonMaterial->useRoughnessFromMetallicTextureAlpha = false;
  }

  _loadMaterialAlphaProperties(context, material, properties.baseColorFactor);
}

void GLTFLoader::_loadMaterial(
  const string_t& context, IGLTFMaterial& material,
  const ::std::function<void(Material* babylonMaterial, bool isNew)>& assign)
{
  if (material.babylonMaterial) {
    assign(material.babylonMaterial, false);
    return;
  }

  if (GLTFLoaderExtension::LoadMaterial(this, context, material, assign)) {
    return;
  }

  _createPbrMaterial(material);
  _loadMaterialBaseProperties(context, material);
  _loadMaterialMetallicRoughnessProperties(context, material);
  assign(material.babylonMaterial, true);
}

void GLTFLoader::_createPbrMaterial(IGLTFMaterial& material)
{
  auto babylonMaterial = PBRMaterial::New(
    !material.name.empty() ? material.name :
                             "mat" + ::std::to_string(material.index),
    _babylonScene);
  babylonMaterial->sideOrientation = Material::CounterClockWiseSideOrientation;
  material.babylonMaterial         = babylonMaterial;
}

void GLTFLoader::_loadMaterialBaseProperties(const string_t& context,
                                             const IGLTFMaterial& material)
{
  auto babylonMaterial = static_cast<PBRMaterial*>(material.babylonMaterial);
  if (!babylonMaterial) {
    return;
  }

  babylonMaterial->emissiveColor
    = !material.emissiveFactor.empty() ?
        Color3::FromArray(material.emissiveFactor) :
        Color3(0.f, 0.f, 0.f);
  if (material.doubleSided) {
    babylonMaterial->setBackFaceCulling(false);
    babylonMaterial->twoSidedLighting = true;
  }

  if (material.normalTexture) {
    auto& normalTexture = *material.normalTexture;
    if (normalTexture.index >= _gltf->textures.size()) {
      throw ::std::runtime_error(context + ": Failed to find normal texture "
                                 + ::std::to_string(normalTexture.index));
    }

    auto& texture = _gltf->textures[normalTexture.index];
    babylonMaterial->bumpTexture
      = _loadTexture("#/textures/" + ::std::to_string(texture.index), texture,
                     normalTexture.texCoord);
    babylonMaterial->invertNormalMapX = !_babylonScene->useRightHandedSystem();
    babylonMaterial->invertNormalMapY = _babylonScene->useRightHandedSystem();
    if (!normalTexture.scale.isNull()) {
      babylonMaterial->bumpTexture->level = *normalTexture.scale;
    }
  }

  if (material.occlusionTexture) {
    auto& occlusionTexture = *material.occlusionTexture;
    if (occlusionTexture.index >= _gltf->textures.size()) {
      throw ::std::runtime_error(context + ": Failed to find occlusion texture "
                                 + ::std::to_string(occlusionTexture.index));
    }

    auto& texture = _gltf->textures[occlusionTexture.index];
    babylonMaterial->ambientTexture
      = _loadTexture("#/textures/" + ::std::to_string(texture.index), texture,
                     occlusionTexture.texCoord);
    babylonMaterial->useAmbientInGrayScale = true;
    if (!occlusionTexture.strength.isNull()) {
      babylonMaterial->ambientTextureStrength = *occlusionTexture.strength;
    }
  }

  if (material.emissiveTexture) {
    auto& emissiveTexture = *material.emissiveTexture;
    if (emissiveTexture.index >= _gltf->textures.size()) {
      throw ::std::runtime_error(context + ": Failed to find emissive texture "
                                 + ::std::to_string(emissiveTexture.index));
    }

    auto& texture = _gltf->textures[emissiveTexture.index];
    babylonMaterial->emissiveTexture
      = _loadTexture("#/textures/" + ::std::to_string(texture.index), texture,
                     emissiveTexture.texCoord);
  }
}

void GLTFLoader::_loadMaterialAlphaProperties(const string_t& context,
                                              const IGLTFMaterial& material,
                                              const Float32Array& colorFactor)
{
  auto babylonMaterial = static_cast<PBRMaterial*>(material.babylonMaterial);
  if (!babylonMaterial) {
    return;
  }

  const auto alphaMode
    = !material.alphaMode.empty() ? material.alphaMode : "OPAQUE";
  if (alphaMode == "OPAQUE") {
    // default is opaque
  }
  else if (alphaMode == "MASK") {
    babylonMaterial->alphaCutOff
      = (material.alphaCutoff.isNull() ? 0.5f : *material.alphaCutoff);

    if (colorFactor.size() >= 4) {
      if (colorFactor[3] == 0.f) {
        babylonMaterial->alphaCutOff = 1.f;
      }
      else {
        babylonMaterial->alphaCutOff /= colorFactor[3];
      }
    }

    if (babylonMaterial->albedoTexture) {
      babylonMaterial->albedoTexture->setHasAlpha(true);
    }
  }
  else if (alphaMode == "BLEND") {
    if (colorFactor.size() >= 4) {
      babylonMaterial->alpha = colorFactor[3];
    }

    if (babylonMaterial->albedoTexture) {
      babylonMaterial->albedoTexture->setHasAlpha(true);
      babylonMaterial->useAlphaFromAlbedoTexture = true;
    }
  }
  else {
    throw ::std::runtime_error(context + ": Invalid alpha mode "
                               + material.alphaMode);
  }
}

Texture* GLTFLoader::_loadTexture(const string_t& context,
                                  const IGLTFTexture& texture,
                                  unsigned int coordinatesIndex)
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
  if (!GLTFUtils::ValidateUri(uri)) {
    throw ::std::runtime_error(context + ": Uri '" + uri + "' is invalid");
  }
}

void GLTFLoader::_tryCatchOnError(const ::std::function<void()>& handler)
{
  if (_disposed) {
    return;
  }

  try {
    handler();
  }
  catch (const ::std::exception& e) {
    BABYLON_LOGF_ERROR("GLTFLoader", "glTF Loader:: %s", e.what());

    if (_errorCallback) {
      _errorCallback(string_t(e.what()));
    }

    dispose();
  }
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
