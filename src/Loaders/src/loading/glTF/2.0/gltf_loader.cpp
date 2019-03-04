#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/animations/animation_group.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/core/time.h>
#include <babylon/engine/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader_extension.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>
#include <babylon/morph/morph_target.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GLTF2 {

void GLTFLoader::RegisterExtension(
  const std::string& name,
  const std::function<IGLTFLoaderExtension(GLTFLoader& loader)>& factory)
{
  if (GLTFLoader::UnregisterExtension(name)) {
    BABYLON_LOGF_WARN("GLTFLoader",
                      "Extension with the name '%s' already exists",
                      name.c_str());
  }

  GLTFLoader::_ExtensionFactories[name] = factory;

  // Keep the order of registration so that extensions registered first are
  // called first.
  GLTFLoader::_ExtensionNames.emplace_back(name);
}

bool GLTFLoader::UnregisterExtension(const std::string& name)
{
  if (!stl_util::contains(GLTFLoader::_ExtensionFactories, name)) {
    return false;
  }

  stl_util::erase(GLTFLoader::_ExtensionFactories, name);
  stl_util::erase(GLTFLoader::_ExtensionNames, name);

  return true;
}

GLTFLoader::GLTFLoader(const GLTFFileLoaderPtr& parent)
    : gltf{nullptr}
    , babylonScene{nullptr}
    , _disposed{false}
    , _parent{parent}
    , _rootBabylonMesh{nullptr}
    , _progressCallback{nullptr}
{
}

GLTFLoader::~GLTFLoader()
{
}

GLTFLoaderState& GLTFLoader::state()
{
  return _state;
}

void GLTFLoader::dispose()
{
}

ImportMeshResult GLTFLoader::importMeshAsync(
  const std::vector<std::string>& meshesNames, Scene* scene,
  const IGLTFLoaderData& data, const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  babylonScene      = scene;
  _rootUrl          = rootUrl;
  _fileName         = !fileName.empty() ? fileName : "scene";
  _progressCallback = onProgress;
  _loadData(data);

  std::vector<size_t> nodes;

  if (!meshesNames.empty()) {
    std::unordered_map<std::string, size_t> nodeMap;
    if (!gltf->nodes.empty()) {
      for (const auto& node : gltf->nodes) {
        if (!node.name.empty()) {
          nodeMap[node.name] = node.index;
        }
      }
    }

    const auto& names = meshesNames;
    for (const auto& name : names) {
      if (!stl_util::contains(nodeMap, name)) {
        throw std::runtime_error(
          String::printf("Failed to find node %s", name.c_str()));
      }
      nodes.emplace_back(nodeMap[name]);
    }
  }

  ImportMeshResult result;
  _loadAsync(nodes, [this, &result]() -> void {
    result = {
      _getMeshes(),         // meshes
      {},                   // particleSystems
      _getSkeletons(),      // skeletons
      _getAnimationGroups() // animationGroups
    };
  });

  return result;
}

void GLTFLoader::loadAsync(
  Scene* scene, const IGLTFLoaderData& data, const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  babylonScene      = scene;
  _rootUrl          = rootUrl;
  _fileName         = !fileName.empty() ? fileName : "scene";
  _progressCallback = onProgress;
  _loadData(data);
  _loadAsync({}, []() -> void {});
}

