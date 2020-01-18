#include <babylon/loading/glTF/2.0/gltf_loader.h>

#include <babylon/animations/animation_group.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/animations/targeted_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/logging.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader_extension.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/morph/morph_target.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {
namespace GLTF2 {

std::vector<std::string> GLTFLoader::_ExtensionNames;
std::unordered_map<std::string, std::function<IGLTFLoaderExtensionPtr(GLTFLoader& loader)>>
  GLTFLoader::_ExtensionFactories;

void GLTFLoader::RegisterExtension(
  const std::string& name,
  const std::function<IGLTFLoaderExtensionPtr(GLTFLoader& loader)>& factory)
{
  if (GLTFLoader::UnregisterExtension(name)) {
    BABYLON_LOGF_WARN("GLTFLoader", "Extension with the name '%s' already exists", name.c_str())
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

  GLTFLoader::_ExtensionFactories.erase(name);
  stl_util::erase(GLTFLoader::_ExtensionNames, name);

  return true;
}

GLTFLoader::GLTFLoader(GLTFFileLoader& parent)
    : _disposed{false}
    , _parent{parent}
    , _gltf{nullptr}
    , _babylonScene{nullptr}
    , _rootBabylonMesh{nullptr}
    , _progressCallback{nullptr}
{
}

GLTFLoader::~GLTFLoader() = default;

GLTFLoaderState& GLTFLoader::state()
{
  return _state;
}

std::unique_ptr<IGLTF>& GLTFLoader::gltf()
{
  return _gltf;
}

Scene* GLTFLoader::babylonScene()
{
  return _babylonScene;
}

MeshPtr GLTFLoader::rootBabylonMesh()
{
  return _rootBabylonMesh;
}

void GLTFLoader::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
}

ImportedMeshes GLTFLoader::importMeshAsync(
  const std::vector<std::string>& meshesNames, Scene* scene, const IGLTFLoaderData& data,
  const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  _babylonScene     = scene;
  _rootUrl          = rootUrl;
  _fileName         = !fileName.empty() ? fileName : "scene";
  _progressCallback = onProgress;
  _loadData(data);

  std::vector<size_t> nodes;

  if (!meshesNames.empty()) {
    std::unordered_map<std::string, size_t> nodeMap;
    if (!_gltf->nodes.empty()) {
      for (const auto& node : _gltf->nodes) {
        if (!node->name.empty()) {
          nodeMap[node->name] = node->index;
        }
      }
    }

    const auto& names = meshesNames;
    for (const auto& name : names) {
      if (!stl_util::contains(nodeMap, name)) {
        throw std::runtime_error(StringTools::printf("Failed to find node %s", name.c_str()));
      }
      nodes.emplace_back(nodeMap[name]);
    }
  }

  ImportedMeshes result;
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
  _babylonScene     = scene;
  _rootUrl          = rootUrl;
  _fileName         = !fileName.empty() ? fileName : "scene";
  _progressCallback = onProgress;
  _loadData(data);
  _loadAsync({}, []() -> void {});
}

void GLTFLoader::_loadAsync(const std::vector<size_t>& nodes,
                            const std::function<void()>& resultFunc)
{
  _uniqueRootUrl = (!StringTools::contains(_rootUrl, "file:") && !_fileName.empty()) ?
                     _rootUrl :
                     StringTools::printf("%s%ld/", _rootUrl.c_str(), Time::unixtimeInMs());

  _loadExtensions();
  _checkExtensions();

  const std::string loadingToReadyCounterName    = "LOADING => READY";
  const std::string loadingToCompleteCounterName = "LOADING => COMPLETE";

  _parent._startPerformanceCounter(loadingToReadyCounterName);
  _parent._startPerformanceCounter(loadingToCompleteCounterName);

  _setState(GLTFLoaderState::LOADING);
  _extensionsOnLoading();

  std::vector<std::function<void()>> promises;

  // Block the marking of materials dirty until the scene is loaded.
  const auto oldBlockMaterialDirtyMechanism  = _babylonScene->blockMaterialDirtyMechanism();
  _babylonScene->blockMaterialDirtyMechanism = true;

  if (!nodes.empty()) {
    GLTF2::IScene scene;
    scene.nodes = nodes;
    promises.emplace_back([this, scene]() -> void { loadSceneAsync("/nodes", scene); });
  }
  else if (_gltf->scene.has_value() || !_gltf->scenes.empty()) {
    const auto& scene = ArrayItem::Get("/scene", _gltf->scenes, _gltf->scene.value_or(0));
    promises.emplace_back([this, scene]() -> void {
      loadSceneAsync(StringTools::printf("/scenes/%ld", scene.index), scene);
    });
  }

  // Restore the blocking of material dirty.
  _babylonScene->blockMaterialDirtyMechanism = oldBlockMaterialDirtyMechanism;

  if (_parent.compileMaterials) {
    promises.emplace_back([this]() -> void { _compileMaterialsAsync(); });
  }

  if (_parent.compileShadowGenerators) {
    promises.emplace_back([this]() -> void { _compileShadowGeneratorsAsync(); });
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
  _parent._endPerformanceCounter(loadingToReadyCounterName);

  if (!_disposed) {
    _parent._endPerformanceCounter(loadingToCompleteCounterName);

    _setState(GLTFLoaderState::COMPLETE);

    _parent.onCompleteObservable.notifyObservers(nullptr);
    _parent.onCompleteObservable.clear();

    dispose();
  }
}

void GLTFLoader::_loadData(const IGLTFLoaderData& data)
{
  _gltf = IGLTF::Parse(data.jsonObject);
  _setupData();

  if (data.bin.has_value()) {
    const auto& buffers = _gltf->buffers;
    if (!buffers.empty() && !buffers[0].uri.empty()) {
      const auto& binaryBuffer = buffers[0];
      if (binaryBuffer.byteLength < data.bin->byteLength() - 3
          || binaryBuffer.byteLength > data.bin->byteLength()) {
        BABYLON_LOGF_WARN("GLTFLoader",
                          "Binary buffer length (%ld) from JSON does not match "
                          "chunk length (%ld)",
                          binaryBuffer.byteLength, data.bin->byteLength())
      }

      // binaryBuffer._data = data.bin;
    }
    else {
      BABYLON_LOG_WARN("GLTFLoader", "Unexpected BIN chunk")
    }
  }
}

void GLTFLoader::_setupData()
{
  ArrayItem::Assign(_gltf->accessors);
  ArrayItem::Assign(_gltf->animations);
  ArrayItem::Assign(_gltf->buffers);
  ArrayItem::Assign(_gltf->bufferViews);
  ArrayItem::Assign(_gltf->cameras);
  ArrayItem::Assign(_gltf->images);
  ArrayItem::Assign(_gltf->materials);
  ArrayItem::Assign(_gltf->meshes);
  ArrayItem::Assign(_gltf->nodes);
  ArrayItem::Assign(_gltf->samplers);
  ArrayItem::Assign(_gltf->scenes);
  ArrayItem::Assign(_gltf->skins);
  ArrayItem::Assign(_gltf->textures);

  if (!_gltf->nodes.empty()) {
    std::unordered_map<size_t, size_t> nodeParents;
    for (const auto& node : _gltf->nodes) {
      if (!node->children.empty()) {
        for (const auto& index : node->children) {
          nodeParents[index] = node->index;
        }
      }
    }

    auto rootNode = _createRootNode();
    for (auto& node : _gltf->nodes) {
      if (stl_util::contains(nodeParents, node->index)) {
        const auto& parentIndex = nodeParents[node->index];
        node->parent            = _gltf->nodes[parentIndex];
      }
      else {
        node->parent = rootNode;
      }
    }
  }
}

void GLTFLoader::_loadExtensions()
{
  for (const auto& name : GLTFLoader::_ExtensionNames) {
    const auto& extension = GLTFLoader::_ExtensionFactories[name](*this);
    _extensions[name]     = extension;

    // _parent->onExtensionLoadedObservable.notifyObservers(extension);
  }

  _parent.onExtensionLoadedObservable.clear();
}

void GLTFLoader::_checkExtensions()
{
  if (!_gltf->extensionsRequired.empty()) {
    for (const auto& name : _gltf->extensionsRequired) {
      if (!stl_util::contains(_extensions, name) || !_extensions[name]->enabled) {
        throw std::runtime_error(
          StringTools::printf("Required extension %s is not available", name.c_str()));
      }
    }
  }
}

void GLTFLoader::_setState(const GLTFLoaderState& iState)
{
  _state = iState;
}

INodePtr GLTFLoader::_createRootNode()
{
  _rootBabylonMesh = Mesh::New("__root__", _babylonScene);
  _rootBabylonMesh->setEnabled(false);

  auto rootNode                   = std::make_shared<INode>();
  rootNode->_babylonTransformNode = _rootBabylonMesh;

  switch (_parent.coordinateSystemMode) {
    case GLTFLoaderCoordinateSystemMode::AUTO: {
      if (!_babylonScene->useRightHandedSystem()) {
        rootNode->rotation = {0.f, 1.f, 0.f, 0.f};
        rootNode->scale    = {1.f, 1.f, -1.f};
        GLTFLoader::_LoadTransform(*rootNode, _rootBabylonMesh);
      }
      break;
    }
    case GLTFLoaderCoordinateSystemMode::FORCE_RIGHT_HANDED: {
      _babylonScene->useRightHandedSystem = true;
      break;
    }
    default: {
      throw std::runtime_error("Invalid coordinate system mode");
    }
  }

  _parent.onMeshLoadedObservable.notifyObservers(_rootBabylonMesh.get());
  return rootNode;
}

bool GLTFLoader::loadSceneAsync(const std::string& context, const IScene& scene)
{
  const auto extensionPromise = _extensionsLoadSceneAsync(context, scene);
  if (extensionPromise) {
    return extensionPromise;
  }

  std::vector<std::function<void()>> promises;

  logOpen(StringTools::printf("%s %s", context.c_str(), scene.name.c_str()));

  if (!scene.nodes.empty()) {
    for (const auto& index : scene.nodes) {
      auto& node = ArrayItem::Get(StringTools::printf("%s/nodes/%ld", context.c_str(), index),
                                  _gltf->nodes, index);
      promises.emplace_back([&]() -> void {
        loadNodeAsync(StringTools::printf("/nodes/%ld", node->index), *node,
                      [&](const TransformNodePtr& babylonMesh) -> void {
                        babylonMesh->parent = _rootBabylonMesh.get();
                      });
      });
    }
  }

  // Link all Babylon bones for each glTF node with the corresponding Babylon
  // transform node. A glTF joint is a pointer to a glTF node in the glTF node
  // hierarchy similar to Unity3D.
  if (!_gltf->nodes.empty()) {
    for (const auto& node : _gltf->nodes) {
      if (node->_babylonTransformNode && !node->_babylonBones.empty()) {
        for (auto& babylonBone : node->_babylonBones) {
          babylonBone->linkTransformNode(node->_babylonTransformNode);
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
  const INode& node, const std::function<void(const AbstractMeshPtr& babylonMesh)>& callback)
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

  const auto& nodes = _gltf->nodes;
  if (!nodes.empty()) {
    for (const auto& node : nodes) {
      _forEachPrimitive(*node, [&meshes](const AbstractMeshPtr& babylonMesh) -> void {
        meshes.emplace_back(babylonMesh);
      });
    }
  }

  return meshes;
}

std::vector<SkeletonPtr> GLTFLoader::_getSkeletons()
{
  std::vector<SkeletonPtr> skeletons;

  const auto& skins = _gltf->skins;
  if (!skins.empty()) {
    for (const auto& skin : skins) {
      if (skin._data) {
        skeletons.emplace_back(skin._data->babylonSkeleton);
      }
    }
  }

  return skeletons;
}

std::vector<AnimationGroupPtr> GLTFLoader::_getAnimationGroups()
{
  std::vector<AnimationGroupPtr> animationGroups;
  const auto& animations = _gltf->animations;
  if (!animations.empty()) {
    for (const auto& animation : animations) {
      if (animation._babylonAnimationGroup) {
        animationGroups.emplace_back(animation._babylonAnimationGroup);
      }
    }
  }
  return animationGroups;
}

void GLTFLoader::_startAnimations()
{
  switch (_parent.animationStartMode) {
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
      BABYLON_LOG_ERROR("GLTFLoader", "Invalid animation start mode")
      return;
    }
  }
}

TransformNodePtr GLTFLoader::loadNodeAsync(
  const std::string& context, INode& node,
  const std::function<void(const TransformNodePtr& babylonTransformNode)>& assign)
{
  const auto extensionPromise = _extensionsLoadNodeAsync(context, node, assign);
  if (extensionPromise) {
    return extensionPromise;
  }

  if (node._babylonTransformNode) {
    throw std::runtime_error(
      StringTools::printf("%s: Invalid recursive node hierarchy", context.c_str()));
  }

  logOpen(StringTools::printf("%s %s", context.c_str(), node.name.c_str()));

  const auto loadNode = [&](const TransformNodePtr& babylonTransformNode) -> void {
    GLTFLoader::AddPointerMetadata(babylonTransformNode, context);
    GLTFLoader::_LoadTransform(node, babylonTransformNode);

    if (node.camera.has_value()) {
      const auto& camera = ArrayItem::Get(StringTools::printf("%s/camera", context.c_str()),
                                          _gltf->cameras, *node.camera);
      loadCameraAsync(StringTools::printf("/cameras/%ld", camera.index), camera,
                      [&](const CameraPtr& babylonCamera) -> void {
                        babylonCamera->parent = babylonTransformNode.get();
                      });
    }

    if (!node.children.empty()) {
      for (const auto& index : node.children) {
        auto& childNode = ArrayItem::Get(
          StringTools::printf("%s/children/%ld", context.c_str(), index), _gltf->nodes, index);
        loadNodeAsync(StringTools::printf("/nodes/%ld", childNode->index), *childNode,
                      [&](const TransformNodePtr& childBabylonMesh) -> void {
                        childBabylonMesh->parent = babylonTransformNode.get();
                      });
      }
    }

    assign(babylonTransformNode);
  };

  if (!node.mesh.has_value()) {
    const auto nodeName
      = !node.name.empty() ? node.name : StringTools::printf("node%ld", node.index);
    node._babylonTransformNode = TransformNode::New(nodeName, _babylonScene);
    loadNode(node._babylonTransformNode);
  }
  else {
    auto& mesh
      = ArrayItem::Get(StringTools::printf("%s/mesh", context.c_str()), _gltf->meshes, *node.mesh);
    _loadMeshAsync(StringTools::printf("/meshes/%ld", mesh.index), node, mesh, loadNode);
  }

  logClose();

  _forEachPrimitive(node, [](const AbstractMeshPtr& babylonMesh) -> void {
    babylonMesh->refreshBoundingInfo(true);
  });

  return node._babylonTransformNode;
}

TransformNodePtr GLTFLoader::_loadMeshAsync(
  const std::string& context, INode& node, IMesh& mesh,
  const std::function<void(const TransformNodePtr& babylonTransformNode)>& assign)
{
  auto& primitives = mesh.primitives;
  if (primitives.empty()) {
    throw std::runtime_error(StringTools::printf("%s: Primitives are missing", context.c_str()));
  }

  if (!primitives[0].index) {
    ArrayItem::Assign(primitives);
  }

  std::vector<std::function<void()>> promises;

  logOpen(StringTools::printf("%s %s", context.c_str(), mesh.name.c_str()));

  const auto name = !node.name.empty() ? node.name : StringTools::printf("node%ld", node.index);

  if (primitives.size() == 1) {
    auto& primitive = mesh.primitives[0];
    promises.emplace_back([&]() -> void {
      _loadMeshPrimitiveAsync(
        StringTools::printf("%s/primitives/%ld", context.c_str(), primitive.index), name, node,
        mesh, primitive, [&node](const TransformNodePtr& babylonMesh) -> void {
          node._babylonTransformNode   = babylonMesh;
          node._primitiveBabylonMeshes = {std::static_pointer_cast<AbstractMesh>(babylonMesh)};
        });
    });
  }
  else {
    node._babylonTransformNode = TransformNode::New(name, _babylonScene);
    node._primitiveBabylonMeshes.clear();
    for (auto& primitive : primitives) {
      promises.emplace_back([this, &context, &mesh, &name, &node, &primitive]() -> void {
        _loadMeshPrimitiveAsync(
          StringTools::printf("%s/primitives/%ld", context.c_str(), primitive.index),
          StringTools::printf("%s_primitive%ld", name.c_str(), primitive.index), node, mesh,
          primitive, [&](const TransformNodePtr& babylonMesh) -> void {
            node._babylonTransformNode = babylonMesh;
            node._primitiveBabylonMeshes.emplace_back(
              std::static_pointer_cast<AbstractMesh>(babylonMesh));
          });
      });
    }
  }

  if (node.skin.has_value()) {
    auto& skin
      = ArrayItem::Get(StringTools::printf("%s/skin", context.c_str()), _gltf->skins, *node.skin);
    promises.emplace_back([this, &node, &skin]() -> void {
      _loadSkinAsync(StringTools::printf("/skins/%ld", skin.index), node, skin);
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  assign(node._babylonTransformNode);

  logClose();

  return node._babylonTransformNode;
}

AbstractMeshPtr GLTFLoader::_loadMeshPrimitiveAsync(
  const std::string& context, const std::string& name, INode& node, const IMesh& mesh,
  IMeshPrimitive& primitive, const std::function<void(const AbstractMeshPtr& babylonMesh)>& assign)
{
  logOpen(context);

  const auto canInstance = (!node.skin.has_value() && mesh.primitives[0].targets.empty());

  AbstractMeshPtr babylonAbstractMesh = nullptr;

  std::vector<std::function<void()>> promises;

  const auto& instanceData = primitive._instanceData;
  if (canInstance && instanceData) {
    babylonAbstractMesh = std::static_pointer_cast<AbstractMesh>(
      instanceData->babylonSourceMesh->createInstance(name));
  }
  else {
    const auto babylonMesh                       = Mesh::New(name, _babylonScene);
    babylonMesh->overrideMaterialSideOrientation = _babylonScene->useRightHandedSystem() ?
                                                     Material::CounterClockWiseSideOrientation :
                                                     Material::ClockWiseSideOrientation;

    _createMorphTargets(context, node, mesh, primitive, babylonMesh);
    promises.emplace_back([&]() -> void {
      auto babylonGeometry = _loadVertexDataAsync(context, primitive, babylonMesh);
      _loadMorphTargetsAsync(context, primitive, babylonMesh, babylonGeometry);
      babylonGeometry->applyToMesh(babylonMesh.get());
    });

    const auto babylonDrawMode = GLTFLoader::_GetDrawMode(context, primitive.mode);
    if (!primitive.material.has_value()) {
      auto babylonMaterial = stl_util::contains(_defaultBabylonMaterialData, babylonDrawMode) ?
                               _defaultBabylonMaterialData[babylonDrawMode] :
                               nullptr;
      if (!babylonMaterial) {
        babylonMaterial = _createDefaultMaterial("__GLTFLoader._default", babylonDrawMode);
        _parent.onMaterialLoadedObservable.notifyObservers(babylonMaterial.get());
        _defaultBabylonMaterialData[babylonDrawMode] = babylonMaterial;
      }
      babylonMesh->material = babylonMaterial;
    }
    else {
      auto& material = ArrayItem::Get(StringTools::printf("%s/material", context.c_str()),
                                      _gltf->materials, *primitive.material);
      promises.emplace_back([&]() -> void {
        _loadMaterialAsync(StringTools::printf("/materials/%ld", material.index), material,
                           babylonMesh, babylonDrawMode,
                           [&](const MaterialPtr& babylonMaterial) -> void {
                             babylonMesh->material = babylonMaterial;
                           });
      });
    }

    if (canInstance) {
      if (!primitive._instanceData) {
        primitive._instanceData = std::make_unique<IMeshPrimitive::IMeshPrimitiveData>();
      }
      primitive._instanceData->babylonSourceMesh = babylonMesh;
    }

    babylonAbstractMesh = babylonMesh;
  }

  GLTFLoader::AddPointerMetadata(babylonAbstractMesh, context);
  _parent.onMeshLoadedObservable.notifyObservers(babylonAbstractMesh.get());
  assign(babylonAbstractMesh);

  logClose();

  for (auto&& promise : promises) {
    promise();
  }

  return babylonAbstractMesh;
}

GeometryPtr GLTFLoader::_loadVertexDataAsync(const std::string& context, IMeshPrimitive& primitive,
                                             const MeshPtr& babylonMesh)
{
  const auto extensionPromise = _extensionsLoadVertexDataAsync(context, primitive, babylonMesh);
  if (extensionPromise) {
    return extensionPromise;
  }

  auto& attributes = primitive.attributes;
  if (attributes.empty()) {
    throw std::runtime_error(StringTools::printf("%s: Attributes are missing", context.c_str()));
  }

  std::vector<std::function<void()>> promises;

  auto babylonGeometry = Geometry::New(babylonMesh->name, _babylonScene);

  if (!primitive.indices.has_value()) {
    babylonMesh->isUnIndexed = true;
  }
  else {
    auto& accessor = ArrayItem::Get(StringTools::printf("%s/indices", context.c_str()),
                                    _gltf->accessors, *primitive.indices);
    promises.emplace_back([this, &babylonGeometry, &accessor]() {
      auto data = _loadIndicesAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index),
                                            accessor);
      babylonGeometry->setIndices(data);
    });
  }

  const auto loadAttribute
    = [&](const std::string& attribute, const std::string& kind,
          const std::function<void(const IAccessor& accessor)>& callback = nullptr) -> void {
    if (!stl_util::contains(attributes, attribute)) {
      return;
    }

    // if (stl_util::contains(babylonMesh->_delayInfo, kind)) {
    //   babylonMesh->_delayInfo.emplace_back(kind);
    // }

    auto& accessor
      = ArrayItem::Get(StringTools::printf("%s/attributes/%s", context.c_str(), attribute.c_str()),
                       _gltf->accessors, attributes[attribute]);
    promises.emplace_back([this, &babylonGeometry, &accessor, kind]() -> void {
      babylonGeometry->setVerticesBuffer(
        _loadVertexAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor,
                                 kind),
        accessor.count);
    });

    if (callback) {
      callback(accessor);
    }
  };

  loadAttribute("POSITION", VertexBuffer::PositionKind);
  loadAttribute("NORMAL", VertexBuffer::NormalKind);
  loadAttribute("TANGENT", VertexBuffer::TangentKind);
  loadAttribute("TEXCOORD_0", VertexBuffer::UVKind);
  loadAttribute("TEXCOORD_1", VertexBuffer::UV2Kind);
  loadAttribute("JOINTS_0", VertexBuffer::MatricesIndicesKind);
  loadAttribute("WEIGHTS_0", VertexBuffer::MatricesWeightsKind);
  loadAttribute("COLOR_0", VertexBuffer::ColorKind, [&](const IAccessor& accessor) -> void {
    if (accessor.type == IGLTF2::AccessorType::VEC4) {
      babylonMesh->hasVertexAlpha = true;
    }
  });

  for (auto&& promise : promises) {
    promise();
  }

  return babylonGeometry;
}

void GLTFLoader::_createMorphTargets(const std::string& context, INode& node, const IMesh& mesh,
                                     const IMeshPrimitive& primitive, const MeshPtr& babylonMesh)
{
  if (primitive.targets.empty()) {
    return;
  }

  if (!node._numMorphTargets.has_value()) {
    node._numMorphTargets = primitive.targets.size();
  }
  else if (primitive.targets.size() != *node._numMorphTargets) {
    throw std::runtime_error(StringTools::printf(
      "%s: Primitives do not have the same number of targets", context.c_str()));
  }

  babylonMesh->morphTargetManager = MorphTargetManager::New(babylonMesh->getScene());
  for (size_t index = 0; index < primitive.targets.size(); ++index) {
    const auto weight = (index < node.weights.size()) ?
                          node.weights[index] :
                          (index < mesh.weights.size()) ? mesh.weights[index] : 0.f;
    babylonMesh->morphTargetManager()->addTarget(MorphTarget::New(
      StringTools::printf("morphTarget%ld", index), weight, babylonMesh->getScene()));
    // TODO: tell the target whether it has positions, normals, tangents
  }
}

void GLTFLoader::_loadMorphTargetsAsync(const std::string& context, const IMeshPrimitive& primitive,
                                        const MeshPtr& babylonMesh,
                                        const GeometryPtr& babylonGeometry)
{
  if (primitive.targets.empty()) {
    return;
  }

  const auto& morphTargetManager = babylonMesh->morphTargetManager();
  for (size_t index = 0; index < morphTargetManager->numTargets; ++index) {
    const auto babylonMorphTarget = morphTargetManager->getTarget(index);
    _loadMorphTargetVertexDataAsync(StringTools::printf("%s/targets/%ld", context.c_str(), index),
                                    babylonGeometry, primitive.targets[index], babylonMorphTarget);
  }
}

void GLTFLoader::_loadMorphTargetVertexDataAsync(
  const std::string& context, const GeometryPtr& babylonGeometry,
  const std::unordered_map<std::string, size_t>& attributes,
  const MorphTargetPtr& babylonMorphTarget)
{
  const auto loadAttribute
    = [this, &attributes, &babylonGeometry, &context](
        std::string attribute, const std::string& kind,
        const std::function<void(const VertexBufferPtr& babylonVertexBuffer, Float32Array& data)>&
          setData) -> void {
    if (!stl_util::contains(attributes, attribute)) {
      return;
    }

    const auto babylonVertexBuffer = babylonGeometry->getVertexBuffer(kind);
    if (!babylonVertexBuffer) {
      return;
    }

    auto& accessor
      = ArrayItem::Get(StringTools::printf("%s/%s", context.c_str(), attribute.c_str()),
                       _gltf->accessors, attributes.at(attribute));
    auto data
      = _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor);
    setData(babylonVertexBuffer, data);
  };

  loadAttribute("POSITION", VertexBuffer::PositionKind,
                [&](const VertexBufferPtr& babylonVertexBuffer, const Float32Array& data) -> void {
                  Float32Array positions(data.size());
                  babylonVertexBuffer->forEach(positions.size(),
                                               [&](float value, size_t index) -> void {
                                                 positions[index] = data[index] + value;
                                               });

                  babylonMorphTarget->setPositions(positions);
                });

  loadAttribute("NORMAL", VertexBuffer::NormalKind,
                [&](const VertexBufferPtr& babylonVertexBuffer, const Float32Array& data) -> void {
                  Float32Array normals(data.size());
                  babylonVertexBuffer->forEach(normals.size(),
                                               [&](float value, size_t index) -> void {
                                                 normals[index] = data[index] + value;
                                               });

                  babylonMorphTarget->setNormals(normals);
                });

  loadAttribute("TANGENT", VertexBuffer::TangentKind,
                [&](const VertexBufferPtr& babylonVertexBuffer, const Float32Array& data) -> void {
                  Float32Array tangents(data.size() / 3 * 4);
                  auto dataIndex = 0ull;
                  babylonVertexBuffer->forEach(tangents.size(),
                                               [&](float value, size_t index) -> void {
                                                 // Tangent data for morph targets is stored as xyz
                                                 // delta. The vertexData.tangent is stored as xyzw.
                                                 // So we need to skip every fourth
                                                 // vertexData.tangent.
                                                 if (((index + 1) % 4) != 0) {
                                                   tangents[dataIndex] = data[dataIndex] + value;
                                                   ++dataIndex;
                                                 }
                                               });
                  babylonMorphTarget->setTangents(tangents);
                });
}

void GLTFLoader::_LoadTransform(const INode& node, const TransformNodePtr& babylonNode)
{
  // Ignore the TRS of skinned nodes.
  // See
  // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skins
  // (second implementation note)
  if (node.skin.has_value()) {
    return;
  }

  std::optional<Vector3> position    = Vector3::Zero();
  std::optional<Quaternion> rotation = Quaternion::Identity();
  std::optional<Vector3> scaling     = Vector3::One();

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

  babylonNode->position           = *position;
  babylonNode->rotationQuaternion = *rotation;
  babylonNode->scaling            = *scaling;
}

void GLTFLoader::_loadSkinAsync(const std::string& context, const INode& node, ISkin& skin)
{
  const auto& assignSkeleton = [&](const SkeletonPtr& skeleton) -> void {
    _forEachPrimitive(
      node, [&](const AbstractMeshPtr& babylonMesh) -> void { babylonMesh->skeleton = skeleton; });
  };

  if (skin._data) {
    assignSkeleton(skin._data->babylonSkeleton);
    return;
  }

  const auto skeletonId = StringTools::printf("skeleton%ld", skin.index);
  const auto babylonSkeleton
    = Skeleton::New(!skin.name.empty() ? skin.name : skeletonId, skeletonId, _babylonScene);

  // See
  // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skins
  // (second implementation note)
  babylonSkeleton->overrideMesh = _rootBabylonMesh;

  _loadBones(context, skin, babylonSkeleton);
  assignSkeleton(babylonSkeleton);

  const auto& promise = [&]() {
    auto inverseBindMatricesData = _loadSkinInverseBindMatricesDataAsync(context, skin);
    _updateBoneMatrices(babylonSkeleton, inverseBindMatricesData);
  };

  skin._data = ISkin::ISkinData{
    babylonSkeleton // babylonSkeleton
  };

  promise();
}

void GLTFLoader::_loadBones(const std::string& context, const ISkin& skin,
                            const SkeletonPtr& babylonSkeleton)
{
  std::unordered_map<size_t, BonePtr> babylonBones;
  for (const auto& index : skin.joints) {
    auto& node = ArrayItem::Get(StringTools::printf("%s/joints/%d", context.c_str(), index),
                                _gltf->nodes, index);
    _loadBone(*node, skin, babylonSkeleton, babylonBones);
  }
}

BonePtr GLTFLoader::_loadBone(INode& node, const ISkin& skin, const SkeletonPtr& babylonSkeleton,
                              std::unordered_map<size_t, BonePtr>& babylonBones)
{
  if (stl_util::contains(babylonBones, node.index) && babylonBones[node.index]) {
    return babylonBones[node.index];
  }

  BonePtr babylonParentBone = nullptr;
  if (node.parent && node.parent->_babylonTransformNode != _rootBabylonMesh) {
    babylonParentBone = _loadBone(*node.parent, skin, babylonSkeleton, babylonBones);
  }

  const auto boneIndex = stl_util::index_of(skin.joints, node.index);

  auto nodeName    = !node.name.empty() ? node.name : StringTools::printf("joint%ld", node.index);
  auto babylonBone = Bone::New(nodeName, babylonSkeleton.get(), babylonParentBone.get(),
                               _getNodeMatrix(node), std::nullopt, std::nullopt, boneIndex);
  babylonBones[node.index] = babylonBone;

  node._babylonBones.emplace_back(babylonBone);

  return babylonBone;
}

Float32Array GLTFLoader::_loadSkinInverseBindMatricesDataAsync(const std::string& context,
                                                               const ISkin& skin)
{
  if (!skin.inverseBindMatrices.has_value()) {
    return {};
  }

  auto& accessor = ArrayItem::Get(StringTools::printf("%s/inverseBindMatrices", context.c_str()),
                                  _gltf->accessors, *skin.inverseBindMatrices);
  return _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor);
}

void GLTFLoader::_updateBoneMatrices(const SkeletonPtr& babylonSkeleton,
                                     const Float32Array& inverseBindMatricesData)
{
  for (const auto& babylonBone : babylonSkeleton->bones) {
    auto baseMatrix      = Matrix::Identity();
    const auto boneIndex = babylonBone->_index;
    if (!inverseBindMatricesData.empty() && boneIndex.has_value() && *boneIndex != -1) {
      Matrix::FromArrayToRef(inverseBindMatricesData, static_cast<unsigned int>(*boneIndex * 16),
                             baseMatrix);
      baseMatrix.invertToRef(baseMatrix);
    }

    const auto& babylonParentBone = babylonBone->getParent();
    if (babylonParentBone) {
      baseMatrix.multiplyToRef(babylonParentBone->getInvertedAbsoluteTransform(), baseMatrix);
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
             !node.scale.empty() ? Vector3::FromArray(node.scale) : Vector3::One(),
             !node.rotation.empty() ? Quaternion::FromArray(node.rotation) : Quaternion::Identity(),
             !node.translation.empty() ? Vector3::FromArray(node.translation) : Vector3::Zero());
}

CameraPtr
GLTFLoader::loadCameraAsync(const std::string& context, const ICamera& camera,
                            const std::function<void(const CameraPtr& babylonCamera)>& assign)
{
  const auto extensionPromise = _extensionsLoadCameraAsync(context, camera, assign);
  if (extensionPromise) {
    return extensionPromise;
  }

  logOpen(StringTools::printf("%s %s", context.c_str(), camera.name.c_str()));

  const auto babylonCamera = FreeCamera::New(
    !camera.name.empty() ? camera.name : StringTools::printf("camera%ld", camera.index),
    Vector3::Zero(), _babylonScene, false);
  babylonCamera->rotation = Vector3(0.f, Math::PI, 0.f);

  switch (camera.type) {
    case IGLTF2::CameraType::PERSPECTIVE: {
      const auto& perspective = camera.perspective;
      if (!perspective) {
        throw std::runtime_error(
          StringTools::printf("%s: Camera perspective properties are missing", context.c_str()));
      }

      babylonCamera->fov  = perspective->yfov;
      babylonCamera->minZ = perspective->znear;
      babylonCamera->maxZ
        = perspective->zfar.has_value() ? *perspective->zfar : std::numeric_limits<float>::max();
      break;
    }
    case IGLTF2::CameraType::ORTHOGRAPHIC: {
      if (!camera.orthographic) {
        throw std::runtime_error(
          StringTools::printf("%s: Camera orthographic properties are missing", context.c_str()));
      }

      babylonCamera->mode        = Camera::ORTHOGRAPHIC_CAMERA;
      babylonCamera->orthoLeft   = -camera.orthographic->xmag;
      babylonCamera->orthoRight  = camera.orthographic->xmag;
      babylonCamera->orthoBottom = -camera.orthographic->ymag;
      babylonCamera->orthoTop    = camera.orthographic->ymag;
      babylonCamera->minZ        = camera.orthographic->znear;
      babylonCamera->maxZ        = camera.orthographic->zfar;
      break;
    }
    default: {
      throw std::runtime_error(StringTools::printf("%s: Invalid camera type", context.c_str()));
    }
  }

  GLTFLoader::AddPointerMetadata(babylonCamera, context);
  _parent.onCameraLoadedObservable.notifyObservers(babylonCamera.get());
  assign(babylonCamera);

  return babylonCamera;
}

void GLTFLoader::_loadAnimationsAsync()
{
  auto& animations = _gltf->animations;
  if (animations.empty()) {
    return;
  }

  std::vector<std::function<AnimationGroupPtr()>> promises;

  for (auto& animation : animations) {
    promises.emplace_back([this, &animation]() -> AnimationGroupPtr {
      return loadAnimationAsync(StringTools::printf("/animations/%ld", animation.index), animation);
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  return;
}

AnimationGroupPtr GLTFLoader::loadAnimationAsync(const std::string& context, IAnimation& animation)
{
  const auto animationGroup = _extensionsLoadAnimationAsync(context, animation);
  if (animationGroup) {
    return animationGroup;
  }

  const auto babylonAnimationGroup = AnimationGroup::New(
    !animation.name.empty() ? animation.name : StringTools::printf("animation%ld", animation.index),
    _babylonScene);
  animation._babylonAnimationGroup = babylonAnimationGroup;

  std::vector<std::function<void()>> promises;

  ArrayItem::Assign(animation.channels);
  ArrayItem::Assign(animation.samplers);

  for (const auto& channel : animation.channels) {
    promises.emplace_back([&]() -> void {
      _loadAnimationChannelAsync(
        StringTools::printf("%s/channels/%ld", context.c_str(), channel.index), context, animation,
        channel, babylonAnimationGroup);
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  babylonAnimationGroup->normalize(0);
  return babylonAnimationGroup;
}

void GLTFLoader::_loadAnimationChannelAsync(const std::string& context,
                                            const std::string& animationContext,
                                            IAnimation& animation, const IAnimationChannel& channel,
                                            const AnimationGroupPtr& babylonAnimationGroup,
                                            const IAnimatablePtr& animationTargetOverride)
{
  if (!channel.target.node.has_value()) {
    return;
  }

  const auto& targetNode = ArrayItem::Get(StringTools::printf("%s/target/node", context.c_str()),
                                          _gltf->nodes, *channel.target.node);

  // Ignore animations that have no animation targets.
  if ((channel.target.path == IGLTF2::AnimationChannelTargetPath::WEIGHTS
       && !targetNode->_numMorphTargets)
      || (channel.target.path != IGLTF2::AnimationChannelTargetPath::WEIGHTS
          && !targetNode->_babylonTransformNode)) {
    return;
  }

  auto& sampler = ArrayItem::Get(StringTools::printf("%s/sampler", context.c_str()),
                                 animation.samplers, channel.sampler);
  auto data     = _loadAnimationSamplerAsync(
    StringTools::printf("%s/samplers/%ld", animationContext.c_str(), channel.sampler), sampler);

  std::string targetPath;
  unsigned int animationType;
  switch (channel.target.path) {
    case IGLTF2::AnimationChannelTargetPath::TRANSLATION: {
      targetPath    = "position";
      animationType = Animation::ANIMATIONTYPE_VECTOR3;
      break;
    }
    case IGLTF2::AnimationChannelTargetPath::ROTATION: {
      targetPath    = "rotationQuaternion";
      animationType = Animation::ANIMATIONTYPE_QUATERNION;
      break;
    }
    case IGLTF2::AnimationChannelTargetPath::SCALE: {
      targetPath    = "scaling";
      animationType = Animation::ANIMATIONTYPE_VECTOR3;
      break;
    }
    case IGLTF2::AnimationChannelTargetPath::WEIGHTS: {
      targetPath    = "influence";
      animationType = Animation::ANIMATIONTYPE_FLOAT;
      break;
    }
    default: {
      throw std::runtime_error(
        StringTools::printf("%s/target/path: Invalid value", context.c_str()));
    }
  }

  unsigned int outputBufferOffset = 0;
  std::function<AnimationValue()> getNextOutputValue;
  if (targetPath == "position") {
    getNextOutputValue = [&]() -> AnimationValue {
      const auto value = Vector3::FromArray(data.output, outputBufferOffset);
      outputBufferOffset += 3;
      return value;
    };
  }
  else if (targetPath == "rotationQuaternion") {
    getNextOutputValue = [&]() -> AnimationValue {
      const auto value = Quaternion::FromArray(data.output, outputBufferOffset);
      outputBufferOffset += 4;
      return value;
    };
  }
  else if (targetPath == "scaling") {
    getNextOutputValue = [&]() -> AnimationValue {
      const auto value = Vector3::FromArray(data.output, outputBufferOffset);
      outputBufferOffset += 3;
      return value;
    };
  }
  else if (targetPath == "influence") {
    getNextOutputValue = [&]() -> AnimationValue {
      Float32Array value(*targetNode->_numMorphTargets);
      for (size_t i = 0; i < *targetNode->_numMorphTargets; ++i) {
        value[i] = data.output[outputBufferOffset++];
      }
      return value;
    };
  }

  std::function<IAnimationKey(size_t frameIndex)> getNextKey;
  switch (data.interpolation) {
    case IGLTF2::AnimationSamplerInterpolation::STEP: {
      getNextKey = [&](size_t frameIndex) -> IAnimationKey {
        auto interpolation = 1; // AnimationKeyInterpolation::STEP
        return IAnimationKey{
          data.input[frameIndex], // frame
          getNextOutputValue(),   // value
          std::nullopt,           // inTangent
          std::nullopt,           // outTangent
          interpolation           // interpolation
        };
      };
      break;
    }
    case IGLTF2::AnimationSamplerInterpolation::LINEAR: {
      getNextKey = [&](size_t frameIndex) -> IAnimationKey {
        return IAnimationKey{
          data.input[frameIndex], // frame
          getNextOutputValue(),   // value
          std::nullopt,           // inTangent
          std::nullopt,           // outTangent
          std::nullopt            // interpolation
        };
      };
      break;
    }
    case IGLTF2::AnimationSamplerInterpolation::CUBICSPLINE: {
      getNextKey = [&](size_t frameIndex) -> IAnimationKey {
        return IAnimationKey{
          data.input[frameIndex], // frame
          getNextOutputValue(),   // value
          getNextOutputValue(),   // inTangent
          getNextOutputValue(),   // outTangent
          std::nullopt            // interpolation
        };
      };
      break;
    }
    default:
      break;
  }

  std::vector<IAnimationKey> keys(data.input.size());
  for (size_t frameIndex = 0; frameIndex < data.input.size(); ++frameIndex) {
    keys[frameIndex] = getNextKey(frameIndex);
  }

  if (targetPath == "influence") {
    for (size_t targetIndex = 0; targetIndex < targetNode->_numMorphTargets; targetIndex++) {
      const auto animationName
        = StringTools::printf("%s_channel%ld", babylonAnimationGroup->name.c_str(),
                              babylonAnimationGroup->targetedAnimations().size());
      auto babylonAnimation = Animation::New(animationName, targetPath, 1, animationType);
      std::vector<IAnimationKey> values;
      for (const auto& key : keys) {
        values.emplace_back(IAnimationKey(key.frame,                                  // frame
                                          key.value.get<Float32Array>()[targetIndex], // value,
                                          key.inTangent,                              // inTangent
                                          key.outTangent,                             // outTangent
                                          key.interpolation // interpolation
                                          ));
      }
      babylonAnimation->setKeys(values);

      _forEachPrimitive(*targetNode, [&](const AbstractMeshPtr& babylonAbstractMesh) -> void {
        const auto babylonMesh = std::static_pointer_cast<Mesh>(babylonAbstractMesh);
        const auto morphTarget = babylonMesh->morphTargetManager()->getTarget(targetIndex);
        const auto babylonAnimationClone = babylonAnimation->clone();
        morphTarget->animations.emplace_back(babylonAnimationClone);
        babylonAnimationGroup->addTargetedAnimation(babylonAnimationClone, morphTarget);
      });
    }
  }
  else {
    const auto animationName
      = StringTools::printf("%s_channel%ld", babylonAnimationGroup->name.c_str(),
                            babylonAnimationGroup->targetedAnimations().size());
    const auto babylonAnimation = Animation::New(animationName, targetPath, 1, animationType);
    babylonAnimation->setKeys(keys);

    if (animationTargetOverride != nullptr && !animationTargetOverride->getAnimations().empty()) {
      animationTargetOverride->getAnimations().emplace_back(babylonAnimation);
      babylonAnimationGroup->addTargetedAnimation(babylonAnimation, animationTargetOverride);
    }
    else {
      targetNode->_babylonTransformNode->getAnimations().emplace_back(babylonAnimation);
      babylonAnimationGroup->addTargetedAnimation(babylonAnimation,
                                                  targetNode->_babylonTransformNode);
    }
  }
}

_IAnimationSamplerData GLTFLoader::_loadAnimationSamplerAsync(const std::string& context,
                                                              IAnimationSampler& sampler)
{
  if (sampler._data.has_value()) {
    return sampler._data.value();
  }

  const auto interpolation
    = sampler.interpolation.value_or(IGLTF2::AnimationSamplerInterpolation::LINEAR);
  switch (interpolation) {
    case IGLTF2::AnimationSamplerInterpolation::STEP:
    case IGLTF2::AnimationSamplerInterpolation::LINEAR:
    case IGLTF2::AnimationSamplerInterpolation::CUBICSPLINE: {
      break;
    }
    default: {
      throw std::runtime_error(
        StringTools::printf("%s/interpolation: Invalid value", context.c_str()));
    }
  }

  auto& inputAccessor  = ArrayItem::Get(StringTools::printf("%s/input", context.c_str()),
                                       _gltf->accessors, sampler.input);
  auto& outputAccessor = ArrayItem::Get(StringTools::printf("%s/output", context.c_str()),
                                        _gltf->accessors, sampler.output);
  _IAnimationSamplerData samplerDdata{
    _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", inputAccessor.index),
                            inputAccessor), // input
    interpolation,                          // interpolation
    _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", outputAccessor.index),
                            outputAccessor), // output
  };
  sampler._data = std::move(samplerDdata);

  return sampler._data.value();
}

ArrayBufferView& GLTFLoader::_loadBufferAsync(const std::string& context, IBuffer& buffer)
{
  if (buffer._data) {
    return buffer._data;
  }

  if (buffer.uri.empty()) {
    throw std::runtime_error(StringTools::printf("%s/uri: Value is missing", context.c_str()));
  }

  buffer._data = loadUriAsync(StringTools::printf("%s/uri", context.c_str()), buffer.uri);

  return buffer._data;
}

ArrayBufferView& GLTFLoader::loadBufferViewAsync(const std::string& context,
                                                 IBufferView& bufferView)
{
  if (bufferView._data) {
    return bufferView._data;
  }

  auto& buffer = ArrayItem::Get(StringTools::printf("%s/buffer", context.c_str()), _gltf->buffers,
                                bufferView.buffer);
  const auto data = _loadBufferAsync(StringTools::printf("/buffers/%ld", buffer.index), buffer);

  // ASYNC_FIXME: We cannot treat the data right now, it will be otained later!
  try {
    bufferView._data = stl_util::to_array<uint8_t>(
      data.uint8Array(), data.byteOffset + (bufferView.byteOffset.value_or(0)),
      bufferView.byteLength);
  }
  catch (const std::exception& e) {
    throw std::runtime_error(StringTools::printf("%s: %s", context.c_str(), e.what()));
  }

  return bufferView._data;
}

template <typename T>
ArrayBufferView& GLTFLoader::_loadAccessorAsync(const std::string& context, IAccessor& accessor)
{
  if (accessor._data.has_value()) {
    return *accessor._data;
  }

  const auto numComponents = GLTFLoader::_GetNumComponents(context, accessor.type);
  const auto byteStride
    = numComponents
      * VertexBuffer::GetTypeByteLength(static_cast<unsigned>(accessor.componentType));
  const auto length = numComponents * accessor.count;

  if (!accessor.bufferView.has_value()) {
    accessor._data = std::vector<T>(length);
  }
  else {
    auto& bufferView = ArrayItem::Get(StringTools::printf("%s/bufferView", context.c_str()),
                                      _gltf->bufferViews, *accessor.bufferView);
    auto data
      = loadBufferViewAsync(StringTools::printf("/bufferViews/%ld", bufferView.index), bufferView);
    if (accessor.componentType == IGLTF2::AccessorComponentType::FLOAT && !accessor.normalized) {
      data = GLTFLoader::_GetTypedArray(context, accessor.componentType, data, accessor.byteOffset,
                                        length);
    }
    else {
      auto typedArray             = Float32Array(length);
      const auto dataFloat32Array = data.float32Array();
      VertexBuffer::ForEach(
        dataFloat32Array, accessor.byteOffset || 0, bufferView.byteStride || byteStride,
        numComponents, static_cast<unsigned>(accessor.componentType), typedArray.size(),
        accessor.normalized || false,
        [&typedArray](float value, size_t index) -> void { typedArray[index] = value; });
      data = typedArray;
    }

    accessor._data = GLTFLoader::_GetTypedArray(context, accessor.componentType, data,
                                                accessor.byteOffset, length);
  }

  if (accessor.sparse) {
    const auto& sparse            = *accessor.sparse;
    const auto accessor_data_then = [this, &context, &sparse, &numComponents,
                                     &accessor](const ArrayBufferView& view) -> Float32Array {
      auto data = view.float32Array();
      auto& indicesBufferView
        = ArrayItem::Get(StringTools::printf("%s/sparse/indices/bufferView", context.c_str()),
                         _gltf->bufferViews, sparse.indices.bufferView);
      auto& valuesBufferView
        = ArrayItem::Get(StringTools::printf("%s/sparse/values/bufferView", context.c_str()),
                         _gltf->bufferViews, sparse.values.bufferView);
      const auto indicesData = loadBufferViewAsync(
        StringTools::printf("/bufferViews/%ld", indicesBufferView.index), indicesBufferView);
      const auto valuesData = loadBufferViewAsync(
        StringTools::printf("/bufferViews/%ld", valuesBufferView.index), valuesBufferView);
      const auto& indices = _castIndicesTo32bit(
        sparse.indices.componentType,
        GLTFLoader::_GetTypedArray(StringTools::printf("%s/sparse/indices", context.c_str()),
                                   sparse.indices.componentType, indicesData,
                                   sparse.indices.byteOffset, sparse.count));
      const auto values
        = GLTFLoader::_GetTypedArray(StringTools::printf("%s/sparse/values", context.c_str()),
                                     accessor.componentType, valuesData, sparse.values.byteOffset,
                                     numComponents * sparse.count)
            .float32Array();
      size_t valuesIndex = 0;
      for (unsigned int indice : indices) {
        auto dataIndex = indice * numComponents;
        for (size_t componentIndex = 0; componentIndex < numComponents; componentIndex++) {
          data[dataIndex++] = values[valuesIndex++];
        }
      }
      return data;
    };
    accessor._data = accessor_data_then(*accessor._data);
  }

  return *accessor._data;
}

Float32Array GLTFLoader::_loadFloatAccessorAsync(const std::string& context, IAccessor& accessor)
{
  return _loadAccessorAsync<float>(context, accessor).float32Array();
}

IndicesArray GLTFLoader::_castIndicesTo32bit(const IGLTF2::AccessorComponentType& type,
                                             const ArrayBufferView& buffer)
{
  switch (type) {
    case IGLTF2::AccessorComponentType::UNSIGNED_BYTE:
      return stl_util::cast_array_elements<uint32_t, uint8_t>(buffer.uint8Array());
    case IGLTF2::AccessorComponentType::UNSIGNED_SHORT:
      return stl_util::cast_array_elements<uint32_t, uint16_t>(buffer.uint16Array());
    default:
      return buffer.uint32Array();
  }
}

IndicesArray GLTFLoader::_getConverted32bitIndices(IAccessor& accessor)
{
  switch (accessor.componentType) {
    case IGLTF2::AccessorComponentType::UNSIGNED_BYTE:
    case IGLTF2::AccessorComponentType::UNSIGNED_SHORT:
      accessor._data
        = ArrayBufferView(_castIndicesTo32bit(accessor.componentType, *accessor._data));
      break;
    default:
      break;
  }

  return accessor._data->uint32Array();
}

IndicesArray GLTFLoader::_loadIndicesAccessorAsync(const std::string& context, IAccessor& accessor)
{
  if (accessor.type != IGLTF2::AccessorType::SCALAR) {
    throw std::runtime_error(StringTools::printf("%s/type: Invalid value", context.c_str()));
  }

  if (accessor.componentType != IGLTF2::AccessorComponentType::UNSIGNED_BYTE
      && accessor.componentType != IGLTF2::AccessorComponentType::UNSIGNED_SHORT
      && accessor.componentType != IGLTF2::AccessorComponentType::UNSIGNED_INT) {
    throw std::runtime_error(
      StringTools::printf("%s/componentType: Invalid value", context.c_str()));
  }

  if (accessor._data.has_value()) {
    return _getConverted32bitIndices(accessor);
  }

  auto& bufferView = ArrayItem::Get(StringTools::printf("%s/bufferView", context.c_str()),
                                    _gltf->bufferViews, *accessor.bufferView);
  const auto data
    = loadBufferViewAsync(StringTools::printf("/bufferViews/%ld", bufferView.index), bufferView);
  accessor._data = GLTFLoader::_GetTypedArray(context, accessor.componentType, data,
                                              accessor.byteOffset, accessor.count);

  return _getConverted32bitIndices(accessor);
}

BufferPtr GLTFLoader::_loadVertexBufferViewAsync(IBufferView& bufferView,
                                                 const std::string& /*kind*/)
{
  if (bufferView._babylonBuffer) {
    return bufferView._babylonBuffer;
  }

  auto data
    = loadBufferViewAsync(StringTools::printf("/bufferViews/%ld", bufferView.index), bufferView);
  bufferView._babylonBuffer
    = std::make_shared<Buffer>(_babylonScene->getEngine(), data.float32Array(), false);

  return bufferView._babylonBuffer;
}

VertexBufferPtr& GLTFLoader::_loadVertexAccessorAsync(const std::string& context,
                                                      IAccessor& accessor, const std::string& kind)
{
  if (accessor._babylonVertexBuffer) {
    return accessor._babylonVertexBuffer;
  }

  if (accessor.sparse) {
    auto data
      = _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor);
    accessor._babylonVertexBuffer
      = std::make_unique<VertexBuffer>(_babylonScene->getEngine(), data, kind, false);
  }
  // HACK: If byte offset is not a multiple of component type byte length then
  // load as a float array instead of using Babylon buffers.
  else if (accessor.byteOffset
           && static_cast<unsigned int>(*accessor.byteOffset)
                  % VertexBuffer::GetTypeByteLength(
                    static_cast<unsigned int>(accessor.componentType))
                != 0) {
    BABYLON_LOG_WARN("GLTFLoader",
                     "Accessor byte offset is not a multiple of component type byte length")
    auto data
      = _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor);
    accessor._babylonVertexBuffer
      = std::make_unique<VertexBuffer>(_babylonScene->getEngine(), data, kind, false);
  }
  // Load joint indices as a float array since the shaders expect float data but glTF uses unsigned
  // byte/short. This prevents certain platforms (e.g. D3D) from having to convert the data to float
  // on the fly.
  else if (kind == VertexBuffer::MatricesIndicesKind) {
    auto data
      = _loadFloatAccessorAsync(StringTools::printf("/accessors/%ld", accessor.index), accessor);
    accessor._babylonVertexBuffer
      = std::make_unique<VertexBuffer>(_babylonScene->getEngine(), data, kind, false);
  }
  else {
    auto& bufferView   = ArrayItem::Get(StringTools::printf("%s/bufferView", context.c_str()),
                                      _gltf->bufferViews, *accessor.bufferView);
    auto babylonBuffer = _loadVertexBufferViewAsync(bufferView, kind);
    const auto size    = GLTFLoader::_GetNumComponents(context, accessor.type);
    accessor._babylonVertexBuffer = std::make_unique<VertexBuffer>(
      _babylonScene->getEngine(), babylonBuffer.get(), kind, false, false, bufferView.byteStride,
      false, accessor.byteOffset, size,
      IGLTF2::EnumUtils::AccessorComponentTypeToNumber(accessor.componentType),
      accessor.normalized.value_or(false), true);
  }

  return accessor._babylonVertexBuffer;
}

void GLTFLoader::_loadMaterialMetallicRoughnessPropertiesAsync(
  const std::string& context, std::optional<IMaterialPbrMetallicRoughness> properties,
  const MaterialPtr& babylonMaterial)
{
  auto babylonPBRMaterial = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      StringTools::printf("%s: Material type not supported", context.c_str()));
  }

  std::vector<std::function<BaseTexturePtr()>> promises;

  if (properties.has_value()) {
    if (!properties->baseColorFactor.empty()) {
      babylonPBRMaterial->albedoColor = Color3::FromArray(properties->baseColorFactor);
      babylonPBRMaterial->alpha       = properties->baseColorFactor[3];
    }
    else {
      babylonPBRMaterial->albedoColor = Color3::White();
    }

    babylonPBRMaterial->metallic  = properties->metallicFactor.value_or(1.f);
    babylonPBRMaterial->roughness = properties->roughnessFactor.value_or(1.f);

    if (properties->baseColorTexture) {
      promises.emplace_back([&]() -> BaseTexturePtr {
        return loadTextureInfoAsync(
          StringTools::printf("%s/baseColorTexture", context.c_str()),
          *properties->baseColorTexture, [&](const BaseTexturePtr& texture) -> void {
            texture->name
              = StringTools::printf("%s (Base Color)", babylonPBRMaterial->name.c_str());
            babylonPBRMaterial->albedoTexture = texture;
          });
      });
    }

    if (properties->metallicRoughnessTexture) {
      promises.emplace_back([&]() -> BaseTexturePtr {
        return loadTextureInfoAsync(
          StringTools::printf("%s/metallicRoughnessTexture", context.c_str()),
          *properties->metallicRoughnessTexture, [&](const BaseTexturePtr& texture) -> void {
            texture->name
              = StringTools::printf("%s (Metallic Roughness)", babylonPBRMaterial->name.c_str());
            babylonPBRMaterial->metallicTexture = texture;
          });
      });

      babylonPBRMaterial->useMetallnessFromMetallicTextureBlue = true;
      babylonPBRMaterial->useRoughnessFromMetallicTextureGreen = true;
      babylonPBRMaterial->useRoughnessFromMetallicTextureAlpha = false;
    }
  }

  for (auto&& promise : promises) {
    promise();
  }
}

MaterialPtr GLTFLoader::_loadMaterialAsync(
  const std::string& context, IMaterial& material, const MeshPtr& babylonMesh,
  unsigned int babylonDrawMode,
  const std::function<void(const MaterialPtr& babylonMaterial)>& assign)
{
  const auto extensionPromise
    = _extensionsLoadMaterialAsync(context, material, babylonMesh, babylonDrawMode, assign);
  if (extensionPromise) {
    return extensionPromise;
  }

  std::optional<GLTF2::IMaterialData> babylonData = std::nullopt;
  if (stl_util::contains(material._data, babylonDrawMode)) {
    babylonData = material._data[babylonDrawMode];
  }

  if (!babylonData.has_value()) {
    logOpen(StringTools::printf("%s %s", context.c_str(), material.name.c_str()));

    const auto babylonMaterial = createMaterial(context, material, babylonDrawMode);
    loadMaterialPropertiesAsync(context, material, babylonMaterial);

    babylonData = GLTF2::IMaterialData{
      babylonMaterial, // babylonMaterial
      {},              // babylonMeshes
      nullptr          // promise
    };

    material._data[babylonDrawMode] = *babylonData;

    GLTFLoader::AddPointerMetadata(babylonMaterial, context);
    _parent.onMaterialLoadedObservable.notifyObservers(babylonMaterial.get());

    logClose();
  }

  babylonData->babylonMeshes.emplace_back(babylonMesh);

  babylonMesh->onDisposeObservable.addOnce([&](Node*, EventState&) -> void {
    auto it = std::find(babylonData->babylonMeshes.begin(), babylonData->babylonMeshes.end(),
                        babylonMesh);
    if (it != babylonData->babylonMeshes.end()) {
      babylonData->babylonMeshes.erase(it);
    }
  });

  assign(babylonData->babylonMaterial);

  return babylonData->babylonMaterial;
}

MaterialPtr GLTFLoader::_createDefaultMaterial(const std::string& name,
                                               unsigned int babylonDrawMode)
{
  auto babylonMaterial = PBRMaterial::New(name, _babylonScene);
  // Moved to mesh so user can change materials on gltf meshes: babylonMaterial.sideOrientation =
  // this._babylonScene.useRightHandedSystem ? Material.CounterClockWiseSideOrientation :
  // Material.ClockWiseSideOrientation;
  babylonMaterial->fillMode                   = babylonDrawMode;
  babylonMaterial->enableSpecularAntiAliasing = true;
  babylonMaterial->useRadianceOverAlpha       = !_parent.transparencyAsCoverage;
  babylonMaterial->useSpecularOverAlpha       = !_parent.transparencyAsCoverage;
  babylonMaterial->transparencyMode           = PBRMaterial::PBRMATERIAL_OPAQUE;
  babylonMaterial->metallic                   = 1.f;
  babylonMaterial->roughness                  = 1.f;
  return babylonMaterial;
}

MaterialPtr GLTFLoader::createMaterial(const std::string& context, const IMaterial& material,
                                       unsigned int babylonDrawMode)
{
  const auto extensionPromise = _extensionsCreateMaterial(context, material, babylonDrawMode);
  if (extensionPromise) {
    return extensionPromise;
  }

  const auto name
    = !material.name.empty() ? material.name : StringTools::printf("material%ld", material.index);
  const auto babylonMaterial = _createDefaultMaterial(name, babylonDrawMode);

  return babylonMaterial;
}

bool GLTFLoader::loadMaterialPropertiesAsync(const std::string& context, const IMaterial& material,
                                             const MaterialPtr& babylonMaterial)
{
  const auto extensionPromise
    = _extensionsLoadMaterialPropertiesAsync(context, material, babylonMaterial);
  if (extensionPromise) {
    return extensionPromise;
  }

  std::vector<std::function<void()>> promises;

  promises.emplace_back(
    [&]() -> void { loadMaterialBasePropertiesAsync(context, material, babylonMaterial); });

  if (material.pbrMetallicRoughness) {
    promises.emplace_back([&]() -> void {
      _loadMaterialMetallicRoughnessPropertiesAsync(
        StringTools::printf("%s/pbrMetallicRoughness", context.c_str()),
        *material.pbrMetallicRoughness, babylonMaterial);
    });
  }

  loadMaterialAlphaProperties(context, material, babylonMaterial);

  for (auto&& promise : promises) {
    promise();
  }

  return true;
}

void GLTFLoader::loadMaterialBasePropertiesAsync(const std::string& context,
                                                 const IMaterial& material,
                                                 const MaterialPtr& babylonMaterial)
{
  auto babylonPBRMaterial = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      StringTools::printf("%s: Material type not supported", context.c_str()));
  }

  std::vector<std::function<BaseTexturePtr()>> promises;

  babylonPBRMaterial->emissiveColor = !material.emissiveFactor.empty() ?
                                        Color3::FromArray(material.emissiveFactor) :
                                        Color3(0.f, 0.f, 0.f);
  if (material.doubleSided.has_value() && *material.doubleSided) {
    babylonPBRMaterial->backFaceCulling  = false;
    babylonPBRMaterial->twoSidedLighting = true;
  }

  if (material.normalTexture) {
    promises.emplace_back([&]() -> BaseTexturePtr {
      return loadTextureInfoAsync(
        StringTools::printf("%s/normalTexture", context.c_str()), *material.normalTexture,
        [&](const BaseTexturePtr& texture) -> void {
          texture->name = StringTools::printf("%s (Normal)", babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->bumpTexture = texture;
        });
    });

    babylonPBRMaterial->invertNormalMapX = !_babylonScene->useRightHandedSystem;
    babylonPBRMaterial->invertNormalMapY = _babylonScene->useRightHandedSystem;
    if (material.normalTexture->scale.has_value()) {
      babylonPBRMaterial->bumpTexture()->level = *material.normalTexture->scale;
    }

    babylonPBRMaterial->forceIrradianceInFragment = true;
  }

  if (material.occlusionTexture) {
    promises.emplace_back([&]() -> BaseTexturePtr {
      return loadTextureInfoAsync(
        StringTools::printf("%s/occlusionTexture", context.c_str()), *material.occlusionTexture,
        [&](const BaseTexturePtr& texture) -> void {
          texture->name = StringTools::printf("%s (Occlusion)", babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->ambientTexture = texture;
        });
    });

    babylonPBRMaterial->useAmbientInGrayScale = true;
    if (material.occlusionTexture->strength.has_value()) {
      babylonPBRMaterial->ambientTextureStrength = *material.occlusionTexture->strength;
    }
  }

  if (material.emissiveTexture) {
    promises.emplace_back([&]() -> BaseTexturePtr {
      return loadTextureInfoAsync(
        StringTools::printf("%s/emissiveTexture", context.c_str()), *material.emissiveTexture,
        [&](const BaseTexturePtr& texture) -> void {
          texture->name = StringTools::printf("%s (Emissive)", babylonPBRMaterial->name.c_str());
          babylonPBRMaterial->emissiveTexture = texture;
        });
    });
  }

  for (auto&& promise : promises) {
    promise();
  }

  return;
}

void GLTFLoader::loadMaterialAlphaProperties(const std::string& context, const IMaterial& material,
                                             const MaterialPtr& babylonMaterial)
{
  auto babylonPBRMaterial = std::static_pointer_cast<PBRMaterial>(babylonMaterial);
  if (!babylonPBRMaterial) {
    throw std::runtime_error(
      StringTools::printf("%s: Material type not supported", context.c_str()));
  }

  const auto alphaMode
    = material.alphaMode.has_value() ? *material.alphaMode : IGLTF2::MaterialAlphaMode::OPAQUE;
  switch (alphaMode) {
    case IGLTF2::MaterialAlphaMode::OPAQUE: {
      babylonPBRMaterial->transparencyMode = PBRMaterial::PBRMATERIAL_OPAQUE;
      break;
    }
    case IGLTF2::MaterialAlphaMode::MASK: {
      babylonPBRMaterial->transparencyMode = PBRMaterial::PBRMATERIAL_ALPHATEST;
      babylonPBRMaterial->alphaCutOff      = material.alphaCutoff.value_or(0.5f);
      if (babylonPBRMaterial->albedoTexture()) {
        babylonPBRMaterial->albedoTexture()->hasAlpha = true;
      }
      break;
    }
    case IGLTF2::MaterialAlphaMode::BLEND: {
      babylonPBRMaterial->transparencyMode = PBRMaterial::PBRMATERIAL_ALPHABLEND;
      if (babylonPBRMaterial->albedoTexture()) {
        babylonPBRMaterial->albedoTexture()->hasAlpha = true;
        babylonPBRMaterial->useAlphaFromAlbedoTexture = true;
      }
      break;
    }
    default: {
      throw std::runtime_error(StringTools::printf("%s/alphaMode: Invalid value (%d)",
                                                   context.c_str(),
                                                   static_cast<int>(*material.alphaMode)));
    }
  }
}

BaseTexturePtr GLTFLoader::loadTextureInfoAsync(
  const std::string& context, const IGLTF2::ITextureInfo& textureInfo,
  const std::function<void(const BaseTexturePtr& babylonTexture)>& assign)
{
  const auto extensionPromise = _extensionsLoadTextureInfoAsync(context, textureInfo, assign);
  if (extensionPromise) {
    return extensionPromise;
  }

  logOpen(context);

  if (textureInfo.texCoord.value_or(0) >= 2) {
    throw new std::runtime_error(StringTools::printf(
      "%s/texCoord: Invalid value (%d)", context.c_str(), static_cast<int>(*textureInfo.texCoord)));
  }

  const auto& texture = ArrayItem::Get(StringTools::printf("%s/index", context.c_str()),
                                       _gltf->textures, textureInfo.index);
  const auto promise
    = _loadTextureAsync(StringTools::printf("/textures/%ld", textureInfo.index), texture,
                        [&](const BaseTexturePtr& babylonTexture) -> void {
                          babylonTexture->coordinatesIndex = textureInfo.texCoord.value_or(0u);

                          GLTFLoader::AddPointerMetadata(babylonTexture, context);
                          _parent.onTextureLoadedObservable.notifyObservers(babylonTexture.get());
                          assign(babylonTexture);
                        });

  logClose();

  return promise;
}

BaseTexturePtr GLTFLoader::_loadTextureAsync(
  const std::string& context, const ITexture& texture,
  const std::function<void(const BaseTexturePtr& babylonTexture)>& assign)
{
  std::vector<std::function<void()>> promises;

  logOpen(StringTools::printf("%s %s", context.c_str(), texture.name.c_str()));

  ISampler defaultSampler;
  defaultSampler.index = 0;

  auto& sampler = (!texture.sampler.has_value() ?
                     defaultSampler :
                     ArrayItem::Get(StringTools::printf("%s/sampler", context.c_str()),
                                    _gltf->samplers, *texture.sampler));
  const auto samplerData
    = _loadSampler(StringTools::printf("/samplers/%ld", sampler.index), sampler);

  auto& image = ArrayItem::Get(StringTools::printf("%s/source", context.c_str()), _gltf->images,
                               texture.source);
  std::string url;
  if (!image.uri.empty()) {
    if (Tools::IsBase64(image.uri)) {
      url = image.uri;
    }
    else if (!_babylonScene->getEngine()->textureFormatInUse().empty()) {
      // If an image uri and a texture format is set like (eg. KTX) load from
      // url instead of blob to support texture format and fallback
      url = _rootUrl + image.uri;
    }
  }

  auto babylonTexture = Texture::New(
    url, _babylonScene, samplerData.noMipMaps, false, samplerData.samplingMode, nullptr,
    [this, &context](const std::string& message, const std::string& exception) {
      if (!_disposed) {
        throw std::runtime_error(StringTools::printf(
          "%s: %s", context.c_str(),
          !exception.empty() ? exception.c_str() :
                               !message.empty() ? message.c_str() : "Failed to load texture"));
      }
    });

  if (url.empty()) {
    promises.emplace_back([this, &image, &babylonTexture]() -> void {
      const auto data = loadImageAsync(StringTools::printf("/images/%ld", image.index), image);
      const auto name = !image.uri.empty() ?
                          image.uri :
                          StringTools::printf("%s#image%ld", _fileName.c_str(), image.index);
      const auto dataUrl = StringTools::printf("data:%s%s", _uniqueRootUrl.c_str(), name.c_str());
      babylonTexture->updateURL(dataUrl, data.uint8Array());
    });
  }

  babylonTexture->wrapU = samplerData.wrapU;
  babylonTexture->wrapV = samplerData.wrapV;
  assign(babylonTexture);

  logClose();

  for (auto&& promise : promises) {
    promise();
  }

  return babylonTexture;
}

_ISamplerData GLTFLoader::_loadSampler(const std::string& context, ISampler& sampler)
{
  if (!sampler._data) {
    sampler._data = {
      (sampler.minFilter == IGLTF2::TextureMinFilter::NEAREST
       || sampler.minFilter == IGLTF2::TextureMinFilter::LINEAR), // noMipMaps
      GLTFLoader::_GetTextureSamplingMode(context, sampler),      // samplingMode
      GLTFLoader::_GetTextureWrapMode(StringTools::printf("%s/wrapS", context.c_str()),
                                      sampler.wrapS), // wrapU
      GLTFLoader::_GetTextureWrapMode(StringTools::printf("%s/wrapT", context.c_str()),
                                      sampler.wrapT) // wrapV
    };
  }

  return *sampler._data;
}

ArrayBufferView& GLTFLoader::loadImageAsync(const std::string& context, IImage& image)
{
  if (!image._data) {
    logOpen(StringTools::printf("%s %s", context.c_str(), image.name.c_str()));

    if (!image.uri.empty()) {
      image._data = loadUriAsync(StringTools::printf("%s/uri", context.c_str()), image.uri);
    }
    else {
      auto& bufferView = ArrayItem::Get(StringTools::printf("%s/bufferView", context.c_str()),
                                        _gltf->bufferViews, *image.bufferView);
      image._data = loadBufferViewAsync(StringTools::printf("/bufferViews/%ld", bufferView.index),
                                        bufferView);
    }

    logClose();
  }

  return image._data;
}

// ASYNC_FIXME this should return a promise, not a value !!! As in the js code.
ArrayBufferView GLTFLoader::loadUriAsync(const std::string& context, const std::string& uri)
{
  const auto extensionPromise = _extensionsLoadUriAsync(context, uri);
  if (extensionPromise.has_value()) {
    return *extensionPromise;
  }

  if (!GLTFLoader::_ValidateUri(uri)) {
    throw std::runtime_error(
      StringTools::printf("%s: '%s' is invalid", context.c_str(), uri.c_str()));
  }

  if (Tools::IsBase64(uri)) {
    const auto data = Tools::DecodeBase64(uri);
    log(StringTools::printf("Decoded %s... (%ld bytes)", uri.substr(0, 64).c_str(), data.size()));
    return data;
  }

  log(StringTools::printf("Loading %s", uri.c_str()));

  ArrayBuffer data;
  auto url = _parent.preprocessUrlAsync(_rootUrl + uri);
  if (!_disposed) {
    FileTools::LoadFile(
      url,
      [this, &data, &uri](const std::variant<std::string, ArrayBuffer>& fileData,
                          const std::string & /*responseURL*/) -> void {
        if (!_disposed) {
          if (std::holds_alternative<ArrayBuffer>(fileData)) {
            data = std::get<ArrayBuffer>(fileData);
            log(StringTools::printf("Loaded %s (%ld bytes)", uri.c_str(), data.size()));
          }
        }
      },
      nullptr, true,
      [this, &context, &uri](const std::string& message, const std::string& exception) -> void {
        log(StringTools::printf("%s: Failed to load (%s %s)", context.c_str(), uri.c_str(),
                                message.c_str(), exception.c_str()));
      });
  }

  return data;
}

void GLTFLoader::_onProgress()
{
}

void GLTFLoader::AddPointerMetadata(const BaseTexturePtr& /*babylonObject*/,
                                    const std::string& /*pointer*/)
{
}

void GLTFLoader::AddPointerMetadata(const TransformNodePtr& /*babylonObject*/,
                                    const std::string& /*pointer*/)
{
}

void GLTFLoader::AddPointerMetadata(const CameraPtr& /*babylonObject*/,
                                    const std::string& /*pointer*/)
{
}

void GLTFLoader::AddPointerMetadata(const MaterialPtr& /*babylonObject*/,
                                    const std::string& /*pointer*/)
{
}

unsigned int GLTFLoader::_GetTextureWrapMode(const std::string& context,
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
      BABYLON_LOGF_WARN("GLTFLoader", "%s: Invalid value", context.c_str())
      return TextureConstants::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFLoader::_GetTextureSamplingMode(const std::string& context,
                                                 const ISampler& sampler)
{
  // Set defaults if undefined
  const auto magFilter = sampler.magFilter.value_or(IGLTF2::TextureMagFilter::LINEAR);
  const auto minFilter = sampler.minFilter.value_or(IGLTF2::TextureMinFilter::LINEAR_MIPMAP_LINEAR);

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
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value", context.c_str())
        return TextureConstants::LINEAR_LINEAR_MIPLINEAR;
    }
  }
  else {
    if (magFilter != IGLTF2::TextureMagFilter::NEAREST) {
      BABYLON_LOGF_WARN("GLTFLoader", "%s/magFilter: Invalid value", context.c_str())
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
        BABYLON_LOGF_WARN("GLTFLoader", "%s/minFilter: Invalid value", context.c_str())
        return TextureConstants::NEAREST_NEAREST_MIPNEAREST;
    }
  }
}

ArrayBufferView GLTFLoader::_GetTypedArray(const std::string& context,
                                           IGLTF2::AccessorComponentType componentType,
                                           const ArrayBufferView& bufferView,
                                           std::optional<size_t> byteOffset, size_t length)
{
  const auto& buffer = bufferView.uint8Array();
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
        throw std::runtime_error(
          StringTools::printf("Invalid component type %d", static_cast<int>(componentType)));
    }
  }
  catch (const std::exception& e) {
    throw std::runtime_error(StringTools::printf("%s: %s", context.c_str(), e.what()));
  }
}

