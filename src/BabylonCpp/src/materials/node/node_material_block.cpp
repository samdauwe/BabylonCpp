#include <babylon/materials/node/node_material_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/unique_id_generator.h>

namespace BABYLON {

NodeMaterialBlock::NodeMaterialBlock(const std::string& iName, NodeMaterialBlockTargets target,
                                     bool isFinalMerger, bool isInput)
    : inputsAreExclusive{false}
    , isUnique{this, &NodeMaterialBlock::get_isUnique}
    , isFinalMerger{this, &NodeMaterialBlock::get_isFinalMerger}
    , isInput{this, &NodeMaterialBlock::get_isInput}
    , buildId{this, &NodeMaterialBlock::get_buildId, &NodeMaterialBlock::set_buildId}
    , target{this, &NodeMaterialBlock::get_target, &NodeMaterialBlock::set_target}
    , inputs{this, &NodeMaterialBlock::get_inputs}
    , outputs{this, &NodeMaterialBlock::get_outputs}
    , _isUnique{false}
    , _target{NodeMaterialBlockTargets::Undefined}
{
  name = iName;

  _target = target;

  _isFinalMerger = isFinalMerger;
  _isInput       = isInput;
  uniqueId       = UniqueIdGenerator::UniqueId();
}

NodeMaterialBlock::~NodeMaterialBlock() = default;

bool NodeMaterialBlock::get_isUnique() const
{
  return _isUnique;
}

bool NodeMaterialBlock::get_isFinalMerger() const
{
  return _isFinalMerger;
}

bool NodeMaterialBlock::get_isInput() const
{
  return _isInput;
}

size_t NodeMaterialBlock::get_buildId() const
{
  return _buildId;
}

void NodeMaterialBlock::set_buildId(size_t value)
{
  _buildId = value;
}

NodeMaterialBlockTargets& NodeMaterialBlock::get_target()
{
  return _target;
}

void NodeMaterialBlock::set_target(const NodeMaterialBlockTargets& value)
{
  if (_target != value) {
    return;
  }
  _target = value;
}

std::vector<NodeMaterialConnectionPointPtr>& NodeMaterialBlock::get_inputs()
{
  return _inputs;
}

std::vector<NodeMaterialConnectionPointPtr>& NodeMaterialBlock::get_outputs()
{
  return _outputs;
}

NodeMaterialConnectionPointPtr NodeMaterialBlock::getInputByName(const std::string& iName) const
{
  for (const auto& input : _inputs) {
    if (input->name == iName) {
      return input;
    }
  }

  return nullptr;
}

NodeMaterialConnectionPointPtr NodeMaterialBlock::getOutputByName(const std::string& iName) const
{
  for (const auto& output : _outputs) {
    if (output->name == iName) {
      return output;
    }
  }

  return nullptr;
}

void NodeMaterialBlock::initialize(NodeMaterialBuildState& /*state*/)
{
  // Do nothing
}

void NodeMaterialBlock::bind(const EffectPtr& /*effect*/, const NodeMaterialPtr& /*nodeMaterial*/,
                             Mesh* /*mesh*/, SubMesh* /*subMesh*/)
{
  // Do nothing
}

std::string NodeMaterialBlock::_declareOutput(const NodeMaterialConnectionPointPtr& output,
                                              const NodeMaterialBuildState& state) const
{
  return StringTools::printf("%s %s", state._getGLType(output->type()).c_str(),
                             output->associatedVariableName().c_str());
}

std::string
NodeMaterialBlock::_writeVariable(const NodeMaterialConnectionPointPtr& currentPoint) const
{
  auto connectionPoint = currentPoint->connectedPoint();

  if (connectionPoint) {
    return currentPoint->associatedVariableName();
  }

  return "0.";
}

std::string NodeMaterialBlock::_writeFloat(float value) const
{
  auto stringVersion = std::to_string(value);

  if (!StringTools::contains(stringVersion, ".")) {
    stringVersion += ".0";
  }
  return stringVersion;
}

std::string NodeMaterialBlock::getClassName() const
{
  return "NodeMaterialBlock";
}

NodeMaterialBlock&
NodeMaterialBlock::registerInput(const std::string& iName,
                                 const NodeMaterialBlockConnectionPointTypes& type, bool isOptional,
                                 const std::optional<NodeMaterialBlockTargets>& iTarget,
                                 const NodeMaterialConnectionPointPtr& point)
{
  auto iPoint = point ? point :
                        NodeMaterialConnectionPoint::New(
                          iName, shared_from_this(), NodeMaterialConnectionPointDirection::Input);
  iPoint->type       = type;
  iPoint->isOptional = isOptional;
  if (iTarget.has_value()) {
    iPoint->target = *iTarget;
  }

  _inputs.emplace_back(iPoint);

  return *this;
}

NodeMaterialBlock&
NodeMaterialBlock::registerOutput(const std::string& iName,
                                  const NodeMaterialBlockConnectionPointTypes& type,
                                  const std::optional<NodeMaterialBlockTargets>& iTarget,
                                  const NodeMaterialConnectionPointPtr& point)
{
  auto iPoint = point ? point :
                        NodeMaterialConnectionPoint::New(
                          iName, shared_from_this(), NodeMaterialConnectionPointDirection::Output);
  iPoint->type = type;
  if (iTarget.has_value()) {
    iPoint->target = *iTarget;
  }

  _outputs.emplace_back(iPoint);

  return *this;
}

NodeMaterialConnectionPointPtr
NodeMaterialBlock::getFirstAvailableInput(const NodeMaterialConnectionPointPtr& forOutput)
{
  for (const auto& input : _inputs) {
    if (!input->connectedPoint()) {
      if (!forOutput || (forOutput->type() == input->type())
          || (input->type() == NodeMaterialBlockConnectionPointTypes::AutoDetect)) {
        return input;
      }
    }
  }

  return nullptr;
}

NodeMaterialConnectionPointPtr
NodeMaterialBlock::getFirstAvailableOutput(const NodeMaterialBlockPtr& forBlock)
{
  for (const auto& output : _outputs) {
    if (!forBlock || (forBlock->target() == NodeMaterialBlockTargets::Undefined)
        || forBlock->target() == NodeMaterialBlockTargets::Neutral
        || (forBlock->target() != output->target())) {
      return output;
    }
  }

  return nullptr;
}

NodeMaterialConnectionPointPtr
NodeMaterialBlock::getSiblingOutput(const NodeMaterialConnectionPointPtr& current)
{
  const auto index = stl_util::index_of(_outputs, current);

  if (index == -1 || index >= static_cast<int>(_outputs.size())) {
    return nullptr;
  }

  return _outputs[static_cast<size_t>(index + 1)];
}

NodeMaterialBlock&
NodeMaterialBlock::connectTo(const NodeMaterialBlockPtr& other,
                             const std::optional<NodeMaterialBlockConnectionOptions>& options)
{
  if (_outputs.empty()) {
    return *this;
  }

  auto output = options.has_value() && !options->output.empty() ? getOutputByName(options->output) :
                                                                  getFirstAvailableOutput(other);

  auto notFound = true;
  while (notFound) {
    auto input = options.has_value() && !options->input.empty() ?
                   other->getInputByName(options->input) :
                   other->getFirstAvailableInput(output);

    if (output && input && output->canConnectTo(*input)) {
      output->connectTo(input);
      notFound = false;
    }
    else if (!output) {
      throw std::runtime_error("Unable to find a compatible match");
    }
    else {
      output = getSiblingOutput(output);
    }
  }

  return *this;
}

NodeMaterialBlock& NodeMaterialBlock::_buildBlock(NodeMaterialBuildState& /*state*/)
{
  // Empty. Must be defined by child nodes
  return *this;
}

void NodeMaterialBlock::updateUniformsAndSamples(NodeMaterialBuildState& /*state*/,
                                                 const NodeMaterialPtr& /*nodeMaterial*/,
                                                 const NodeMaterialDefines& /*defines*/,
                                                 std::vector<std::string>& /*uniformBuffers*/)
{
  // Do nothing
}

void NodeMaterialBlock::provideFallbacks(AbstractMesh* /*mesh*/, EffectFallbacks* /*fallbacks*/)
{
  // Do nothing
}

void NodeMaterialBlock::initializeDefines(AbstractMesh* /*mesh*/,
                                          const NodeMaterialPtr& /*nodeMaterial*/,
                                          NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  // Do nothing
}

void NodeMaterialBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                       const NodeMaterialPtr& /*nodeMaterial*/,
                                       NodeMaterialDefines& /*defines*/, bool /*useInstances*/,
                                       const SubMeshPtr& /*subMesh*/)
{
  // Do nothing
}

