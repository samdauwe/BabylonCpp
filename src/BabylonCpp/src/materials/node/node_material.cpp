#include <babylon/materials/node/node_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/node/blocks/color_splitter_block.h>
#include <babylon/materials/node/blocks/dual/current_screen_block.h>
#include <babylon/materials/node/blocks/dual/reflection_texture_block.h>
#include <babylon/materials/node/blocks/dual/texture_block.h>
#include <babylon/materials/node/blocks/fragment/fragment_output_block.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/blocks/multiply_block.h>
#include <babylon/materials/node/blocks/particle/particle_blend_multiply_block.h>
#include <babylon/materials/node/blocks/particle/particle_ramp_gradient_block.h>
#include <babylon/materials/node/blocks/particle/particle_texture_block.h>
#include <babylon/materials/node/blocks/remap_block.h>
#include <babylon/materials/node/blocks/transform_block.h>
#include <babylon/materials/node/blocks/trigonometry_block.h>
#include <babylon/materials/node/blocks/vector_merger_block.h>
#include <babylon/materials/node/blocks/vertex/vertex_output_block.h>
#include <babylon/materials/node/inode_material_options.h>
#include <babylon/materials/node/node_material_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/node/optimizers/node_material_optimizer.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/particles/base_particle_system.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

bool NodeMaterial::IgnoreTexturesAtLoadTime = false;
size_t NodeMaterial::_BuildIdGenerator      = 0;

NodeMaterial::NodeMaterial(const std::string& iName, Scene* iScene,
                           const INodeMaterialOptionsPtr& options)
    : PushMaterial{iName, iScene}
    , ignoreAlpha{false}
    , maxSimultaneousLights{4}
    , options{this, &NodeMaterial::get_options, &NodeMaterial::set_options}
    , imageProcessingConfiguration{this, &NodeMaterial::get_imageProcessingConfiguration,
                                   &NodeMaterial::set_imageProcessingConfiguration}
    , _mode{NodeMaterialModes::Material}
    , mode(this, &NodeMaterial::get_mode)
    , _imageProcessingConfiguration{nullptr}
    , _options{nullptr}
    , _vertexCompilationState{nullptr}
    , _fragmentCompilationState{nullptr}
    , _sharedData{nullptr}
    , _buildId{NodeMaterial::_BuildIdGenerator++}
    , _buildWasSuccessful{false}
    , _animationFrame{-1}
    , _imageProcessingObserver{nullptr}
{
  _options = options;

  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);
}

NodeMaterial::~NodeMaterial() = default;

INodeMaterialOptionsPtr& NodeMaterial::get_options()
{
  return _options;
}

void NodeMaterial::set_options(const INodeMaterialOptionsPtr& value)
{
  _options = value;
}

ImageProcessingConfigurationPtr& NodeMaterial::get_imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void NodeMaterial::set_imageProcessingConfiguration(const ImageProcessingConfigurationPtr& value)
{
  _attachImageProcessingConfiguration(value);

  // Ensure the effect will be rebuilt.
  _markAllSubMeshesAsTexturesDirty();
}

NodeMaterialModes& NodeMaterial::get_mode()
{
  return _mode;
}

std::string NodeMaterial::getClassName() const
{
  return "NodeMaterial";
}

void NodeMaterial::_attachImageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& configuration)
{
  if (configuration == _imageProcessingConfiguration) {
    return;
  }

  // Detaches observer.
  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(_imageProcessingObserver);
  }

  // Pick the scene configuration if needed.
  if (!configuration) {
    _imageProcessingConfiguration = getScene()->imageProcessingConfiguration();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  if (_imageProcessingConfiguration) {
    _imageProcessingObserver = _imageProcessingConfiguration->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*ipc*/, EventState& /*es*/) -> void {
        _markAllSubMeshesAsImageProcessingDirty();
      });
  }
}

NodeMaterialBlockPtr NodeMaterial::getBlockByName(const std::string& iName) const
{
  NodeMaterialBlockPtr result = nullptr;
  for (const auto& block : attachedBlocks) {
    if (block->name() == iName) {
      if (!result) {
        result = block;
      }
      else {
        BABYLON_LOGF_WARN("NodeMaterial", "More than one block was found with the name '%s'",
                          name.c_str())
        return result;
      }
    }
  }

  return result;
}

NodeMaterialBlockPtr NodeMaterial::getBlockByPredicate(
  const std::function<bool(const NodeMaterialBlockPtr& block)>& predicate)
{
  for (const auto& block : attachedBlocks) {
    if (predicate(block)) {
      return block;
    }
  }

  return nullptr;
}

InputBlockPtr NodeMaterial::getInputBlockByPredicate(
  const std::function<bool(const InputBlockPtr& block)>& predicate)
{
  for (const auto& block : attachedBlocks) {
    if (block->isInput && predicate(std::static_pointer_cast<InputBlock>(block))) {
      return std::static_pointer_cast<InputBlock>(block);
    }
  }

  return nullptr;
}

std::vector<InputBlockPtr> NodeMaterial::getInputBlocks() const
{
  std::vector<InputBlockPtr> blocks;
  for (const auto& block : attachedBlocks) {
    if (block->isInput()) {
      blocks.emplace_back(std::static_pointer_cast<InputBlock>(block));
    }
  }

  return blocks;
}

NodeMaterial& NodeMaterial::registerOptimizer(const NodeMaterialOptimizerPtr& optimizer)
{
  auto index = stl_util::index_of(_optimizers, optimizer);

  if (index > -1) {
    return *this;
  }

  _optimizers.emplace_back(optimizer);

  return *this;
}

NodeMaterial& NodeMaterial::unregisterOptimizer(const NodeMaterialOptimizerPtr& optimizer)
{
  auto index = stl_util::index_of(_optimizers, optimizer);

  if (index == -1) {
    return *this;
  }

  stl_util::splice(_optimizers, index, 1);

  return *this;
}