unsigned int GLTFLoader::_GetNumComponents(const std::string& context, IGLTF2::AccessorType type)
{
  switch (type) {
    case IGLTF2::AccessorType::SCALAR:
      return 1;
    case IGLTF2::AccessorType::VEC2:
      return 2;
    case IGLTF2::AccessorType::VEC3:
      return 3;
    case IGLTF2::AccessorType::VEC4:
      return 4;
    case IGLTF2::AccessorType::MAT2:
      return 4;
    case IGLTF2::AccessorType::MAT3:
      return 9;
    case IGLTF2::AccessorType::MAT4:
      return 16;
    default:
      break;
  }

  throw std::runtime_error(StringTools::printf("%s: Invalid type", context.c_str()));
}

unsigned int GLTFLoader::_GetNumComponents(const std::string& context, const std::string& type)
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
    StringTools::printf("%s: Invalid type (%s)", context.c_str(), type.c_str()));
}

bool GLTFLoader::_ValidateUri(const std::string& uri)
{
  return (Tools::IsBase64(uri) || StringTools::indexOf(uri, "..") == -1);
}

unsigned int GLTFLoader::_GetDrawMode(const std::string& context,
                                      std::optional<IGLTF2::MeshPrimitiveMode> mode)
{
  if (!mode.has_value()) {
    mode = IGLTF2::MeshPrimitiveMode::TRIANGLES;
  }

  switch (*mode) {
    case IGLTF2::MeshPrimitiveMode::POINTS:
      return Material::PointListDrawMode;
    case IGLTF2::MeshPrimitiveMode::LINES:
      return Material::LineListDrawMode;
    case IGLTF2::MeshPrimitiveMode::LINE_LOOP:
      return Material::LineLoopDrawMode;
    case IGLTF2::MeshPrimitiveMode::LINE_STRIP:
      return Material::LineStripDrawMode;
    case IGLTF2::MeshPrimitiveMode::TRIANGLES:
      return Material::TriangleFillMode;
    case IGLTF2::MeshPrimitiveMode::TRIANGLE_STRIP:
      return Material::TriangleStripDrawMode;
    case IGLTF2::MeshPrimitiveMode::TRIANGLE_FAN:
      return Material::TriangleFanDrawMode;
    default:
      break;
  }

  throw std::runtime_error(StringTools::printf("%s: Invalid mesh primitive mode", context.c_str()));
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
      if (extension->enabled) {
        action(*extension);
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

bool GLTFLoader::_extensionsLoadSceneAsync(const std::string& /*context*/, const IScene& /*scene*/)
{
  return false;
}

TransformNodePtr GLTFLoader::_extensionsLoadNodeAsync(
  const std::string& /*context*/, const INode& /*node*/,
  const std::function<void(const TransformNodePtr& babylonTransformNode)>&
  /*assign*/)
{
  return nullptr;
}

CameraPtr GLTFLoader::_extensionsLoadCameraAsync(
  const std::string& /*context*/, const ICamera& /*camera*/,
  const std::function<void(const CameraPtr& babylonCamera)>& /*assign*/)
{
  return nullptr;
}

GeometryPtr GLTFLoader::_extensionsLoadVertexDataAsync(const std::string& /*context*/,
                                                       const IMeshPrimitive& /*primitive*/,
                                                       const MeshPtr& /*babylonMesh*/)
{
  return nullptr;
}

MaterialPtr GLTFLoader::_extensionsLoadMaterialAsync(
  const std::string& /*context*/, const IMaterial& /*material*/, const MeshPtr& /*babylonMesh*/,
  unsigned int /*babylonDrawMode*/,
  const std::function<void(const MaterialPtr& babylonMaterial)>& /*assign*/)
{
  return nullptr;
}

MaterialPtr GLTFLoader::_extensionsCreateMaterial(const std::string& /*context*/,
                                                  const IMaterial& /*material*/,
                                                  unsigned int /*babylonDrawMode*/)
{
  return nullptr;
}

bool GLTFLoader::_extensionsLoadMaterialPropertiesAsync(const std::string& /*context*/,
                                                        const IMaterial& /*material*/,
                                                        const MaterialPtr& /*babylonMaterial*/)
{
  return false;
}

BaseTexturePtr GLTFLoader::_extensionsLoadTextureInfoAsync(
  const std::string& /*context*/, const IGLTF2::ITextureInfo& /*textureInfo*/,
  const std::function<void(const BaseTexturePtr& babylonTexture)>& /*assign*/)
{
  return nullptr;
}

AnimationGroupPtr GLTFLoader::_extensionsLoadAnimationAsync(const std::string& /*context*/,
                                                            const IAnimation& /*animation*/)
{
  return nullptr;
}

std::optional<ArrayBufferView> GLTFLoader::_extensionsLoadUriAsync(const std::string& /*context*/,
                                                                   const std::string& /*uri*/)
{
  return std::nullopt;
}

void GLTFLoader::logOpen(const std::string& message)
{
  _parent._logOpen(message);
}

void GLTFLoader::logClose()
{
  _parent._logClose();
}

void GLTFLoader::log(const std::string& message)
{
  _parent._log(message);
}

void GLTFLoader::startPerformanceCounter(const std::string& counterName)
{
  _parent._startPerformanceCounter(counterName);
}

void GLTFLoader::endPerformanceCounter(const std::string& counterName)
{
  _parent._endPerformanceCounter(counterName);
}

} // end of namespace GLTF2
} // end of namespace BABYLON
