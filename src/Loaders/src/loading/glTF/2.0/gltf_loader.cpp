#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader_utils.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/morph/morph_target_manager.h>
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
  const auto colorComponentCount = GLTFLoader::_GetNumComponents(accessor.type);
  if (colorComponentCount == 4
      && accessor.componentType == EComponentType::FLOAT) {
    return data.float32Array;
  }

  auto factor = 1.f;
  switch (accessor.componentType) {
    case EComponentType::FLOAT: {
      factor = 1.f;
      break;
    }
    case EComponentType::UNSIGNED_BYTE: {
      factor = 1.f / 255.f;
      break;
    }
    case EComponentType::UNSIGNED_SHORT: {
      factor = 1.f / 65535.f;
      break;
    }
    default: {
      throw ::std::runtime_error(
        context + ": Invalid component type ("
        + ::std::to_string(static_cast<unsigned>(accessor.componentType))
        + ")");
    }
  }

  Float32Array result(accessor.count * 4);
  if (colorComponentCount == 4) {
    for (unsigned int i = 0; i < result.size(); ++i) {
      result[i] = buffer[i] * factor;
    }
  }
  else {
    unsigned int offset = 0;
    for (unsigned int i = 0; i < result.size(); ++i) {
      if ((i + 1) % 4 == 0) {
        result[i] = 1;
      }
      else {
        result[i] = buffer[offset++] * factor;
      }
    }
  }

  return result;
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

void GLTFLoader::_loadTransform(IGLTFNode& node)
{
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

  node.babylonMesh->setPosition(position);
  node.babylonMesh->setRotationQuaternion(rotation);
  node.babylonMesh->setScaling(scaling);
}

Skeleton* GLTFLoader::_loadSkin(const string_t& context, IGLTFSkin& skin)
{
  const auto skeletonId   = "skeleton" + ::std::to_string(skin.index);
  const auto skeletonName = !skin.name.empty() ? skin.name : skeletonId;
  skin.babylonSkeleton = new Skeleton(skeletonName, skeletonId, _babylonScene);

  if (skin.inverseBindMatrices.isNull()) {
    _loadBones(context, skin, Float32Array());
  }
  else {
    const auto index = *skin.inverseBindMatrices;
    if (index >= _gltf->accessors.size()) {
      throw ::std::runtime_error(
        context + ": Failed to find inverse bind matrices attribute "
        + ::std::to_string(index));
    }
    auto& accessor = _gltf->accessors[index];

    _loadAccessorAsync("#/accessors/" + ::std::to_string(accessor.index),
                       accessor, [&](ArrayBufferView& data) {
                         _loadBones(context, skin, data.float32Array);
                       });
  }

  return skin.babylonSkeleton;
}

Bone* GLTFLoader::_createBone(IGLTFNode& node, const IGLTFSkin& skin,
                              Bone* parent, const Matrix& localMatrix,
                              const Matrix& baseMatrix, unsigned int index)
{
  const auto boneName
    = !node.name.empty() ? node.name : "bone" + ::std::to_string(node.index);
  const auto babylonBone = Bone::New(boneName, skin.babylonSkeleton, parent,
                                     localMatrix, nullptr, baseMatrix, index);
  node.babylonBones[skin.index] = babylonBone;
  node.babylonAnimationTargets.emplace_back(babylonBone);
  return babylonBone;
}

void GLTFLoader::_loadBones(const string_t& context, const IGLTFSkin& skin,
                            const Float32Array& inverseBindMatrixData)
{
  unordered_map_t<int, Bone*> babylonBones;
  for (const auto& index : skin.joints) {
    if (index >= _gltf->nodes.size()) {
      throw ::std::runtime_error(context + ": Failed to find joint "
                                 + ::std::to_string(index));
    }
    auto& node = _gltf->nodes[index];

    _loadBone(node, skin, inverseBindMatrixData, babylonBones);
  }
}