NodeMaterial& NodeMaterial::addOutputNode(const NodeMaterialBlockPtr& node)
{
  if (node->target() == NodeMaterialBlockTargets::Undefined) {
    throw std::runtime_error(
      "This node is not meant to be an output node. You may want to explicitly set its target "
      "value.");
  }

  if ((node->target() == NodeMaterialBlockTargets::Vertex) != 0) {
    _addVertexOutputNode(node);
  }

  if ((node->target() == NodeMaterialBlockTargets::Fragment) != 0) {
    _addFragmentOutputNode(node);
  }

  return *this;
}

NodeMaterial& NodeMaterial::removeOutputNode(const NodeMaterialBlockPtr& node)
{
  if (node->target() == NodeMaterialBlockTargets::Undefined) {
    return *this;
  }

  if (node->target() == NodeMaterialBlockTargets::Vertex) {
    _removeVertexOutputNode(node);
  }

  if (node->target() == NodeMaterialBlockTargets::Fragment) {
    _removeFragmentOutputNode(node);
  }

  return *this;
}

NodeMaterial& NodeMaterial::_addVertexOutputNode(const NodeMaterialBlockPtr& node)
{
  if (stl_util::contains(_vertexOutputNodes, node)) {
    return *this;
  }

  node->target = NodeMaterialBlockTargets::Vertex;
  _vertexOutputNodes.emplace_back(node);

  return *this;
}

NodeMaterial& NodeMaterial::_removeVertexOutputNode(const NodeMaterialBlockPtr& node)
{
  auto index = stl_util::index_of(_vertexOutputNodes, node);
  if (index == -1) {
    return *this;
  }

  stl_util::splice(_vertexOutputNodes, index, 1);

  return *this;
}

NodeMaterial& NodeMaterial::_addFragmentOutputNode(const NodeMaterialBlockPtr& node)
{
  if (stl_util::contains(_fragmentOutputNodes, node)) {
    return *this;
  }

  node->target = NodeMaterialBlockTargets::Fragment;
  _fragmentOutputNodes.emplace_back(node);

  return *this;
}

NodeMaterial& NodeMaterial::_removeFragmentOutputNode(const NodeMaterialBlockPtr& node)
{
  auto index = stl_util::index_of(_fragmentOutputNodes, node);
  if (index == -1) {
    return *this;
  }

  stl_util::splice(_fragmentOutputNodes, index, 1);

  return *this;
}

bool NodeMaterial::needAlphaBlending() const
{
  if (ignoreAlpha) {
    return false;
  }
  return (alpha < 1.f) || (_sharedData && _sharedData->hints.needAlphaBlending);
}

bool NodeMaterial::needAlphaTesting() const
{
  return _sharedData && _sharedData->hints.needAlphaTesting;
}

void NodeMaterial::_initializeBlock(
  const NodeMaterialBlockPtr& node, const NodeMaterialBuildStatePtr& iState,
  std::vector<NodeMaterialBlockPtr>& nodesToProcessForOtherBuildState)
{
  node->initialize(*iState);
  node->autoConfigure(shared_from_this());
  node->_preparationId = _buildId;

  if (!stl_util::contains(attachedBlocks, node)) {
    if (node->isUnique()) {
      const auto className = node->getClassName();

      for (const auto& other : attachedBlocks) {
        if (other->getClassName() == className) {
          throw std::runtime_error(StringTools::printf(
            "Cannot have multiple blocks of type %s in the same NodeMaterial", className.c_str()));
        }
      }
    }
    attachedBlocks.emplace_back(node);
  }

  for (const auto& input : node->inputs()) {
    input->associatedVariableName = "";

    const auto& connectedPoint = input->connectedPoint();
    if (connectedPoint) {
      const auto block = connectedPoint->ownerBlock();
      if (block != node) {
        if (block->target() == NodeMaterialBlockTargets::VertexAndFragment) {
          nodesToProcessForOtherBuildState.emplace_back(block);
        }
        else if (iState->target == NodeMaterialBlockTargets::Fragment
                 && block->target() == NodeMaterialBlockTargets::Vertex
                 && block->_preparationId != _buildId) {
          nodesToProcessForOtherBuildState.emplace_back(block);
        }
        _initializeBlock(block, iState, nodesToProcessForOtherBuildState);
      }
    }
  }

  for (const auto& output : node->outputs()) {
    output->associatedVariableName = "";
  }
}

void NodeMaterial::_resetDualBlocks(const NodeMaterialBlockPtr& node, size_t iId)
{
  if (node->target() == NodeMaterialBlockTargets::VertexAndFragment) {
    node->buildId = iId;
  }

  for (const auto& inputs : node->inputs()) {
    const auto connectedPoint = inputs->connectedPoint();
    if (connectedPoint) {
      const auto block = connectedPoint->ownerBlock();
      if (block != node) {
        _resetDualBlocks(block, iId);
      }
    }
  }
}

void NodeMaterial::removeBlock(const NodeMaterialBlockPtr& block)
{
  auto attachedBlockIndex = stl_util::index_of(attachedBlocks, block);
  if (attachedBlockIndex > -1) {
    stl_util::splice(attachedBlocks, attachedBlockIndex, 1);
  }

  if (block->isFinalMerger()) {
    removeOutputNode(block);
  }
}