void NodeMaterialBlock::autoConfigure(const NodeMaterialPtr& /*nodeMaterial*/)
{
  // Do nothing
}

void NodeMaterialBlock::replaceRepeatableContent(
  NodeMaterialBuildState& /*vertexShaderState*/,
  const NodeMaterialBuildState& /*fragmentShaderState*/, AbstractMesh* /*mesh*/,
  NodeMaterialDefines& /*defines*/)
{
  // Do nothing
}

bool NodeMaterialBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                                const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  return true;
}

void NodeMaterialBlock::_linkConnectionTypes(size_t inputIndex0, size_t inputIndex1)
{
  _inputs[inputIndex0]->_linkedConnectionSource = _inputs[inputIndex1];
  _inputs[inputIndex1]->_linkedConnectionSource = _inputs[inputIndex0];
}

void NodeMaterialBlock::_processBuild(const NodeMaterialBlockPtr& block,
                                      NodeMaterialBuildState& state,
                                      const NodeMaterialConnectionPointPtr& input,
                                      const std::vector<NodeMaterialBlockPtr>& activeBlocks)
{
  block->build(state, activeBlocks);

  const auto localBlockIsFragment = (state._vertexState != nullptr);
  const auto otherBlockWasGeneratedInVertexShader
    = block->_buildTarget == NodeMaterialBlockTargets::Vertex
      && (block->target() != NodeMaterialBlockTargets::VertexAndFragment);

  if (localBlockIsFragment
      && (((block->target() == block->_buildTarget)) || ((block->target() == input->target()))
          || (target() != NodeMaterialBlockTargets::VertexAndFragment
              && otherBlockWasGeneratedInVertexShader))) { // context switch! We need a varying
    if ((!block->isInput()
         && state.target != block->_buildTarget) // block was already emitted by vertex shader
        || (block->isInput() && std::static_pointer_cast<InputBlock>(block)->isAttribute()
            && !std::static_pointer_cast<InputBlock>(block)
                  ->_noContextSwitch()) // block is an attribute
    ) {
      auto connectedPoint = input->connectedPoint();
      if (state._vertexState->_emitVaryingFromString("v_"
                                                       + connectedPoint->associatedVariableName(),
                                                     state._getGLType(connectedPoint->type))) {
        state._vertexState->compilationString
          += StringTools::printf("v_%s = %s;\r\n", connectedPoint->associatedVariableName().c_str(),
                                 connectedPoint->associatedVariableName().c_str());
      }
      input->associatedVariableName
        = StringTools::printf("v_%s", connectedPoint->associatedVariableName().c_str());
      input->_enforceAssociatedVariableName = true;
    }
  }
}