void GLTFLoader::_loadAsync(const std::vector<size_t> nodes,
                            const std::function<void()>& resultFunc)
{
  _uniqueRootUrl
    = (!String::contains(_rootUrl, "file:") && !_fileName.empty()) ?
        _rootUrl :
        String::printf("%s%ld/", _rootUrl.c_str(), Time::unixtimeInMs());

  _loadExtensions();
  _checkExtensions();

  const std::string loadingToReadyCounterName    = "LOADING => READY";
  const std::string loadingToCompleteCounterName = "LOADING => COMPLETE";

  _parent->_startPerformanceCounter(loadingToReadyCounterName);
  _parent->_startPerformanceCounter(loadingToCompleteCounterName);

  _setState(GLTFLoaderState::LOADING);
  _extensionsOnLoading();

  std::vector<std::function<void()>> promises;

  if (!nodes.empty()) {
    GLTF2::IScene scene;
    scene.nodes = nodes;
    promises.emplace_back(
      [this, scene]() -> void { loadSceneAsync("/nodes", scene); });
  }
  else if (gltf->scene.has_value()) {
    const auto& scene = ArrayItem::Get("/scene", gltf->scenes, *gltf->scene);
    promises.emplace_back([this, scene]() -> void {
      loadSceneAsync(String::printf("/scenes/%ld", scene.index), scene);
    });
  }

  if (_parent->compileMaterials) {
    promises.emplace_back([this]() -> void { _compileMaterialsAsync(); });
  }

  if (_parent->compileShadowGenerators) {
    promises.emplace_back(
      [this]() -> void { _compileShadowGeneratorsAsync(); });
  }

  for (auto&& promise : promises) {
    promise();
  }

  // LOADING => READY
  if (_rootBabylonMesh) {
    _rootBabylonMesh->setEnabled(true);
  }

  _setState(GLTFLoaderState::READY);
  _extensionsOnReady();

  _startAnimations();

  resultFunc();

  // READY => COMPLETE
  _parent->_endPerformanceCounter(loadingToReadyCounterName);

  if (!_disposed) {
    _parent->_endPerformanceCounter(loadingToCompleteCounterName);

    _setState(GLTFLoaderState::COMPLETE);

    _parent->onCompleteObservable.notifyObservers(nullptr);
    _parent->onCompleteObservable.clear();

    dispose();
  }
}

void GLTFLoader::_loadData(const IGLTFLoaderData& data)
{
  gltf = nullptr; // Parse json !
  _setupData();

  if (data.bin.has_value()) {
    const auto& buffers = gltf->buffers;
    if (!buffers.empty() && !buffers[0].uri.empty()) {
      const auto& binaryBuffer = buffers[0];
      if (binaryBuffer.byteLength < data.bin.byteLength - 3
          || binaryBuffer.byteLength > data.bin.byteLength) {
        BABYLON_LOGF_WARN("GLTFLoader",
                          "Binary buffer length (%ld) from JSON does not match "
                          "chunk length (%ld)",
                          binaryBuffer.byteLength, data.bin.byteLength);
      }

      binaryBuffer._data = data.bin;
    }
    else {
      BABYLON_LOG_WARN("GLTFLoader", "Unexpected BIN chunk");
    }
  }
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
  for (const auto& name : GLTFLoader::_ExtensionNames) {
    const auto& extension = GLTFLoader::_ExtensionFactories[name](*this);
    _extensions[name]     = extension;

    _parent->onExtensionLoadedObservable.notifyObservers(extension);
  }

  _parent->onExtensionLoadedObservable.clear();
}

void GLTFLoader::_checkExtensions()
{
  if (!gltf->extensionsRequired.empty()) {
    for (const auto& name : gltf->extensionsRequired) {
      if (!stl_util::contains(_extensions, name)
          || !_extensions[name].enabled) {
        throw std::runtime_error(String::printf(
          "Required extension %s is not available", name.c_str()));
      }
    }
  }
}

void GLTFLoader::_setState(const GLTFLoaderState& state)
{
  _state = state;
}

INode GLTFLoader::_createRootNode()
{
  _rootBabylonMesh = Mesh::New("__root__", babylonScene);
  _rootBabylonMesh->setEnabled(false);

  INode rootNode;
  rootNode._babylonTransformNode = _rootBabylonMesh;

  switch (_parent->coordinateSystemMode) {
    case GLTFLoaderCoordinateSystemMode::AUTO: {
      if (!babylonScene->useRightHandedSystem()) {
        rootNode.rotation = {0.f, 1.f, 0.f, 0.f};
        rootNode.scale    = {1.f, 1.f, -1.f};
        GLTFLoader::_LoadTransform(rootNode, _rootBabylonMesh);
      }
      break;
    }
    case GLTFLoaderCoordinateSystemMode::FORCE_RIGHT_HANDED: {
      babylonScene->useRightHandedSystem = true;
      break;
    }
    default: {
      throw std::runtime_error("Invalid coordinate system mode");
    }
  }

  _parent->onMeshLoadedObservable.notifyObservers(_rootBabylonMesh.get());
  return rootNode;
}