void NodeMaterial::build(bool verbose)
{
  _buildWasSuccessful = false;
  auto engine         = getScene()->getEngine();

  const auto allowEmptyVertexProgram = _mode == NodeMaterialModes::Particle;

  if (_vertexOutputNodes.empty() && !allowEmptyVertexProgram) {
    throw std::runtime_error("You must define at least one vertexOutputNode");
  }

  if (_fragmentOutputNodes.empty()) {
    throw std::runtime_error("You must define at least one fragmentOutputNode");
  }

  // Compilation state
  _vertexCompilationState                          = std::make_shared<NodeMaterialBuildState>();
  _vertexCompilationState->supportUniformBuffers   = engine->supportsUniformBuffers();
  _vertexCompilationState->target                  = NodeMaterialBlockTargets::Vertex;
  _fragmentCompilationState                        = std::make_shared<NodeMaterialBuildState>();
  _fragmentCompilationState->supportUniformBuffers = engine->supportsUniformBuffers();
  _fragmentCompilationState->target                = NodeMaterialBlockTargets::Fragment;

  // Shared data
  _sharedData                           = std::make_shared<NodeMaterialBuildStateSharedData>();
  _vertexCompilationState->sharedData   = _sharedData;
  _fragmentCompilationState->sharedData = _sharedData;
  _sharedData->buildId                  = _buildId;
  _sharedData->emitComments             = _options->emitComments;
  _sharedData->verbose                  = verbose;
  _sharedData->scene                    = getScene();
  _sharedData->allowEmptyVertexProgram  = allowEmptyVertexProgram;

  // Initialize blocks
  std::vector<NodeMaterialBlockPtr> vertexNodes;
  std::vector<NodeMaterialBlockPtr> fragmentNodes;

  for (const auto& vertexOutputNode : _vertexOutputNodes) {
    vertexNodes.emplace_back(vertexOutputNode);
    _initializeBlock(vertexOutputNode, _vertexCompilationState, fragmentNodes);
  }

  for (const auto& fragmentOutputNode : _fragmentOutputNodes) {
    fragmentNodes.emplace_back(fragmentOutputNode);
    _initializeBlock(fragmentOutputNode, _fragmentCompilationState, vertexNodes);
  }

  // Optimize
  optimize();

  // Vertex
  for (const auto& vertexOutputNode : vertexNodes) {
    vertexOutputNode->build(*_vertexCompilationState, vertexNodes);
  }

  // Fragment
  _fragmentCompilationState->uniforms             = _vertexCompilationState->uniforms;
  _fragmentCompilationState->_uniformDeclaration  = _vertexCompilationState->_uniformDeclaration;
  _fragmentCompilationState->_constantDeclaration = _vertexCompilationState->_constantDeclaration;
  _fragmentCompilationState->_vertexState         = _vertexCompilationState;

  for (const auto& fragmentOutputNode : fragmentNodes) {
    _resetDualBlocks(fragmentOutputNode, _buildId - 1);
  }

  for (const auto& fragmentOutputNode : fragmentNodes) {
    fragmentOutputNode->build(*_fragmentCompilationState, fragmentNodes);
  }

  // Finalize
  _vertexCompilationState->finalize(*_vertexCompilationState);
  _fragmentCompilationState->finalize(*_fragmentCompilationState);

  _buildId = NodeMaterial::_BuildIdGenerator++;

  // Errors
  _sharedData->emitErrors();

  if (verbose) {
    BABYLON_LOG_INFO("NodeMaterial", "Vertex shader:")
    BABYLON_LOG_INFO("NodeMaterial", _vertexCompilationState->compilationString)
    BABYLON_LOG_INFO("NodeMaterial", "Fragment shader:")
    BABYLON_LOG_INFO("NodeMaterial", _fragmentCompilationState->compilationString)
  }

  _buildWasSuccessful = true;
  onBuildObservable.notifyObservers(this);

  // Wipe defines
  const auto& meshes = getScene()->meshes;
  for (const auto& mesh : meshes) {
    if (mesh->subMeshes.empty()) {
      continue;
    }
    for (const auto& subMesh : mesh->subMeshes) {
      if (!subMesh->getMaterial() || subMesh->getMaterial().get() != this) {
        continue;
      }

      if (!subMesh->_materialDefines) {
        continue;
      }

      auto& defines = subMesh->_materialDefines;
      defines->markAllAsDirty();
      defines.reset();
    }
  }
}

void NodeMaterial::optimize()
{
  for (const auto& optimizer : _optimizers) {
    optimizer->optimize(_vertexOutputNodes, _fragmentOutputNodes);
  }
}

void NodeMaterial::_prepareDefinesForAttributes(AbstractMesh* mesh, NodeMaterialDefines& defines)
{
  const auto oldNormal  = defines["NORMAL"];
  const auto oldTangent = defines["TANGENT"];
  const auto oldUV1     = defines["UV1"];

  defines.boolDef["NORMAL"] = mesh->isVerticesDataPresent(VertexBuffer::NormalKind);

  defines.boolDef["TANGENT"] = mesh->isVerticesDataPresent(VertexBuffer::TangentKind);

  defines.boolDef["UV1"] = mesh->isVerticesDataPresent(VertexBuffer::UVKind);

  if (oldNormal != defines["NORMAL"] || oldTangent != defines["TANGENT"]
      || oldUV1 != defines["UV1"]) {
    defines.markAsAttributesDirty();
  }
}

PostProcessPtr
NodeMaterial::createPostProcess(const CameraPtr& camera,
                                const std::variant<float, PostProcessOptions>& iOptions,
                                unsigned int samplingMode, Engine* engine, bool reusable,
                                unsigned int textureType, unsigned int textureFormat)
{
  if (mode() != NodeMaterialModes::PostProcess) {
    BABYLON_LOG_WARN("NodeMaterial", "Incompatible material mode");
    return nullptr;
  }
  return _createEffectForPostProcess(nullptr, camera, iOptions, samplingMode, engine, reusable,
                                     textureType, textureFormat);
}

void NodeMaterial::createEffectForPostProcess(const PostProcessPtr& postProcess)
{
  _createEffectForPostProcess(postProcess);
}