bool NodeMaterialBlock::validateBlockName(const std::string& newName) const
{
  static const std::vector<std::string> reservedNames{
    "position",            //
    "normal",              //
    "tangent",             //
    "particle_positionw",  //
    "uv",                  //
    "uv2",                 //
    "position2d",          //
    "particle_uv",         //
    "matricesIndices",     //
    "matricesWeights",     //
    "world0",              //
    "world1",              //
    "world2",              //
    "world3",              //
    "particle_color",      //
    "particle_texturemask" //
  };
  return stl_util::contains(reservedNames, newName) ? false : true;
}

bool NodeMaterialBlock::build(NodeMaterialBuildState& state,
                              const std::vector<NodeMaterialBlockPtr>& activeBlocks)
{
  if (_buildId == state.sharedData->buildId) {
    return true;
  }

  if (!isInput()) {
    /** Prepare outputs */
    for (const auto& output : _outputs) {
      if (output->associatedVariableName().empty()) {
        output->associatedVariableName = state._getFreeVariableName(output->name);
      }
    }
  }

  // Check if "parent" blocks are compiled
  for (const auto& input : _inputs) {
    if (!input->connectedPoint()) {
      if (!input->isOptional) { // Emit a warning
        state.sharedData->checks.notConnectedNonOptionalInputs.emplace_back(input);
      }
      continue;
    }

    if (target() != NodeMaterialBlockTargets::Neutral) {
      if (input->target() == target()) {
        continue;
      }

      if (input->target() == state.target) {
        continue;
      }
    }

    const auto block = input->connectedPoint()->ownerBlock();
    if (block && block.get() != this) {
      _processBuild(block, state, input, activeBlocks);
    }
  }

  if (_buildId == state.sharedData->buildId) {
    return true; // Need to check again as inputs can be connected multiple time to this endpoint
  }

  // Logs
  if (state.sharedData->verbose) {
    const auto logStr = StringTools::printf(
      "%s: Building %s [%s]",
      state.target == NodeMaterialBlockTargets::Vertex ? "Vertex shader" : "Fragment shader",
      name.c_str(), getClassName().c_str());
    BABYLON_LOG_INFO("NodeMaterialBlock", logStr)
  }

  // Checks final outputs
  if (isFinalMerger) {
    switch (state.target) {
      case NodeMaterialBlockTargets::Vertex:
        state.sharedData->checks.emitVertex = true;
        break;
      case NodeMaterialBlockTargets::Fragment:
        state.sharedData->checks.emitFragment = true;
        break;
      default:
        break;
    }
  }

  if (!isInput && state.sharedData->emitComments) {
    state.compilationString += StringTools::printf("\r\n//%s\r\n", name.c_str());
  }

  _buildBlock(state);

  _buildId     = state.sharedData->buildId;
  _buildTarget = state.target;

  // Compile connected blocks
  for (const auto& output : _outputs) {
    if (output->target() == state.target) {
      continue;
    }

    for (const auto& endpoint : output->endpoints()) {
      const auto& block = endpoint->ownerBlock();

      if (block && (block->target() != state.target) && stl_util::contains(activeBlocks, block)) {
        _processBuild(block, state, endpoint, activeBlocks);
      }
    }
  }
  return false;
}

