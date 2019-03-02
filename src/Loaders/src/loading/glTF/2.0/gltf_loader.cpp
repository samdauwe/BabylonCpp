#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/animations/animation_group.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GLTF2 {

GLTFLoader::GLTFLoader(const GLTFFileLoaderPtr& parent) : _parent{parent}
{
}

GLTFLoader::~GLTFLoader()
{
}

void GLTFLoader::dispose()
{
}

void GLTFLoader::importMeshAsync()
{
}

void GLTFLoader::loadAsync()
{
}

void GLTFLoader::_loadAsync()
{
}

void GLTFLoader::_loadData()
{
}

void GLTFLoader::_setupData()
{
  ArrayItem::Assign(gltf->accessors);
  ArrayItem::Assign(gltf->animations);
  ArrayItem::Assign(gltf->buffers);
  ArrayItem::Assign(gltf->bufferViews);
  ArrayItem::Assign(gltf->cameras);
  ArrayItem::Assign(gltf->images);
  ArrayItem::Assign(gltf->materials);
  ArrayItem::Assign(gltf->meshes);
  ArrayItem::Assign(gltf->nodes);
  ArrayItem::Assign(gltf->samplers);
  ArrayItem::Assign(gltf->scenes);
  ArrayItem::Assign(gltf->skins);
  ArrayItem::Assign(gltf->textures);

  if (!gltf->nodes.empty()) {
    std::unordered_map<size_t, size_t> nodeParents;
    for (const auto& node : gltf->nodes) {
      if (!node.children.empty()) {
        for (const auto& index : node.children) {
          nodeParents[index] = node.index;
        }
      }
    }

    auto rootNode = _createRootNode();
    for (auto& node : gltf->nodes) {
      if (stl_util::contains(nodeParents, node.index)) {
        const auto parentIndex = nodeParents[node.index];
        node.parent            = gltf->nodes[parentIndex];
      }
      else {
        node.parent = rootNode;
      }
    }
  }
}

void GLTFLoader::_loadExtensions()
{
}

void GLTFLoader::_checkExtensions()
{
}

void GLTFLoader::_setState()
{
}

INode GLTFLoader::_createRootNode()
{
}

void GLTFLoader::loadSceneAsync(const std::string& context, const IScene& scene)
{
}

void GLTFLoader::_forEachPrimitive(
  const INode& node,
  const std::function<void(const AbstractMeshPtr& babylonMesh)>& callback)
{
}

std::vector<AbstractMeshPtr> GLTFLoader::_getMeshes()
{
}

std::vector<SkeletonPtr> GLTFLoader::_getSkeletons()
{
}
std::vector<AnimationGroupPtr> GLTFLoader::_getAnimationGroups()
{
}

void GLTFLoader::_startAnimations()
{
}

TransformNodePtr GLTFLoader::loadNodeAsync(
  const std::string& context, const INode& node,
  const std::function<void(const TransformNodePtr& babylonTransformNode)>&
    assign)
{
}

TransformNodePtr GLTFLoader::_loadMeshAsync(
  const std::string& context, const INode& node, const IMesh& mesh,
  const std::function<void(const TransformNodePtr& babylonTransformNode)>&
    callback)
{
}

AbstractMeshPtr GLTFLoader::_loadMeshPrimitiveAsync(
  const std::string& context, const std::string& name, const INode& node,
  const IMesh& mesh, const IMeshPrimitive& primitive,
  const std::function<void(const AbstractMeshPtr& babylonMesh)>& assign)
{
}

GeometryPtr GLTFLoader::_loadVertexDataAsync(const std::string& context,
                                             IMeshPrimitive& primitive,
                                             const MeshPtr& babylonMesh)
{
}

void GLTFLoader::_createMorphTargets(const std::string& context,
                                     const INode& node, const IMesh& mesh,
                                     const IMeshPrimitive& primitive,
                                     const MeshPtr& babylonMesh)
{
}