PostProcessPtr
NodeMaterial::_createEffectForPostProcess(PostProcessPtr postProcess, const CameraPtr& camera,
                                          const std::variant<float, PostProcessOptions>& iOptions,
                                          unsigned int samplingMode, Engine* engine, bool reusable,
                                          unsigned int textureType, unsigned int textureFormat)
{
  auto tempName = name + std::to_string(_buildId);

  NodeMaterialDefines defines;

  const auto dummyMesh = AbstractMesh::New(tempName + "PostProcess", getScene());

  auto buildId = _buildId;

  _processDefines(dummyMesh.get(), defines);

  Effect::RegisterShader(tempName, _fragmentCompilationState->_builtCompilationString,
                         _vertexCompilationState->_builtCompilationString);

  std::unordered_map<std::string, unsigned int> indexParameters{
    {"maxSimultaneousLights", maxSimultaneousLights}};
  if (!postProcess) {
    postProcess = PostProcess::New(
      name + "PostProcess", tempName, _fragmentCompilationState->uniforms,
      _fragmentCompilationState->samplers, iOptions, camera, samplingMode, engine, reusable,
      defines.toString(), textureType, tempName, indexParameters, false, textureFormat);
  }
  else {
    postProcess->updateEffect(defines.toString(), _fragmentCompilationState->uniforms,
                              _fragmentCompilationState->samplers, indexParameters, nullptr,
                              nullptr, tempName, tempName);
  }

  postProcess->nodeMaterialSource = shared_from_this();

  postProcess->onApplyObservable.add(
    [this, &buildId, &tempName, &defines, &dummyMesh](Effect* effect, EventState& /*es*/) -> void {
      if (buildId != _buildId) {
        Effect::ShadersStore().erase(tempName + "VertexShader");
        Effect::ShadersStore().erase(tempName + "PixelShader");

        tempName = name + std::to_string(_buildId);

        defines.markAsUnprocessed();

        buildId = _buildId;
      }

      const auto result = _processDefines(dummyMesh.get(), defines);

      if (result) {
        Effect::RegisterShader(tempName, _fragmentCompilationState->_builtCompilationString,
                               _vertexCompilationState->_builtCompilationString);

#if 0
  TimingTools.SetImmediate(() =>
      postProcess!.updateEffect(defines.toString(), _fragmentCompilationState.uniforms,
                                _fragmentCompilationState.samplers,
                                { maxSimultaneousLights: maxSimultaneousLights }, undefined,
                                undefined, tempName, tempName)
  );
#endif
      }

      _checkInternals(effect);
    });

  return postProcess;
}

ProceduralTexturePtr
NodeMaterial::createProceduralTexture(const std::variant<int, RenderTargetSize>& /*size*/,
                                      Scene* /*scene*/)
{
  // TODO implement
  return nullptr;
}

void NodeMaterial::_createEffectForParticles(
  const IParticleSystemPtr& particleSystem, unsigned int blendMode,
  const std::function<void(Effect* effect)>& iOnCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& iOnError, EffectPtr effect,
  NodeMaterialDefines* defines, AbstractMeshPtr dummyMesh,
  const std::string& particleSystemDefinesJoined_)
{
  auto tempName = StringTools::printf("%s%ull_%u", name.c_str(), _buildId, blendMode);

  if (!defines) {
    defines = new NodeMaterialDefines();
  }

  if (!dummyMesh) {
    dummyMesh = getScene()->getMeshByName(name + "Particle");
    if (!dummyMesh) {
      dummyMesh = AbstractMesh::New(name + "Particle", getScene());
    }
  }

  auto buildId = _buildId;

  std::vector<std::string> particleSystemDefines;
  auto particleSystemDefinesJoined = particleSystemDefinesJoined_;

  if (!effect) {
    const auto result = _processDefines(dummyMesh.get(), *defines);

    Effect::RegisterShader(tempName, _fragmentCompilationState->_builtCompilationString);

    particleSystem->fillDefines(particleSystemDefines, blendMode);

    particleSystemDefinesJoined = StringTools::join(particleSystemDefines, "\n");

    effect = getScene()->getEngine()->createEffectForParticles(
      tempName, _fragmentCompilationState->uniforms, _fragmentCompilationState->samplers,
      defines->toString() + "\n" + particleSystemDefinesJoined,
      result && result->fallbacks ? result->fallbacks.get() : nullptr, iOnCompiled, iOnError,
      particleSystem);

    particleSystem->setCustomEffect(effect, blendMode);
  }

  effect->onBindObservable().add([this, &buildId, &tempName, &blendMode, &defines,
                                  &particleSystemDefines, &particleSystem,
                                  &particleSystemDefinesJoined, &dummyMesh, iOnCompiled, iOnError,
                                  &effect](Effect* /*effect*/, EventState& /*es*/) -> void {
    if (buildId != _buildId) {
      Effect::ShadersStore().erase(tempName + "PixelShader");

      tempName = StringTools::printf("%s%ull_%u", name.c_str(), _buildId, blendMode);

      defines->markAsUnprocessed();

      buildId = _buildId;
    }

    particleSystemDefines.clear();

    particleSystem->fillDefines(particleSystemDefines, blendMode);

    const auto particleSystemDefinesJoinedCurrent = StringTools::join(particleSystemDefines, "\n");

    if (particleSystemDefinesJoinedCurrent != particleSystemDefinesJoined) {
      defines->markAsUnprocessed();
      particleSystemDefinesJoined = particleSystemDefinesJoinedCurrent;
    }

    const auto result = _processDefines(dummyMesh.get(), *defines);

    if (result) {
      Effect::RegisterShader(tempName, _fragmentCompilationState->_builtCompilationString);

      effect = getScene()->getEngine()->createEffectForParticles(
        tempName, _fragmentCompilationState->uniforms, _fragmentCompilationState->samplers,
        defines->toString() + "\n" + particleSystemDefinesJoined, result->fallbacks.get(),
        iOnCompiled, iOnError, particleSystem);
      particleSystem->setCustomEffect(effect, blendMode);
      _createEffectForParticles(
        particleSystem, blendMode, iOnCompiled, iOnError, effect, defines, dummyMesh,
        particleSystemDefinesJoined); // add the effect.onBindObservable observer
      return;
    }

    _checkInternals(effect.get());
  });
}