std::string NodeMaterialBlock::_inputRename(const std::string& iName)
{
  return iName;
}

std::string NodeMaterialBlock::_outputRename(const std::string& iName)
{
  return iName;
}

std::string NodeMaterialBlock::_dumpPropertiesCode()
{
  return "";
}

std::string NodeMaterialBlock::_dumpCode(std::vector<std::string>& uniqueNames,
                                         std::vector<NodeMaterialBlockPtr>& alreadyDumped)
{
  alreadyDumped.emplace_back(shared_from_this());

  std::string codeString;

  // Get unique name
  auto nameAsVariableName = StringTools::regexReplace(name, "[^A-Za-z_]+", "");
  _codeVariableName       = !nameAsVariableName.empty() ?
                        nameAsVariableName :
                        StringTools::printf("%s_%zu", getClassName().c_str(), uniqueId);

  if (stl_util::contains(uniqueNames, _codeVariableName)) {
    auto index = 0;
    do {
      ++index;
      _codeVariableName = nameAsVariableName + std::to_string(index);
    } while (stl_util::contains(uniqueNames, _codeVariableName));
  }

  uniqueNames.emplace_back(_codeVariableName);

  // Declaration
  codeString = StringTools::printf("\r\n// %s\r\n", getClassName().c_str());
  if (!comments.empty()) {
    codeString += StringTools::printf("// %s\r\n", comments.c_str());
  }
  codeString += StringTools::printf("auto %s = %s::New(\"%s\");\r\n", _codeVariableName.c_str(),
                                    getClassName().c_str(), name.c_str());

  // Properties
  codeString += _dumpPropertiesCode();

  // Inputs
  for (const auto& input : inputs()) {
    if (!input->isConnected()) {
      continue;
    }

    auto connectedOutput = input->connectedPoint();
    auto connectedBlock  = connectedOutput->ownerBlock();

    if (!stl_util::contains(alreadyDumped, connectedBlock)) {
      codeString += connectedBlock->_dumpCode(uniqueNames, alreadyDumped);
    }
  }

  // Outputs
  for (const auto& output : outputs()) {
    if (!output->hasEndpoints()) {
      continue;
    }

    for (const auto& endpoint : output->endpoints()) {
      const auto& connectedBlock = endpoint->ownerBlock();
      if (connectedBlock && !stl_util::contains(alreadyDumped, connectedBlock)) {
        codeString += connectedBlock->_dumpCode(uniqueNames, alreadyDumped);
      }
    }
  }

  return codeString;
}

std::string
NodeMaterialBlock::_dumpCodeForOutputConnections(std::vector<NodeMaterialBlockPtr>& alreadyDumped)
{
  std::string codeString = "";

  if (stl_util::contains(alreadyDumped, shared_from_this())) {
    return codeString;
  }

  alreadyDumped.emplace_back(shared_from_this());

  for (const auto& input : inputs()) {
    if (!input->isConnected()) {
      continue;
    }

    const auto& connectedOutput = input->connectedPoint();
    const auto& connectedBlock  = connectedOutput->ownerBlock();

    codeString += connectedBlock->_dumpCodeForOutputConnections(alreadyDumped);
    codeString += StringTools::printf("%s.%s.connectTo(%s.%s);\r\n",
                                      connectedBlock->_codeVariableName.c_str(),
                                      connectedBlock->_outputRename(connectedOutput->name).c_str(),
                                      _codeVariableName.c_str(), _inputRename(input->name).c_str());
  }

  return codeString;
}

NodeMaterialPtr NodeMaterialBlock::clone(Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

json NodeMaterialBlock::serialize() const
{
  return nullptr;
}

void NodeMaterialBlock::_deserialize(const json& serializationObject, Scene* /*scene*/,
                                     const std::string& /*rootUrl*/)
{
  name     = json_util::get_string(serializationObject, "name");
  comments = json_util::get_string(serializationObject, "comments");
}

void NodeMaterialBlock::dispose()
{
  for (const auto& input : inputs()) {
    input->dispose();
  }

  for (const auto& output : outputs()) {
    output->dispose();
  }
}

} // end of namespace BABYLON