Bone* GLTFLoader::_loadBone(const IGLTFNode& node, const IGLTFSkin& skin,
                            const Float32Array& inverseBindMatrixData,
                            unordered_map_t<int, Bone*>& babylonBones)
{
  if (stl_util::contains(babylonBones, node.index)
      && babylonBones[node.index]) {
    return babylonBones[node.index];
  }

  const auto boneIndex = stl_util::index_of(skin.joints, node.index);

  auto baseMatrix = Matrix::Identity();
  if (!inverseBindMatrixData.empty() && boneIndex != -1) {
    auto _boneIndex = static_cast<unsigned>(boneIndex);
    baseMatrix      = Matrix::FromArray(inverseBindMatrixData, _boneIndex * 16);
    baseMatrix.invertToRef(baseMatrix);
  }

  Bone* babylonParentBone = nullptr;
  if (node.index != skin.skeleton && node.parent && node.parent != _rootNode) {
    babylonParentBone
      = _loadBone(node.parent, skin, inverseBindMatrixData, babylonBones);
    baseMatrix.multiplyToRef(babylonParentBone->getInvertedAbsoluteTransform(),
                             baseMatrix);
  }

  auto babylonBone = _createBone(node, skin, babylonParentBone,
                                 _getNodeMatrix(node), baseMatrix, boneIndex);
  babylonBones[node.index] = babylonBone;
  return babylonBone;
}

Matrix GLTFLoader::_getNodeMatrix(const IGLTFNode& node)
{
  if (!node.matrix.empty()) {
    return Matrix::FromArray(node.matrix);
  }
  else {
    auto rotation = !node.rotation.empty() ?
                      Quaternion::FromArray(node.rotation) :
                      Quaternion::Identity();
    return Matrix::Compose(
      !node.scale.empty() ? Vector3::FromArray(node.scale) : Vector3::One(),
      rotation,
      !node.translation.empty() ? Vector3::FromArray(node.translation) :
                                  Vector3::Zero());
  }
}

void GLTFLoader::_traverseNodes(
  const string_t& context, const Uint32Array& indices,
  ::std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action,
  IGLTFNode* parentNode)
{
  for (const auto& index : indices) {
    if (index >= _gltf->nodes.size()) {
      throw ::std::runtime_error(context + ": Failed to find node "
                                 + ::std::to_string(index));
    }
    auto& node = _gltf->nodes[index];

    _traverseNode(context, node, action, parentNode);
  }
}

void GLTFLoader::_traverseNode(
  const string_t& context, IGLTFNode* node,
  ::std::function<bool(IGLTFNode* node, IGLTFNode* parentNode)>& action,
  IGLTFNode* parentNode)
{
  if (GLTFLoaderExtension::TraverseNode(this, context, node, action,
                                        parentNode)) {
    return;
  }

  if (!action(node, parentNode)) {
    return;
  }

  if (!node->children.empty()) {
    _traverseNodes(context, node->children, action, node);
  }
}

void GLTFLoader::_loadAnimations()
{
  auto& animations = _gltf->animations;
  if (animations.empty()) {
    return;
  }

  for (unsigned int index = 0; index < animations.size(); ++index) {
    auto& animation = animations[index];
    _loadAnimation("#/animations/" + ::std::to_string(index), animation);
  }
}

void GLTFLoader::_loadAnimation(const string_t& context,
                                IGLTFAnimation& animation)
{
  animation.targets.clear();

  for (unsigned int index = 0; index < animation.channels.size(); ++index) {
    if (index >= animation.channels.size()) {
      throw ::std::runtime_error(context + ": Failed to find channel "
                                 + ::std::to_string(index));
    }
    auto& channel = animation.channels[index];

    if (channel.sampler >= animation.samplers.size()) {
      throw ::std::runtime_error(context + ": Failed to find sampler "
                                 + ::std::to_string(channel.sampler));
    }
    auto& sampler = animation.samplers[channel.sampler];

    _loadAnimationChannel(
      animation, context + "/channels/" + ::std::to_string(index), channel,
      context + "/samplers/" + ::std::to_string(channel.sampler), sampler);
  }
}