void NodeMaterial::_checkInternals(Effect* effect)
{
  // Animated blocks
  if (!_sharedData->animatedInputs.empty()) {
    const auto scene = getScene();

    const auto frameId = scene->getFrameId();

    if (_animationFrame != frameId) {
      for (const auto& input : _sharedData->animatedInputs) {
        input->animate(scene);
      }

      _animationFrame = frameId;
    }
  }

  // Bindable blocks
  for (const auto& block : _sharedData->bindableBlocks) {
    block->bind(effect, shared_from_this());
  }

  // Connection points
  for (const auto& inputBlock : _sharedData->inputBlocks) {
    inputBlock->_transmit(effect, getScene());
  }
}

void NodeMaterial::createEffectForParticles(
  const IParticleSystemPtr& particleSystem, const std::function<void(Effect* effect)>& iOnCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& iOnError)
{
  if (mode() != NodeMaterialModes::Particle) {
    BABYLON_LOG_WARN("NodeMaterial", "Incompatible material mode");
    return;
  }

  _createEffectForParticles(particleSystem, BaseParticleSystem::BLENDMODE_ONEONE, iOnCompiled,
                            iOnError);
  _createEffectForParticles(particleSystem, BaseParticleSystem::BLENDMODE_MULTIPLY, iOnCompiled,
                            iOnError);
}

std::optional<_ProcessedDefinesResult> NodeMaterial::_processDefines(AbstractMesh* mesh,
                                                                     NodeMaterialDefines& defines,
                                                                     bool useInstances,
                                                                     SubMesh* subMesh)
{
  std::optional<_ProcessedDefinesResult> result = std::nullopt;

  // Shared defines
  for (const auto& b : _sharedData->blocksWithDefines) {
    b->initializeDefines(mesh, shared_from_this(), defines, useInstances);
  }

  for (const auto& b : _sharedData->blocksWithDefines) {
    b->prepareDefines(mesh, shared_from_this(), defines, useInstances, subMesh);
  }

  // Need to recompile?
  if (defines.isDirty()) {
    const auto lightDisposed = defines._areLightsDisposed;
    defines.markAsProcessed();

    // Repeatable content generators
    _vertexCompilationState->compilationString = _vertexCompilationState->_builtCompilationString;
    _fragmentCompilationState->compilationString
      = _fragmentCompilationState->_builtCompilationString;

    for (const auto& b : _sharedData->repeatableContentBlocks) {
      b->replaceRepeatableContent(*_vertexCompilationState, *_fragmentCompilationState, mesh,
                                  defines);
    }

    // Uniforms
    std::vector<std::string> uniformBuffers;
    for (const auto& b : _sharedData->dynamicUniformBlocks) {
      b->updateUniformsAndSamples(*_vertexCompilationState, shared_from_this(), defines,
                                  uniformBuffers);
    }

    auto& mergedUniforms = _vertexCompilationState->uniforms;

    for (const auto& u : _fragmentCompilationState->uniforms) {
      const auto index = stl_util::index_of(mergedUniforms, u);

      if (index == -1) {
        mergedUniforms.emplace_back(u);
      }
    }

    // Samplers
    auto& mergedSamplers = _vertexCompilationState->samplers;

    for (const auto& s : _fragmentCompilationState->samplers) {
      const auto index = stl_util::index_of(mergedSamplers, s);

      if (index == -1) {
        mergedSamplers.emplace_back(s);
      }
    };

    auto fallbacks = std::make_unique<EffectFallbacks>();

    for (const auto& b : _sharedData->blocksWithFallbacks) {
      b->provideFallbacks(mesh, fallbacks.get());
    }

    result = {
      lightDisposed,        // lightDisposed
      uniformBuffers,       // uniformBuffers
      mergedUniforms,       // mergedUniforms
      mergedSamplers,       // mergedSamplers
      std::move(fallbacks), // fallbacks
    };
  }

  return result;
}