bool GLTFLoader::loadSceneAsync(const std::string& context, const IScene& scene)
{
  const auto extensionPromise = _extensionsLoadSceneAsync(context, scene);
  if (extensionPromise) {
    return extensionPromise;
  }

  std::vector<std::function<void()>> promises;

  logOpen(String::printf("%s %s", context.c_str(), scene.name.c_str()));

  if (!scene.nodes.empty()) {
    for (const auto& index : scene.nodes) {
      const auto& node
        = ArrayItem::Get(String::printf("%s/nodes/%ld", context.c_str(), index),
                         gltf->nodes, index);
      promises.emplace_back([&]() -> void {
        loadNodeAsync(String::printf("/nodes/%ld", node.index), node,
                      [&](const TransformNodePtr& babylonMesh) -> void {
                        babylonMesh->parent = _rootBabylonMesh.get();
                      });
      });
    }
  }

  // Link all Babylon bones for each glTF node with the corresponding Babylon
  // transform node. A glTF joint is a pointer to a glTF node in the glTF node
  // hierarchy similar to Unity3D.
  if (!gltf->nodes.empty()) {
    for (const auto& node : gltf->nodes) {
      if (node._babylonTransformNode && !node._babylonBones.empty()) {
        for (auto& babylonBone : node._babylonBones) {
          babylonBone->linkTransformNode(node._babylonTransformNode);
        }
      }
    }
  }

  promises.emplace_back([this]() -> void { _loadAnimationsAsync(); });

  logClose();

  for (auto&& promise : promises) {
    promise();
  }

  return true;
}

void GLTFLoader::_forEachPrimitive(
  const INode& node,
  const std::function<void(const AbstractMeshPtr& babylonMesh)>& callback)
{
  if (!node._primitiveBabylonMeshes.empty()) {
    for (const auto& babylonMesh : node._primitiveBabylonMeshes) {
      callback(babylonMesh);
    }
  }
}

std::vector<AbstractMeshPtr> GLTFLoader::_getMeshes()
{
  std::vector<AbstractMeshPtr> meshes;

  // Root mesh is always first.
  meshes.emplace_back(_rootBabylonMesh);

  const auto& nodes = gltf->nodes;
  if (!nodes.empty()) {
    for (const auto& node : nodes) {
      _forEachPrimitive(node,
                        [&meshes](const AbstractMeshPtr& babylonMesh) -> void {
                          meshes.emplace_back(babylonMesh);
                        });
    }
  }

  return meshes;
}

std::vector<SkeletonPtr> GLTFLoader::_getSkeletons()
{
  std::vector<SkeletonPtr> skeletons;

  const auto& skins = gltf->skins;
  if (!skins.empty()) {
    for (const auto& skin : skins) {
      if (skin._data.has_value()) {
        skeletons.emplace_back(skin._data->babylonSkeleton);
      }
    }
  }

  return skeletons;
}

std::vector<AnimationGroupPtr> GLTFLoader::_getAnimationGroups()
{
  std::vector<AnimationGroupPtr> animationGroups;

  const auto& animations = gltf->animations;
  if (!animations.empty()) {
    for (const auto& animation : animations) {
      if (animation._babylonAnimationGroup.has_value()) {
        animationGroups.emplace_back(*animation._babylonAnimationGroup);
      }
    }
  }

  return animationGroups;
}