void GLTFLoader::_loadMorphTargetsAsync(const std::string& context,
                                        const IMeshPrimitive& primitive,
                                        const MeshPtr& babylonMesh,
                                        const GeometryPtr& babylonGeometry)
{
}

void GLTFLoader::_loadMorphTargetVertexDataAsync(
  const std::string& context, const GeometryPtr& babylonGeometry,
  const std::unordered_map<std::string, size_t>& attributes,
  const MorphTargetPtr& babylonMorphTarget)
{
}

void GLTFLoader::_LoadTransform(const INode& node,
                                const TransformNodePtr& babylonNode)
{
  // Ignore the TRS of skinned nodes.
  // See
  // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skins
  // (second implementation note)
  if (node.skin.has_value()) {
    return;
  }

  auto position = Vector3::Zero();
  auto rotation = Quaternion::Identity();
  auto scaling  = Vector3::One();

  if (!node.matrix.empty()) {
    const auto matrix = Matrix::FromArray(node.matrix);
    matrix.decompose(scaling, rotation, position);
  }
  else {
    if (!node.translation.empty()) {
      position = Vector3::FromArray(node.translation);
    }
    if (!node.rotation.empty()) {
      rotation = Quaternion::FromArray(node.rotation);
    }
    if (!node.scale.empty()) {
      scaling = Vector3::FromArray(node.scale);
    }
  }

  babylonNode->position           = position;
  babylonNode->rotationQuaternion = rotation;
  babylonNode->scaling            = scaling;
}

void GLTFLoader::_loadSkinAsync(const std::string& context, const INode& node,
                                const ISkin& skin)
{
}

void GLTFLoader::_loadBones(const std::string& context, const ISkin& skin,
                            const SkeletonPtr& babylonSkeleton)
{
}

BonePtr GLTFLoader::_loadBone(INode& node, const ISkin& skin,
                              const SkeletonPtr& babylonSkeleton,
                              std::unordered_map<size_t, BonePtr>& babylonBones)
{
  if (stl_util::contains(babylonBones, node.index)
      && babylonBones[node.index]) {
    return babylonBones[node.index];
  }

  BonePtr babylonParentBone = nullptr;
  if (node.parent && node.parent._babylonTransformNode != _rootBabylonMesh) {
    babylonParentBone
      = _loadBone(node.parent, skin, babylonSkeleton, babylonBones);
  }

  const auto boneIndex = stl_util::index_of(skin.joints, node.index);

  auto nodeName
    = !node.name.empty() ? node.name : String::printf("joint%ld", node.index);
  auto babylonBone
    = Bone::New(nodeName, babylonSkeleton.get(), babylonParentBone.get(),
                _getNodeMatrix(node), std::nullopt, std::nullopt, boneIndex);
  babylonBones[node.index] = babylonBone;

  node._babylonBones.emplace_back(babylonBone);

  return babylonBone;
}

Float32Array
GLTFLoader::_loadSkinInverseBindMatricesDataAsync(const std::string& context,
                                                  const ISkin& skin)
{
}

void GLTFLoader::_updateBoneMatrices(
  const SkeletonPtr& babylonSkeleton,
  const Float32Array& inverseBindMatricesData)
{
  for (const auto& babylonBone : babylonSkeleton->bones) {
    auto baseMatrix      = Matrix::Identity();
    const auto boneIndex = babylonBone->_index;
    if (!inverseBindMatricesData.empty() && boneIndex.has_value()
        && *boneIndex != -1) {
      Matrix::FromArrayToRef(inverseBindMatricesData,
                             static_cast<unsigned int>(*boneIndex * 16),
                             baseMatrix);
      baseMatrix.invertToRef(baseMatrix);
    }

    const auto& babylonParentBone = babylonBone->getParent();
    if (babylonParentBone) {
      baseMatrix.multiplyToRef(
        babylonParentBone->getInvertedAbsoluteTransform(), baseMatrix);
    }

    babylonBone->updateMatrix(baseMatrix, false, false);
    babylonBone->_updateDifferenceMatrix(std::nullopt, false);
  }
}