void GLTFLoader::_loadAnimationChannel(IGLTFAnimation& animation,
                                       const string_t& channelContext,
                                       const IGLTFAnimationChannel& channel,
                                       const string_t& samplerContext,
                                       const IGLTFAnimationSampler& sampler)
{
  if (channel.target.node >= _gltf->nodes.size()) {
    throw ::std::runtime_error(channelContext + ": Failed to find target node "
                               + ::std::to_string(channel.target.node));
  }
  auto& targetNode = _gltf->nodes[channel.target.node];

  string_t targetPath;
  unsigned int animationType{0};
  if (channel.target.path == "translation") {
    targetPath    = "position";
    animationType = Animation::ANIMATIONTYPE_VECTOR3;
  }
  else if (channel.target.path == "rotation") {
    targetPath    = "rotationQuaternion";
    animationType = Animation::ANIMATIONTYPE_QUATERNION;
  }
  else if (channel.target.path == "scale") {
    targetPath    = "scaling";
    animationType = Animation::ANIMATIONTYPE_VECTOR3;
  }
  else if (channel.target.path == "weights") {
    targetPath    = "influence";
    animationType = Animation::ANIMATIONTYPE_FLOAT;
  }
  else {
    throw ::std::runtime_error(channelContext + ": Invalid target path "
                               + channel.target.path);
  }

  Float32Array inputData;
  Float32Array outputData;

  const auto checkSuccess = [&]() {
    if (inputData.empty() || outputData.empty()) {
      return;
    }

    unsigned int outputBufferOffset = 0;

    ::std::function<AnimationValue()> getNextOutputValue;
    if (targetPath == "position") {
      getNextOutputValue = [&]() {
        const auto value = Vector3::FromArray(outputData, outputBufferOffset);
        outputBufferOffset += 3;
        return value;
      };
    }
    else if (targetPath == "rotationQuaternion") {
      getNextOutputValue = [&]() {
        const auto value
          = Quaternion::FromArray(outputData, outputBufferOffset);
        outputBufferOffset += 4;
        return value;
      };
    }
    else if (targetPath == "scaling") {
      getNextOutputValue = [&]() {
        const auto value = Vector3::FromArray(outputData, outputBufferOffset);
        outputBufferOffset += 3;
        return value;
      };
    }
    else if (targetPath == "influence") {
      getNextOutputValue = [&]() {
        const auto numTargets
          = targetNode.babylonMesh->morphTargetManager()->numTargets();
        Float32Array value(numTargets);
        for (size_t i = 0; i < numTargets; i++) {
          value[i] = outputData[outputBufferOffset++];
        }
        return value;
      };
    }

    ::std::function<AnimationKey(unsigned int frameIndex)> getNextKey;
    if (sampler.interpolation == "LINEAR") {
      getNextKey = [&](unsigned int frameIndex) {
        return AnimationKey{static_cast<int>(inputData[frameIndex]),
                            getNextOutputValue()};
      };
    }
    else if (sampler.interpolation == "CUBICSPLINE") {
      getNextKey = [&](unsigned int frameIndex) {
        AnimationKey key{static_cast<int>(inputData[frameIndex]),
                         getNextOutputValue()};
        key.value      = getNextOutputValue();
        key.outTangent = getNextOutputValue();
        return key;
      };
    }
    else {
      throw ::std::runtime_error(samplerContext + ": Invalid interpolation "
                                 + sampler.interpolation);
    }

    vector_t<AnimationKey> keys(inputData.size());
    for (unsigned int frameIndex = 0; frameIndex < inputData.size();
         ++frameIndex) {
      keys[frameIndex] = getNextKey(frameIndex);
    }

    if (targetPath == "influence") {
      auto morphTargetManager = targetNode.babylonMesh->morphTargetManager();

      for (size_t targetIndex = 0;
           targetIndex < morphTargetManager->numTargets(); ++targetIndex) {
        auto morphTarget   = morphTargetManager->getTarget(targetIndex);
        auto animationName = (!animation.name.empty() ?
                                animation.name :
                                "anim" + ::std::to_string(animation.index))
                             + "_" + ::std::to_string(targetIndex);
        auto babylonAnimation = new Animation(animationName, targetPath, 1,
                                              static_cast<int>(animationType));
        vector_t<AnimationKey> animationKeys;
        for (auto& key : keys) {
          AnimationKey animationKey(static_cast<int>(key.frame), key.value);
          animationKeys.emplace_back(animationKey);
        }

        morphTarget->animations().emplace_back(babylonAnimation);
        animation.targets.emplace_back(morphTarget);
      }
    }
    else {
      auto animationName = (!animation.name.empty() ?
                              animation.name :
                              "anim" + ::std::to_string(animation.index));
      auto babylonAnimation = new Animation(animationName, targetPath, 1,
                                            static_cast<int>(animationType));
      babylonAnimation->setKeys(keys);

      for (auto& target : targetNode.babylonAnimationTargets) {
        target->animations.emplace_back(babylonAnimation->clone());
        animation.targets.emplace_back(target);
      }
    }
  };

  if (sampler.input >= _gltf->accessors.size()) {
    throw ::std::runtime_error(samplerContext
                               + ": Failed to find input accessor "
                               + ::std::to_string(sampler.input));
  }
  auto& inputAccessor = _gltf->accessors[sampler.input];

  _loadAccessorAsync("#/accessors/" + ::std::to_string(inputAccessor.index),
                     inputAccessor, [&](const ArrayBufferView& data) {
                       inputData = data.float32Array;
                       checkSuccess();
                     });

  if (sampler.output >= _gltf->accessors.size()) {
    throw ::std::runtime_error(samplerContext
                               + ": Failed to find output accessor "
                               + ::std::to_string(sampler.input));
  }
  auto& outputAccessor = _gltf->accessors[sampler.output];

  _loadAccessorAsync("#/accessors/" + ::std::to_string(outputAccessor.index),
                     outputAccessor, [&](const ArrayBufferView& data) {
                       outputData = data.float32Array;
                       checkSuccess();
                     });
}