void GLTFLoader::_startAnimations()
{
  switch (_parent->animationStartMode) {
    case GLTFLoaderAnimationStartMode::NONE: {
      // do nothing
      break;
    }
    case GLTFLoaderAnimationStartMode::FIRST: {
      const auto babylonAnimationGroups = _getAnimationGroups();
      if (!babylonAnimationGroups.empty()) {
        babylonAnimationGroups[0]->start(true);
      }
      break;
    }
    case GLTFLoaderAnimationStartMode::ALL: {
      const auto babylonAnimationGroups = _getAnimationGroups();
      for (const auto& babylonAnimationGroup : babylonAnimationGroups) {
        babylonAnimationGroup->start(true);
      }
      break;
    }
    default: {
      BABYLON_LOG_ERROR("GLTFLoader", "Invalid animation start mode");
      return;
    }
  }
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

void GLTFLoader::_createMorphTargets(const std::string& context, INode& node,
                                     const IMesh& mesh,
                                     const IMeshPrimitive& primitive,
                                     const MeshPtr& babylonMesh)
{
  if (primitive.targets.empty()) {
    return;
  }

  if (!node._numMorphTargets.has_value()) {
    node._numMorphTargets = primitive.targets.size();
  }
  else if (primitive.targets.size() != *node._numMorphTargets) {
    throw std::runtime_error(
      String::printf("%s: Primitives do not have the same number of targets",
                     context.c_str()));
  }

  babylonMesh->morphTargetManager = MorphTargetManager::New(babylonScene);
  for (size_t index = 0; index < primitive.targets.size(); ++index) {
    const auto weight
      = (index < node.weights.size()) ? node.weights[index] : 0.f;
    babylonMesh->morphTargetManager()->addTarget(
      MorphTarget::New(String::printf("morphTarget%ld", index), weight));
    // TODO: tell the target whether it has positions, normals, tangents
  }
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
  std::unordered_map<size_t, BonePtr> babylonBones;
  for (const auto& index : skin.joints) {
    auto& node
      = ArrayItem::Get(String::printf("%s/joints/%d", context.c_str(), index),
                       gltf->nodes, index);
    _loadBone(node, skin, babylonSkeleton, babylonBones);
  }
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
  if (!skin.inverseBindMatrices.has_value()) {
    return {};
  }

  const auto& accessor
    = ArrayItem::Get(String::printf("%s/inverseBindMatrices", context.c_str()),
                     gltf->accessors, *skin.inverseBindMatrices);
  return _loadFloatAccessorAsync(
    String::printf("/accessors/%ld", accessor.index), accessor);
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
  if (sampler._data.has_value()) {
    return *sampler._data;
  }

  const auto interpolation = sampler.interpolation.value_or(
    IGLTF2::AnimationSamplerInterpolation::LINEAR);
  switch (interpolation) {
    case IGLTF2::AnimationSamplerInterpolation::STEP:
    case IGLTF2::AnimationSamplerInterpolation::LINEAR:
    case IGLTF2::AnimationSamplerInterpolation::CUBICSPLINE: {
      break;
    }
    default: {
      throw std::runtime_error(
        String::printf("%s/interpolation: Invalid value", context.c_str()));
    }
  }

  const auto& inputAccessor
    = ArrayItem::Get(String::printf("%s/input", context.c_str()),
                     gltf->accessors, sampler.input);
  const auto& outputAccessor
    = ArrayItem::Get(String::printf("%s/output", context.c_str()),
                     gltf->accessors, sampler.output);
  _IAnimationSamplerData samplerDdata{
    _loadFloatAccessorAsync(
      String::printf("/accessors/%ld", inputAccessor.index),
      inputAccessor), // input
    interpolation,    // interpolation
    _loadFloatAccessorAsync(
      String::printf("/accessors/%ld", outputAccessor.index),
      outputAccessor), // output
  };
  sampler._data = std::move(samplerDdata);

  return *sampler._data;
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
  if (bufferView._data.has_value()) {
    return *bufferView._data;
  }

  const auto& buffer
    = ArrayItem::Get(String::printf("%s/buffer", context.c_str()),
                     gltf->buffers, bufferView.buffer);
  const auto data
    = _loadBufferAsync(String::printf("/buffers/%ld", buffer.index), buffer);
  try {
    bufferView._data = stl_util::to_array<uint8_t>(
      data.uint8Array, data.byteOffset + (bufferView.byteOffset || 0),
      bufferView.byteLength);
  }
  catch (const std::exception& e) {
    throw new std::runtime_error(
      String::printf("%s: %s", context.c_str(), e.what()));
  }

  return *bufferView._data;
}

IndicesArray GLTFLoader::_loadIndicesAccessorAsync(const std::string& context,
                                                   const IAccessor& accessor)
{
  if (accessor.type != IGLTF2::AccessorType::SCALAR) {
    throw new std::runtime_error(
      String::printf("%s/type: Invalid value", context.c_str()));
  }

  if (accessor.componentType != IGLTF2::AccessorComponentType::UNSIGNED_BYTE
      && accessor.componentType != IGLTF2::AccessorComponentType::UNSIGNED_SHORT
      && accessor.componentType
           != IGLTF2::AccessorComponentType::UNSIGNED_INT) {
    throw new std::runtime_error(
      String::printf("%s/componentType: Invalid value", context.c_str()));
  }

  if (accessor._data.has_value()) {
    return *accessor._data;
  }

  const auto& bufferView
    = ArrayItem::Get(String::printf("%s/bufferView", context.c_str()),
                     gltf->bufferViews, *accessor.bufferView);
  const auto data = loadBufferViewAsync(
    String::printf("/bufferViews/%ld", bufferView.index), bufferView);
  accessor._data = GLTFLoader::_GetTypedArray(
    context, accessor.componentType, data, accessor.byteOffset, accessor.count);

  return *accessor._data;
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
  const std::string& context,
  std::optional<IMaterialPbrMetallicRoughness> properties,
  const MaterialPtr& babylonMaterial)
{
  auto babylonPBRMaterial
    = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      String::printf("%s: Material type not supported", context.c_str()));
  }

  std::vector<std::function<BaseTexturePtr()>> promises;

  if (properties.has_value()) {
    if (!properties->baseColorFactor.empty()) {
      babylonPBRMaterial->albedoColor
        = Color3::FromArray(properties->baseColorFactor);
      babylonPBRMaterial->alpha = properties->baseColorFactor[3];
    }
    else {
      babylonPBRMaterial->albedoColor = Color3::White();
    }

    babylonPBRMaterial->metallic  = properties->metallicFactor.value_or(1.f);
    babylonPBRMaterial->roughness = properties->roughnessFactor.value_or(1.f);

    if (properties->baseColorTexture.has_value()) {
      promises.emplace_back([&]() {
        loadTextureInfoAsync(
          String::printf("%s/baseColorTexture", context.c_str()),
          *properties->baseColorTexture,
          [&](const BaseTexturePtr& texture) -> void {
            texture->name = String::printf("%s (Base Color)",
                                           babylonPBRMaterial->name.c_str());
            babylonPBRMaterial->albedoTexture = texture;
          })
      });
    }

    if (properties->metallicRoughnessTexture.has_value()) {
      promises.emplace_back([&]() {
        loadTextureInfoAsync(
          String::printf("%s/metallicRoughnessTexture", context.c_str()),
          *properties->metallicRoughnessTexture,
          [&](const BaseTexturePtr& texture) -> void {
            texture->name = String::printf("%s (Metallic Roughness)",
                                           babylonPBRMaterial->name.c_str());
            babylonPBRMaterial->metallicTexture = texture;
          })
      });

      babylonPBRMaterial->useMetallnessFromMetallicTextureBlue = true;
      babylonPBRMaterial->useRoughnessFromMetallicTextureGreen = true;
      babylonPBRMaterial->useRoughnessFromMetallicTextureAlpha = false;
    }
  }

  for (auto&& promise : promises) {
    promise();
  }

  return;
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
  const auto extensionPromise
    = _extensionsCreateMaterial(context, material, babylonDrawMode);
  if (extensionPromise) {
    return extensionPromise;
  }

  const auto name = !material.name.empty() ?
                      material.name :
                      String::printf("material%ld", material.index);
  const auto babylonMaterial = _createDefaultMaterial(name, babylonDrawMode);

  return babylonMaterial;
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
  auto babylonPBRMaterial
    = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      String::printf("%s: Material type not supported", context.c_str()));
  }

  std::vector<std::function<BaseTexturePtr()>> promises;

  babylonPBRMaterial->emissiveColor
    = !material.emissiveFactor.empty() ?
        Color3::FromArray(material.emissiveFactor) :
        Color3(0.f, 0.f, 0.f);
  if (material.doubleSided.has_value() && *material.doubleSided) {
    babylonPBRMaterial->backFaceCulling  = false;
    babylonPBRMaterial->twoSidedLighting = true;
  }

  if (material.normalTexture.has_value()) {
    promises.emplace_back([&]() {
      loadTextureInfoAsync(
        String::printf("%s/normalTexture", context.c_str()),
        *material.normalTexture, [&](const BaseTexturePtr& texture) -> void {
          texture->name
            = String::printf("%s (Normal)", babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->bumpTexture = texture;
        })
    });

    babylonPBRMaterial->invertNormalMapX = !babylonScene->useRightHandedSystem;
    babylonPBRMaterial->invertNormalMapY = babylonScene->useRightHandedSystem;
    if (material.normalTexture->scale.has_value()) {
      babylonPBRMaterial->bumpTexture()->level = *material.normalTexture->scale;
    }

    babylonPBRMaterial->forceIrradianceInFragment = true;
  }

  if (material.occlusionTexture) {
    promises.emplace_back([&]() {
      loadTextureInfoAsync(
        String::printf("%s/occlusionTexture", context.c_str()),
        *material.occlusionTexture, [&](const BaseTexturePtr& texture) -> void {
          texture->name                      = String::printf("%s (Occlusion)",
                                         babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->ambientTexture = texture;
        })
    });

    babylonPBRMaterial->useAmbientInGrayScale = true;
    if (material.occlusionTexture->strength.has_value()) {
      babylonPBRMaterial->ambientTextureStrength
        = *material.occlusionTexture->strength;
    }
  }

  if (material.emissiveTexture) {
    promises.emplace_back([&]() {
      loadTextureInfoAsync(
        String::printf("%s/emissiveTexture", context.c_str()),
        *material.emissiveTexture, [&](const BaseTexturePtr& texture) -> void {
          texture->name
            = String::printf("%s (Emissive)", babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->emissiveTexture = texture;
        })
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  return;
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
      babylonPBRMaterial->alphaCutOff = material.alphaCutoff.value_or(0.5f);
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
  const auto extensionPromise
    = _extensionsLoadTextureInfoAsync(context, textureInfo, assign);
  if (extensionPromise) {
    return extensionPromise;
  }

  logOpen(context);

  const auto texture
    = ArrayItem::Get(String::printf("%s/index", context.c_str()),
                     gltf->textures, textureInfo.index);
  const auto promise = _loadTextureAsync(
    String::printf("/textures/%s", textureInfo.index), texture,
    [&](const BaseTexturePtr& babylonTexture) -> void {
      babylonTexture->coordinatesIndex = textureInfo.texCoord.value_or(0u);

      GLTFLoader::AddPointerMetadata(babylonTexture, context);
      _parent->onTextureLoadedObservable.notifyObservers(babylonTexture.get());
      assign(babylonTexture);
    });

  logClose();

  return promise;
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
  if (image._data.has_value()) {
    logOpen(String::printf("%s %s", context.c_str(), image.name.c_str()));

    if (!image.uri.empty()) {
      image._data
        = loadUriAsync(String::printf("%s/uri", context.c_str()), image.uri);
    }
    else {
      const auto& bufferView
        = ArrayItem::Get(String::printf("%s/bufferView", context.c_str()),
                         gltf->bufferViews, *image.bufferView);
      image._data = loadBufferViewAsync(
        String::printf("/bufferViews/%ld", bufferView.index), bufferView);
    }

    logClose();
  }

  return image._data;
}

ArrayBufferView GLTFLoader::loadUriAsync(const std::string& context,
                                         const std::string& uri)
{
}

void GLTFLoader::_onProgress()
{
}

void GLTFLoader::AddPointerMetadata(const BaseTexturePtr& babylonObject,
                                    const std::string& pointer)
{
}

unsigned int
GLTFLoader::_GetTextureWrapMode(const std::string& context,
                                std::optional<IGLTF2::TextureWrapMode> mode)
{
  // Set defaults if undefined
  mode = mode.value_or(IGLTF2::TextureWrapMode::REPEAT);

  switch (*mode) {
    case IGLTF2::TextureWrapMode::CLAMP_TO_EDGE:
      return TextureConstants::CLAMP_ADDRESSMODE;
    case IGLTF2::TextureWrapMode::MIRRORED_REPEAT:
      return TextureConstants::MIRROR_ADDRESSMODE;
    case IGLTF2::TextureWrapMode::REPEAT:
      return TextureConstants::WRAP_ADDRESSMODE;
    default:
      BABYLON_LOGF_WARN("GLTFLoader", "%s: Invalid value", context.c_str());
      return TextureConstants::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFLoader::_GetTextureSamplingMode(const std::string& context,
                                                 const ISampler& sampler)
{
  // Set defaults if undefined
  const auto magFilter
    = sampler.magFilter.value_or(IGLTF2::TextureMagFilter::LINEAR);
  const auto minFilter = sampler.minFilter.value_or(
    IGLTF2::TextureMinFilter::LINEAR_MIPMAP_LINEAR);

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
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value",
                          context.c_str());
        return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
  else {
    if (magFilter != IGLTF2::TextureMagFilter::NEAREST) {
      BABYLON_LOGF_WARN("GLTFLoader", "%s/magFilter: Invalid value",
                        context.c_str());
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
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value",
                          context.c_str());
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
  byteOffset         = bufferView.byteOffset + byteOffset.value_or(0);

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
    String::printf("%s: Invalid mesh primitive mode", context.c_str()));
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

bool GLTFLoader::_extensionsLoadSceneAsync(const std::string& context,
                                           const IScene& scene)
{
  return false;
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