bool NodeMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances)
{
  if (!_buildWasSuccessful) {
    return false;
  }

  auto scene = getScene();
  if (!_sharedData->animatedInputs.empty()) {
    const auto& frameId = scene->getFrameId();

    if (_animationFrame != frameId) {
      for (const auto& input : _sharedData->animatedInputs) {
        input->animate(scene);
      }

      _animationFrame = frameId;
    }
  }

  if (subMesh->effect() && isFrozen()) {
    if (subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<NodeMaterialDefines>();
  }

  auto defines = std::static_pointer_cast<NodeMaterialDefines>(subMesh->_materialDefines);
  if (_isReadyForSubMesh(subMesh)) {
    return true;
  }

  auto engine = scene->getEngine();

  _prepareDefinesForAttributes(mesh, *defines);

  // Check if blocks are ready
  for (const auto& b : _sharedData->blockingBlocks) {
    if (!b->isReady(mesh, shared_from_this(), *defines, useInstances)) {
      return false;
    }
  }

  auto result = _processDefines(mesh, *defines, useInstances, subMesh);

  if (result) {
    auto previousEffect = subMesh->effect();
    // Compilation
    auto join = defines->toString();

    std::unordered_map<std::string, std::string> baseName{
      {"vertex", "nodeMaterial" + std::to_string(_buildId)},            //
      {"fragment", "nodeMaterial" + std::to_string(_buildId)},          //
      {"vertexSource", _vertexCompilationState->compilationString},     //
      {"fragmentSource", _fragmentCompilationState->compilationString}, //
    };

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights},
      {"maxSimultaneousMorphTargets", defines->intDef["NUM_MORPH_INFLUENCERS"]}};

    IEffectCreationOptions iOptions;
    iOptions.attributes            = _vertexCompilationState->attributes;
    iOptions.uniformsNames         = std::move(result->mergedUniforms);
    iOptions.uniformBuffersNames   = std::move(result->uniformBuffers);
    iOptions.samplers              = std::move(result->mergedSamplers);
    iOptions.materialDefines       = defines.get();
    iOptions.defines               = std::move(join);
    iOptions.fallbacks             = std::move(result->fallbacks);
    iOptions.onCompiled            = onCompiled;
    iOptions.onError               = onError;
    iOptions.indexParameters       = std::move(indexParameters);
    iOptions.maxSimultaneousLights = maxSimultaneousLights;

    auto effect = scene->getEngine()->createEffect(baseName, iOptions, engine);

    if (effect) {
      /* if (_onEffectCreatedObservable()) */ {
        onCreatedEffectParameters.effect  = effect.get();
        onCreatedEffectParameters.subMesh = subMesh;
        _onEffectCreatedObservable.notifyObservers(&onCreatedEffectParameters);
      }

      // Use previous effect while new one is compiling
      if (allowShaderHotSwapping && previousEffect && !effect->isReady()) {
        effect = previousEffect;
        defines->markAsUnprocessed();

        if (result->lightDisposed) {
          // re register in case it takes more than one frame.
          defines->_areLightsDisposed = true;
          return false;
        }
      }
      else {
        scene->resetCachedMaterial();
        subMesh->setEffect(effect, defines);
      }
    }
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines->_renderId                     = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

std::string NodeMaterial::compiledShaders() const
{
  return StringTools::printf("// Vertex shader\r\n%s\r\n\r\n// Fragment shader\r\n%s",
                             _vertexCompilationState->compilationString.c_str(),
                             _fragmentCompilationState->compilationString.c_str());
}

void NodeMaterial::bindOnlyWorldMatrix(Matrix& world, const EffectPtr& /*effectOverride*/)
{
  auto scene = getScene();

  if (!_activeEffect) {
    return;
  }

  const auto& hints = _sharedData->hints;

  if (hints.needWorldViewMatrix) {
    world.multiplyToRef(scene->getViewMatrix(), _cachedWorldViewMatrix);
  }

  if (hints.needWorldViewProjectionMatrix) {
    world.multiplyToRef(scene->getTransformMatrix(), _cachedWorldViewProjectionMatrix);
  }

  // Connection points
  for (const auto& inputBlock : _sharedData->inputBlocks) {
    inputBlock->_transmitWorld(_activeEffect.get(), world, _cachedWorldViewMatrix,
                               _cachedWorldViewProjectionMatrix);
  }
}

void NodeMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene  = getScene();
  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(world);

  auto mustRebind = _mustRebind(scene, effect, mesh->visibility());

  if (mustRebind) {
    const auto& sharedData = _sharedData;
    if (effect && scene->getCachedEffect() != effect) {
      // Bindable blocks
      for (const auto& block : sharedData->bindableBlocks) {
        block->bind(effect.get(), shared_from_this(), mesh, subMesh);
      }

      // Connection points
      for (const auto& inputBlock : sharedData->inputBlocks) {
        inputBlock->_transmit(effect.get(), scene);
      }
    }
  }

  _afterBind(mesh, _activeEffect);
}

std::vector<BaseTexturePtr> NodeMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  if (_sharedData) {
    for (const auto& t : _sharedData->textureBlocks) {
      if (std::holds_alternative<TextureBlockPtr>(t) && std::get<TextureBlockPtr>(t)) {
        activeTextures.emplace_back(std::get<TextureBlockPtr>(t)->texture);
      }
      else if (std::holds_alternative<ReflectionTextureBlockPtr>(t)
               && std::get<ReflectionTextureBlockPtr>(t)) {
        activeTextures.emplace_back(std::get<ReflectionTextureBlockPtr>(t)->texture);
      }
    }
  }

  return activeTextures;
}

std::vector<NodeMaterial::TextureInputBlockType> NodeMaterial::getTextureBlocks()
{
  if (!_sharedData) {
    return {};
  }

  return _sharedData->textureBlocks;
}

bool NodeMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (!_sharedData) {
    return false;
  }

  for (const auto& t : _sharedData->textureBlocks) {
    if (std::holds_alternative<TextureBlockPtr>(t)
        && std::get<TextureBlockPtr>(t)->texture == std::static_pointer_cast<Texture>(texture)) {
      return true;
    }
    else if (std::holds_alternative<ReflectionTextureBlockPtr>(t)
             && std::get<ReflectionTextureBlockPtr>(t)->texture == texture) {
      return true;
    }
  }

  return false;
}

void NodeMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures, bool notBoundToMesh)
{
  if (forceDisposeTextures) {
    for (const auto& tb : _sharedData->textureBlocks) {
      if (std::holds_alternative<TextureBlockPtr>(tb)) {
        auto texture = std::get<TextureBlockPtr>(tb)->texture;
        if (texture) {
          texture->dispose();
        }
      }
      else if (std::holds_alternative<ReflectionTextureBlockPtr>(tb)) {
        auto texture = std::get<ReflectionTextureBlockPtr>(tb)->texture;
        if (texture) {
          texture->dispose();
        }
      }
    }
  }

  for (const auto& block : attachedBlocks) {
    block->dispose();
  }

  onBuildObservable.clear();

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures, notBoundToMesh);
}

void NodeMaterial::edit(const INodeMaterialEditorOptionsPtr& /*config*/)
{
}

void NodeMaterial::clear()
{
  _vertexOutputNodes.clear();
  _fragmentOutputNodes.clear();
  attachedBlocks.clear();
}