void GLTFLoader::_loadBufferAsync(
  const string_t& context, IGLTFBuffer& buffer,
  const ::std::function<void(IGLTFBufferView& data)>& onSuccess)
{
  _addPendingData(buffer);

  if (buffer.loadedData) {
    onSuccess(buffer.loadedData);
    _removePendingData(buffer);
  }
  else if (buffer.loadedObservable) {
    buffer.loadedObservable->add([&](IGLTFBuffer* buffer, EventState& /*es*/) {
      onSuccess(buffer->loadedData);
      _removePendingData(buffer);
    });
  }
  else {
    if (buffer.uri.empty()) {
      throw ::std::runtime_error(context + ": Uri is missing");
    }

    if (GLTFUtils::IsBase64(buffer.uri)) {
      const auto data   = GLTFUtils::DecodeBase64(buffer.uri);
      buffer.loadedData = ArrayBufferView(data);
      onSuccess(buffer.loadedData);
      _removePendingData(buffer);
    }
    else {
      buffer.loadedObservable = ::std::make_shared<Observable<IGLTFBuffer>>();
      buffer.loadedObservable->add(
        [&](IGLTFBuffer* buffer, EventState& /*es*/) {
          onSuccess(buffer->loadedData);
          _removePendingData(buffer);
        });

      _loadUri(context, buffer.uri, [&](const ArrayBufferView& data) {
        buffer.loadedData = data;
        buffer.loadedObservable->notifyObservers(&buffer);
        buffer.loadedObservable = nullptr;
      });
    }
  }
}