Matrix GLTFLoader::_getNodeMatrix(const INode& node)
{
  return !node.matrix.empty() ?
           Matrix::FromArray(node.matrix) :
           Matrix::Compose(
             !node.scale.empty() ? Vector3::FromArray(node.scale) :
                                   Vector3::One(),
             !node.rotation.empty() ? Quaternion::FromArray(node.rotation) :
                                      Quaternion::Identity(),
             !node.translation.empty() ? Vector3::FromArray(node.translation) :
                                         Vector3::Zero());
}

CameraPtr GLTFLoader::loadCameraAsync(
  const std::string& context, const ICamera& camera,
  const std::function<void(const CameraPtr& babylonCamera)>& assign)
{
}

void GLTFLoader::_loadAnimationsAsync()
{
  auto& animations = gltf->animations;
  if (animations.empty()) {
    return;
  }

  std::vector<std::function<AnimationGroupPtr()>> promises;

  for (auto& animation : animations) {
    promises.emplace_back([this, &animation]() -> AnimationGroupPtr {
      return loadAnimationAsync(
        String::printf("/animations/%ld", animation.index), animation);
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  return;
}

AnimationGroupPtr GLTFLoader::loadAnimationAsync(const std::string& context,
                                                 IAnimation& animation)
{
  const auto animationGroup = _extensionsLoadAnimationAsync(context, animation);
  if (animationGroup) {
    return animationGroup;
  }

  const auto babylonAnimationGroup = AnimationGroup::New(
    !animation.name.empty() ? animation.name :
                              String::printf("animation%ld", animation.index),
    babylonScene);
  animation._babylonAnimationGroup = babylonAnimationGroup;

  std::vector<std::function<void()>> promises;

  ArrayItem::Assign(animation.channels);
  ArrayItem::Assign(animation.samplers);

  for (const auto& channel : animation.channels) {
    promises.emplace_back([&]() -> void {
      _loadAnimationChannelAsync(
        String::printf("%s/channels/%ld", context.c_str(), channel.index),
        context, animation, channel, babylonAnimationGroup);
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  babylonAnimationGroup->normalize(0);
  return babylonAnimationGroup;
}

void GLTFLoader::_loadAnimationChannelAsync(
  const std::string& context, const std::string& animationContext,
  const IAnimation& animation, const IAnimationChannel& channel,
  const AnimationGroupPtr& babylonAnimationGroup)
{
}

_IAnimationSamplerData
GLTFLoader::_loadAnimationSamplerAsync(const std::string& context,
                                       const IAnimationSampler& sampler)
{
}

ArrayBufferView GLTFLoader::_loadBufferAsync(const std::string& context,
                                             const IBuffer& buffer)
{
  if (buffer._data.has_value()) {
    return *buffer._data;
  }

  if (buffer.uri.empty()) {
    throw std::runtime_error(
      String::printf("%s/uri: Value is missing", context.c_str()));
  }

  buffer._data
    = loadUriAsync(String::printf("%s/uri", context.c_str()), buffer.uri);

  return buffer._data;
}

ArrayBufferView GLTFLoader::loadBufferViewAsync(const std::string& context,
                                                const IBufferView& bufferView)
{
}

IndicesArray GLTFLoader::_loadIndicesAccessorAsync(const std::string& context,
                                                   const IAccessor& accessor)
{
}

Float32Array GLTFLoader::_loadFloatAccessorAsync(const std::string& context,
                                                 const IAccessor& accessor)
{
}

Buffer GLTFLoader::_loadVertexBufferViewAsync(const IBufferView& bufferView,
                                              const std::string& kind)
{
}

VertexBuffer GLTFLoader::_loadVertexAccessorAsync(const std::string& context,
                                                  const IAccessor& accessor,
                                                  const std::string& kind)
{
}

void GLTFLoader::_loadMaterialMetallicRoughnessPropertiesAsync(
  const std::string& context, const IMaterialPbrMetallicRoughness& properties,
  const MaterialPtr& babylonMaterial)
{
}

MaterialPtr GLTFLoader::_loadMaterialAsync(
  const std::string& context, const IMaterial& material,
  const MeshPtr& babylonMesh, unsigned int babylonDrawMode,
  const std::function<void(const MaterialPtr& babylonMaterial)>& assign)
{
}

MaterialPtr GLTFLoader::_createDefaultMaterial(const std::string& name,
                                               unsigned int babylonDrawMode)
{
  auto babylonMaterial = PBRMaterial::New(name, babylonScene);
  babylonMaterial->sideOrientation
    = babylonScene->useRightHandedSystem ?
        Material::CounterClockWiseSideOrientation() :
        Material::ClockWiseSideOrientation();
  babylonMaterial->fillMode                   = babylonDrawMode;
  babylonMaterial->enableSpecularAntiAliasing = true;
  babylonMaterial->useRadianceOverAlpha = !_parent->transparencyAsCoverage;
  babylonMaterial->useSpecularOverAlpha = !_parent->transparencyAsCoverage;
  babylonMaterial->transparencyMode     = PBRMaterial::PBRMATERIAL_OPAQUE;
  babylonMaterial->metallic             = 1;
  babylonMaterial->roughness            = 1;
  return std::move(babylonMaterial);
}

MaterialPtr GLTFLoader::createMaterial(const std::string& context,
                                       const IMaterial& material,
                                       unsigned int babylonDrawMode)
{
}

void GLTFLoader::loadMaterialPropertiesAsync(const std::string& context,
                                             const IMaterial& material,
                                             const MaterialPtr& babylonMaterial)
{
}

void GLTFLoader::loadMaterialBasePropertiesAsync(
  const std::string& context, const IMaterial& material,
  const MaterialPtr& babylonMaterial)
{
}

void GLTFLoader::loadMaterialAlphaProperties(const std::string& context,
                                             const IMaterial& material,
                                             const MaterialPtr& babylonMaterial)
{
  auto babylonPBRMaterial
    = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      String::printf("%s: Material type not supported", context.c_str()));
  }

  const auto alphaMode = material.alphaMode.has_value() ?
                           *material.alphaMode :
                           IGLTF2::MaterialAlphaMode::OPAQUE;
  switch (alphaMode) {
    case IGLTF2::MaterialAlphaMode::OPAQUE: {
      babylonPBRMaterial->transparencyMode = PBRMaterial::PBRMATERIAL_OPAQUE;
      break;
    }
    case IGLTF2::MaterialAlphaMode::MASK: {
      babylonPBRMaterial->transparencyMode = PBRMaterial::PBRMATERIAL_ALPHATEST;
      babylonPBRMaterial->alphaCutOff
        = (!material.alphaCutoff.has_value() ? 0.5f : *material.alphaCutoff);
      if (babylonPBRMaterial->albedoTexture()) {
        babylonPBRMaterial->albedoTexture()->hasAlpha = true;
      }
      break;
    }
    case IGLTF2::MaterialAlphaMode::BLEND: {
      babylonPBRMaterial->transparencyMode
        = PBRMaterial::PBRMATERIAL_ALPHABLEND;
      if (babylonPBRMaterial->albedoTexture()) {
        babylonPBRMaterial->albedoTexture()->hasAlpha = true;
        babylonPBRMaterial->useAlphaFromAlbedoTexture = true;
      }
      break;
    }
    default: {
      throw std::runtime_error(
        String::printf("%s/alphaMode: Invalid value (%d)", context.c_str(),
                       static_cast<int>(*material.alphaMode)));
    }
  }
}

BaseTexturePtr GLTFLoader::loadTextureInfoAsync(
  const std::string& context, const ITextureInfo& textureInfo,
  const std::function<void(const BaseTexturePtr& babylonTexture)>& assign)
{
}

BaseTexturePtr GLTFLoader::_loadTextureAsync(
  const std::string& context, const ITexture& texture,
  const std::function<void(const BaseTexturePtr& babylonTexture)>& assign)
{
}

_ISamplerData GLTFLoader::_loadSampler(const std::string& context,
                                       ISampler& sampler)
{
  if (!sampler._data.has_value()) {
    sampler._data = {
      (sampler.minFilter == IGLTF2::TextureMinFilter::NEAREST
       || sampler.minFilter == IGLTF2::TextureMinFilter::LINEAR), // noMipMaps
      GLTFLoader::_GetTextureSamplingMode(context, sampler), // samplingMode
      GLTFLoader::_GetTextureWrapMode(
        String::printf("%s/wrapS", context.c_str()), sampler.wrapS), // wrapU
      GLTFLoader::_GetTextureWrapMode(
        String::printf("%s/wrapT", context.c_str()), sampler.wrapT) // wrapV
    };
  }

  return *sampler._data;
}

ArrayBufferView GLTFLoader::loadImageAsync(const std::string& context,
                                           const IImage& image)
{
}

ArrayBufferView GLTFLoader::loadUriAsync(const std::string& context,
                                         const std::string& uri)
{
}

void GLTFLoader::_onProgress()
{
}

void GLTFLoader::AddPointerMetadata(const json& babylonObject,
                                    const std::string& pointer)
{
}

unsigned int
GLTFLoader::_GetTextureWrapMode(const std::string& context,
                                std::optional<IGLTF2::TextureWrapMode> mode)
{
  // Set defaults if undefined
  mode = (!mode.has_value()) ? IGLTF2::TextureWrapMode::REPEAT : mode;

  switch (*mode) {
    case IGLTF2::TextureWrapMode::CLAMP_TO_EDGE:
      return TextureConstants::CLAMP_ADDRESSMODE;
    case IGLTF2::TextureWrapMode::MIRRORED_REPEAT:
      return TextureConstants::MIRROR_ADDRESSMODE;
    case IGLTF2::TextureWrapMode::REPEAT:
      return TextureConstants::WRAP_ADDRESSMODE;
    default:
      BABYLON_LOGF_WARN("GLTFLoader", "%s: Invalid value (%d)", context.c_str(),
                        static_cast<int>(*mode));
      return TextureConstants::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFLoader::_GetTextureSamplingMode(const std::string& context,
                                                 const ISampler& sampler)
{
  // Set defaults if undefined
  const auto magFilter = (!sampler.magFilter.has_value()) ?
                           IGLTF2::TextureMagFilter::LINEAR :
                           *sampler.magFilter;
  const auto minFilter = (!sampler.minFilter.has_value()) ?
                           IGLTF2::TextureMinFilter::LINEAR_MIPMAP_LINEAR :
                           *sampler.minFilter;

  if (magFilter == IGLTF2::TextureMagFilter::LINEAR) {
    switch (minFilter) {
      case IGLTF2::TextureMinFilter::NEAREST:
        return TextureConstants::LINEAR_NEAREST;
      case IGLTF2::TextureMinFilter::LINEAR:
        return TextureConstants::LINEAR_LINEAR;
      case IGLTF2::TextureMinFilter::NEAREST_MIPMAP_NEAREST:
        return TextureConstants::LINEAR_NEAREST_MIPNEAREST;
      case IGLTF2::TextureMinFilter::LINEAR_MIPMAP_NEAREST:
        return TextureConstants::LINEAR_LINEAR_MIPNEAREST;
      case IGLTF2::TextureMinFilter::NEAREST_MIPMAP_LINEAR:
        return TextureConstants::LINEAR_NEAREST_MIPLINEAR;
      case IGLTF2::TextureMinFilter::LINEAR_MIPMAP_LINEAR:
        return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
      default:
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value (%d)",
                          context.c_str(), static_cast<int>(minFilter));
        return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
  else {
    if (magFilter != IGLTF2::TextureMagFilter::NEAREST) {
      BABYLON_LOGF_WARN("GLTFLoader", "%s/magFilter: Invalid value (%d)",
                        context.c_str(), static_cast<int>(magFilter));
    }

    switch (minFilter) {
      case IGLTF2::TextureMinFilter::NEAREST:
        return TextureConstants::NEAREST_NEAREST;
      case IGLTF2::TextureMinFilter::LINEAR:
        return TextureConstants::NEAREST_LINEAR;
      case IGLTF2::TextureMinFilter::NEAREST_MIPMAP_NEAREST:
        return TextureConstants::NEAREST_NEAREST_MIPNEAREST;
      case IGLTF2::TextureMinFilter::LINEAR_MIPMAP_NEAREST:
        return TextureConstants::NEAREST_LINEAR_MIPNEAREST;
      case IGLTF2::TextureMinFilter::NEAREST_MIPMAP_LINEAR:
        return TextureConstants::NEAREST_NEAREST_MIPLINEAR;
      case IGLTF2::TextureMinFilter::LINEAR_MIPMAP_LINEAR:
        return TextureConstants::NEAREST_LINEAR_MIPLINEAR;
      default:
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value (%d)",
                          context.c_str(), static_cast<int>(minFilter));
        return TextureConstants::NEAREST_NEAREST_MIPNEAREST;
    }
  }
}

ArrayBufferView
GLTFLoader::_GetTypedArray(const std::string& context,
                           IGLTF2::AccessorComponentType componentType,
                           const ArrayBufferView& bufferView,
                           std::optional<size_t> byteOffset, size_t length)
{
  const auto& buffer = bufferView.int8Array;
  byteOffset
    = bufferView.byteOffset + (byteOffset.has_value() ? *byteOffset : 0);

  try {
    switch (componentType) {
      case IGLTF2::AccessorComponentType::BYTE:
        return stl_util::to_array<int8_t>(buffer, *byteOffset, length);
      case IGLTF2::AccessorComponentType::UNSIGNED_BYTE:
        return stl_util::to_array<uint8_t>(buffer, *byteOffset, length);
      case IGLTF2::AccessorComponentType::SHORT:
        return stl_util::to_array<uint16_t>(buffer, *byteOffset, length);
      case IGLTF2::AccessorComponentType::UNSIGNED_SHORT:
        return stl_util::to_array<uint16_t>(buffer, *byteOffset, length);
      case IGLTF2::AccessorComponentType::UNSIGNED_INT:
        return stl_util::to_array<uint32_t>(buffer, *byteOffset, length);
      case IGLTF2::AccessorComponentType::FLOAT:
        return stl_util::to_array<float_t>(buffer, *byteOffset, length);
      default:
        throw std::runtime_error(String::printf(
          "Invalid component type %d", static_cast<int>(componentType)));
    }
  }
  catch (const std::exception& e) {
    throw std::runtime_error(
      String::printf("%s: %s", context.c_str(), e.what()));
  }
}

unsigned int GLTFLoader::_GetNumComponents(const std::string& context,
                                           const std::string& type)
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

  throw std::runtime_error(
    String::printf("%s: Invalid type (%d)", context.c_str(), type));
}

bool GLTFLoader::_ValidateUri(const std::string& uri)
{
  return (Tools::IsBase64(uri) || String::indexOf(uri, "..") == -1);
}

unsigned int
GLTFLoader::_GetDrawMode(const std::string& context,
                         std::optional<IGLTF2::MeshPrimitiveMode> mode)
{
  if (!mode.has_value()) {
    mode = IGLTF2::MeshPrimitiveMode::TRIANGLES;
  }

  switch (*mode) {
    case IGLTF2::MeshPrimitiveMode::POINTS:
      return Material::PointListDrawMode();
    case IGLTF2::MeshPrimitiveMode::LINES:
      return Material::LineListDrawMode();
    case IGLTF2::MeshPrimitiveMode::LINE_LOOP:
      return Material::LineLoopDrawMode();
    case IGLTF2::MeshPrimitiveMode::LINE_STRIP:
      return Material::LineStripDrawMode();
    case IGLTF2::MeshPrimitiveMode::TRIANGLES:
      return Material::TriangleFillMode();
    case IGLTF2::MeshPrimitiveMode::TRIANGLE_STRIP:
      return Material::TriangleStripDrawMode();
    case IGLTF2::MeshPrimitiveMode::TRIANGLE_FAN:
      return Material::TriangleFanDrawMode();
  }

  throw std::runtime_error(
    String::printf("%s: Invalid mesh primitive mode (%d)", context.c_str(),
                   static_cast<int>(*mode)));
}

void GLTFLoader::_compileMaterialsAsync()
{
}

void GLTFLoader::_compileShadowGeneratorsAsync()
{
}

void GLTFLoader::_forEachExtensions(
  const std::function<void(const IGLTFLoaderExtension& extension)>& action)
{
  for (const auto& name : GLTFLoader::_ExtensionNames) {
    if (stl_util::contains(_extensions, name)) {
      const auto& extension = _extensions[name];
      if (extension.enabled) {
        action(extension);
      }
    }
  }
}

void GLTFLoader::_extensionsOnLoading()
{
}

void GLTFLoader::_extensionsOnReady()
{
}

void GLTFLoader::_extensionsLoadSceneAsync(const std::string& context,
                                           const IScene& scene)
{
}

TransformNodePtr GLTFLoader::_extensionsLoadNodeAsync(
  const std::string& context, INode& node,
  const std::function<void(const TransformNode& babylonTransformNode)>& assign)
{
}

CameraPtr GLTFLoader::_extensionsLoadCameraAsync(
  const std::string& context, const ICamera& camera,
  const std::function<void(const Camera& babylonCamera)>& assign)
{
}

GeometryPtr _extensionsLoadVertexDataAsync(const std::string& context,
                                           const IMeshPrimitive& primitive,
                                           const Mesh& babylonMesh)
{
}

MaterialPtr GLTFLoader::_extensionsLoadMaterialAsync(
  const std::string& context, const IMaterial& material,
  const Mesh& babylonMesh, unsigned int babylonDrawMode,
  const std::function<void(const MaterialPtr& babylonMaterial)>& assign)
{
}

MaterialPtr GLTFLoader::_extensionsCreateMaterial(const std::string& context,
                                                  const IMaterial& material,
                                                  unsigned int babylonDrawMode)
{
}

void GLTFLoader::_extensionsLoadMaterialPropertiesAsync(
  const std::string& context, const IMaterial& material,
  const MaterialPtr& babylonMaterial)
{
}

BaseTexturePtr& GLTFLoader::_extensionsLoadTextureInfoAsync(
  const std::string& context, const ITextureInfo& textureInfo,
  const std::function<void(const BaseTexture& babylonTexture)>& assign)
{
}

AnimationGroupPtr
GLTFLoader::_extensionsLoadAnimationAsync(const std::string& context,
                                          const IAnimation& animation)
{
}

ArrayBufferView GLTFLoader::_extensionsLoadUriAsync(const std::string& context,
                                                    const std::string& uri)
{
}

void GLTFLoader::logOpen(const std::string& message)
{
  _parent->_logOpen(message);
}

void GLTFLoader::logClose()
{
  _parent->_logClose();
}

void GLTFLoader::log(const std::string& message)
{
  _parent->_log(message);
}

void GLTFLoader::startPerformanceCounter(const std::string& counterName)
{
  _parent->_startPerformanceCounter(counterName);
}

void GLTFLoader::endPerformanceCounter(const std::string& counterName)
{
  _parent->_endPerformanceCounter(counterName);
}

} // end of namespace GLTF2
} // end of namespace BABYLON