void NodeMaterial::setToDefault()
{
  clear();

  auto positionInput = InputBlock::New("Position");
  positionInput->setAsAttribute("position");

  auto worldInput = InputBlock::New("World");
  worldInput->setAsSystemValue(NodeMaterialSystemValues::World);

  auto worldPos = TransformBlock::New("WorldPos");
  positionInput->connectTo(worldPos);
  worldInput->connectTo(worldPos);

  auto viewProjectionInput = InputBlock::New("ViewProjection");
  viewProjectionInput->setAsSystemValue(NodeMaterialSystemValues::ViewProjection);

  auto worldPosdMultipliedByViewProjection
    = TransformBlock::New("WorldPos * ViewProjectionTransform");
  worldPos->connectTo(worldPosdMultipliedByViewProjection);
  viewProjectionInput->connectTo(worldPosdMultipliedByViewProjection);

  auto vertexOutput = VertexOutputBlock::New("VertexOutput");
  worldPosdMultipliedByViewProjection->connectTo(vertexOutput);

  // Pixel
  auto pixelColor   = InputBlock::New("color");
  pixelColor->value = std::make_shared<AnimationValue>(Color4(0.8f, 0.8f, 0.8f, 1.f));

  auto fragmentOutput = FragmentOutputBlock::New("FragmentOutput");
  pixelColor->connectTo(fragmentOutput);

  // Add to nodes
  addOutputNode(vertexOutput);
  addOutputNode(fragmentOutput);

  _mode = NodeMaterialModes::Material;
}

void NodeMaterial::setToDefaultPostProcess()
{
  clear();

  // editorData = nullptr;

  const auto position = InputBlock::New("Position");
  position->setAsAttribute("position2d");

  const auto const1  = InputBlock::New("Constant1");
  const1->isConstant = true;
  const1->value      = std::make_shared<AnimationValue>(1.f);

  const auto vmerger = VectorMergerBlock::New("Position3D");

  position->connectTo(vmerger);
  const1->connectTo(vmerger, NodeMaterialBlockConnectionOptions{
                               "w", // input
                             });

  const auto vertexOutput = VertexOutputBlock::New("VertexOutput");
  vmerger->connectTo(vertexOutput);

  // Pixel
  const auto scale          = InputBlock::New("Scale");
  scale->visibleInInspector = true;
  scale->value              = std::make_shared<AnimationValue>(Vector2(1.f, 1.f));

  const auto uv0 = RemapBlock::New("uv0");
  position->connectTo(uv0);

  const auto uv = MultiplyBlock::New("UV scale");
  uv0->connectTo(uv);
  scale->connectTo(uv);

  const auto currentScreen = CurrentScreenBlock::New("CurrentScreen");
  uv->connectTo(currentScreen);

  currentScreen->texture
    = Texture::New("https://assets.babylonjs.com/nme/currentScreenPostProcess.png", getScene());

  const auto fragmentOutput = FragmentOutputBlock::New("FragmentOutput");
  currentScreen->connectTo(fragmentOutput, NodeMaterialBlockConnectionOptions{
                                             "",     // input
                                             "rgba", // output
                                           });

  // Add to nodes
  addOutputNode(vertexOutput);
  addOutputNode(fragmentOutput);

  _mode = NodeMaterialModes::PostProcess;
}

void NodeMaterial::setToDefaultProceduralTexture()
{
  clear();

  // editorData = nullptr;

  const auto position = InputBlock::New("Position");
  position->setAsAttribute("position2d");

  const auto const1  = InputBlock::New("Constant1");
  const1->isConstant = true;
  const1->value      = std::make_shared<AnimationValue>(1.f);

  const auto vmerger = VectorMergerBlock::New("Position3D");

  position->connectTo(vmerger);
  NodeMaterialBlockConnectionOptions iOptions;
  iOptions.input = "w";
  const1->connectTo(vmerger, iOptions);

  const auto vertexOutput = VertexOutputBlock::New("VertexOutput");
  vmerger->connectTo(vertexOutput);

  // Pixel
  const auto time     = InputBlock::New("Time");
  time->value         = std::make_shared<AnimationValue>(0.f);
  time->min           = 0.f;
  time->max           = 0.f;
  time->isBoolean     = false;
  time->matrixMode    = 0;
  time->animationType = AnimatedInputBlockTypes::Time;
  time->isConstant    = false;

  const auto color          = InputBlock::New("Color3");
  color->value              = std::make_shared<AnimationValue>(Color3(1.f, 1.f, 1.f));
  color->isConstant         = false;
  const auto fragmentOutput = FragmentOutputBlock::New("FragmentOutput");

  const auto vectorMerger          = VectorMergerBlock::New("VectorMerger");
  vectorMerger->visibleInInspector = false;

  const auto cos = TrigonometryBlock::New("Cos");
  cos->operation = TrigonometryBlockOperations::Cos;

  position->connectTo(vectorMerger);
  time->output()->connectTo(cos->input());
  cos->output()->connectTo(vectorMerger->z());
  vectorMerger->xyzOut()->connectTo(fragmentOutput->rgb());

  // Add to nodes
  addOutputNode(vertexOutput);
  addOutputNode(fragmentOutput);

  _mode = NodeMaterialModes::ProceduralTexture;
}

void NodeMaterial::setToDefaultParticle()
{
  clear();

  // editorData = nullptr;

  // Pixel
  const auto uv = InputBlock::New("uv");
  uv->setAsAttribute("particle_uv");

  const auto texture = ParticleTextureBlock::New("ParticleTexture");
  uv->connectTo(texture);

  const auto color = InputBlock::New("Color");
  color->setAsAttribute("particle_color");

  const auto multiply = MultiplyBlock::New("Texture * Color");
  texture->connectTo(multiply);
  color->connectTo(multiply);

  const auto rampGradient = ParticleRampGradientBlock::New("ParticleRampGradient");
  multiply->connectTo(rampGradient);

  const auto cSplitter = ColorSplitterBlock::New("ColorSplitter");
  color->connectTo(cSplitter);

  const auto blendMultiply = ParticleBlendMultiplyBlock::New("ParticleBlendMultiply");
  rampGradient->connectTo(blendMultiply);
  texture->connectTo(blendMultiply, NodeMaterialBlockConnectionOptions{
                                      "",  // input
                                      "a", // output
                                    });
  cSplitter->connectTo(blendMultiply, NodeMaterialBlockConnectionOptions{
                                        "",  // input
                                        "a", // output
                                      });

  const auto fragmentOutput = FragmentOutputBlock::New("FragmentOutput");
  blendMultiply->connectTo(fragmentOutput);

  // Add to nodes
  addOutputNode(fragmentOutput);

  _mode = NodeMaterialModes::Particle;
}