void GLTFLoader::_loadBufferViewAsync(
  const string_t& context, const IGLTFBufferView& bufferView,
  const ::std::function<void(ArrayBufferView& data)>& onSuccess)
{
  if (bufferView.buffer >= _gltf->buffers.size()) {
    throw ::std::runtime_error(context + ": Failed to find buffer "
                               + ::std::to_string(bufferView.buffer));
  }
  auto& buffer = _gltf->buffers[bufferView.buffer];

  _loadBufferAsync("#/buffers/" + ::std::to_string(buffer.index), buffer,
                   [&](IGLTFBufferView& bufferData) {
                     ArrayBufferView data;

                     try {
                       // data = Uint8Array(bufferData.buffer,
                       //                  bufferData.byteOffset +
                       //                  (bufferView.byteOffset || 0),
                       //                  bufferView.byteLength);
                     }
                     catch (const ::std::exception& e) {
                       throw ::std::runtime_error(context + ": " + e.what());
                     }

                     onSuccess(data);
                   });
}

void GLTFLoader::_loadAccessorAsync(
  const string_t& context, const IGLTFAccessor& accessor,
  const ::std::function<void(ArrayBufferView& data)>& onSuccess)
{
  if (accessor.sparse) {
    throw ::std::runtime_error(
      context + ": Sparse accessors are not currently supported");
  }

  if (accessor.bufferView >= _gltf->bufferViews.size()) {
    throw ::std::runtime_error(context + ": Failed to find buffer view "
                               + ::std::to_string(accessor.bufferView));
  }
  auto& bufferView = _gltf->bufferViews[accessor.bufferView];

  _loadBufferViewAsync(
    "#/bufferViews/" + ::std::to_string(bufferView.index), bufferView,
    [&](ArrayBufferView& bufferViewData) {
      const auto numComponents = GLTFLoader::_GetNumComponents(accessor.type);
      if (numComponents == 0) {
        throw ::std::runtime_error(context + ": Invalid type " + accessor.type);
      }

      ArrayBufferView data;
      auto byteOffset = accessor.byteOffset ? *accessor.byteOffset : 0;
      auto byteStride = bufferView.byteStride;

      try {
        switch (accessor.componentType) {
          case EComponentType::BYTE: {
            data = _buildArrayBuffer<Float32Array>(bufferViewData, byteOffset,
                                                   accessor.count,
                                                   numComponents, byteStride);
            break;
          }
          case EComponentType::UNSIGNED_BYTE: {
            data = _buildArrayBuffer<Uint8Array>(bufferViewData, byteOffset,
                                                 accessor.count, numComponents,
                                                 byteStride);
            break;
          }
          case EComponentType::SHORT: {
            data = _buildArrayBuffer<Int16Array>(bufferViewData, byteOffset,
                                                 accessor.count, numComponents,
                                                 byteStride);
            break;
          }
          case EComponentType::UNSIGNED_SHORT: {
            data = _buildArrayBuffer<Uint16Array>(bufferViewData, byteOffset,
                                                  accessor.count, numComponents,
                                                  byteStride);
            break;
          }
          case EComponentType::UNSIGNED_INT: {
            data = _buildArrayBuffer<Uint32Array>(bufferViewData, byteOffset,
                                                  accessor.count, numComponents,
                                                  byteStride);
            break;
          }
          case EComponentType::FLOAT: {
            data = _buildArrayBuffer<Float32Array>(bufferViewData, byteOffset,
                                                   accessor.count,
                                                   numComponents, byteStride);
            break;
          }
          default: {
            auto componentType = static_cast<unsigned>(accessor.componentType);
            throw ::std::runtime_error(context + ": Invalid component type "
                                       + ::std::to_string(componentType));
          }
        }
      }
      catch (const ::std::exception& e) {
        throw ::std::runtime_error(context + ": " + e.what());
      }

      onSuccess(data);
    });
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