void NodeMaterial::loadAsync(const std::string& url)
{
  FileTools::LoadFile(
    url,
    [this](const std::variant<std::string, ArrayBufferView>& data,
           const std::string& /*responseURL*/) -> void {
      if (std::holds_alternative<std::string>(data)) {
        auto serializationObject = json::parse(std::get<std::string>(data));

        loadFromSerialization(serializationObject, "");
      }
    },
    nullptr, false,
    [url](const std::string& message, const std::string& /*exception*/) -> void {
      BABYLON_LOG_ERROR("NodeMaterial", "Could not load file %s, reason: %s", url.c_str(),
                        message.c_str())
    });
}

void NodeMaterial::_gatherBlocks(const NodeMaterialBlockPtr& rootNode,
                                 std::vector<NodeMaterialBlockPtr>& list)
{
  if (stl_util::contains(list, rootNode)) {
    return;
  }
  list.emplace_back(rootNode);

  for (const auto& input : rootNode->inputs()) {
    const auto connectedPoint = input->connectedPoint();
    if (connectedPoint) {
      const auto block = connectedPoint->ownerBlock();
      if (block != rootNode) {
        _gatherBlocks(block, list);
      }
    }
  }
}

std::string NodeMaterial::generateCode()
{
  std::vector<NodeMaterialBlockPtr> alreadyDumped;
  std::vector<NodeMaterialBlockPtr> vertexBlocks;
  std::vector<std::string> uniqueNames;
  // Gets active blocks
  for (const auto& outputNode : _vertexOutputNodes) {
    _gatherBlocks(outputNode, vertexBlocks);
  }

  std::vector<NodeMaterialBlockPtr> fragmentBlocks;
  for (const auto& outputNode : _fragmentOutputNodes) {
    _gatherBlocks(outputNode, fragmentBlocks);
  }

  // Generate vertex shader
  std::string codeString = StringTools::printf("auto nodeMaterial = NodeMaterial::New(\"%s\");\r\n",
                                               !name.empty() ? name.c_str() : "node material");
  for (const auto& node : vertexBlocks) {
    if (node->isInput() && !stl_util::contains(alreadyDumped, node)) {
      codeString += node->_dumpCode(uniqueNames, alreadyDumped);
    }
  }

  // Generate fragment shader
  for (const auto& node : fragmentBlocks) {
    if (node->isInput() && !stl_util::contains(alreadyDumped, node)) {
      codeString += node->_dumpCode(uniqueNames, alreadyDumped);
    }
  }

  // Connections
  alreadyDumped = {};
  codeString += "\r\n// Connections\r\n";
  for (const auto& node : _vertexOutputNodes) {
    codeString += node->_dumpCodeForOutputConnections(alreadyDumped);
  }
  for (const auto& node : _fragmentOutputNodes) {
    codeString += node->_dumpCodeForOutputConnections(alreadyDumped);
  }

  // Output nodes
  codeString += "\r\n// Output nodes\r\n";
  for (const auto& node : _vertexOutputNodes) {
    codeString += StringTools::printf("nodeMaterial->addOutputNode(%s);\r\n",
                                      node->_codeVariableName.c_str());
  }

  for (const auto& node : _fragmentOutputNodes) {
    codeString += StringTools::printf("nodeMaterial->addOutputNode(%s);\r\n",
                                      node->_codeVariableName.c_str());
  }

  codeString += "nodeMaterial->build();\r\n";

  return codeString;
}

json NodeMaterial::serialize() const
{
  return nullptr;
}

void NodeMaterial::_restoreConnections(const NodeMaterialBlockPtr& block, const json& source,
                                       const std::unordered_map<size_t, NodeMaterialBlockPtr>& map)
{
  for (const auto& outputPoint : block->outputs()) {
    for (const auto& candidate : json_util::get_array<json>(source, "blocks")) {
      auto candidateId = json_util::get_number<size_t>(candidate, "id");

      if (!stl_util::contains(map, candidateId) || !map.at(candidateId)) {
        continue;
      }

      auto target = map.at(candidateId);

      for (const auto& input : json_util::get_array<json>(source, "inputs")) {
        auto inputTargetBlockId = json_util::get_number<size_t>(input, "targetBlockId");
        if (map.at(inputTargetBlockId) == block
            && json_util::get_string(input, "targetConnectionName") == outputPoint->name) {
          auto inputPoint = target->getInputByName(json_util::get_string(input, "inputName"));
          if (!inputPoint || inputPoint->isConnected()) {
            continue;
          }

          outputPoint->connectTo(inputPoint, true);
          _restoreConnections(target, source, map);
          continue;
        }
      }
    }
  }
}

void NodeMaterial::loadFromSerialization(const json& /*source*/, const std::string& /*rootUrl*/,
                                         bool /*merge*/)
{
}

MaterialPtr NodeMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

NodeMaterialPtr NodeMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

NodeMaterialPtr NodeMaterial::CreateDefault(const std::string& iName, Scene* scene)
{
  auto newMaterial = NodeMaterial::New(iName, scene);
  newMaterial->setToDefault();
  newMaterial->build();

  return newMaterial;
}

} // end of namespace BABYLON
