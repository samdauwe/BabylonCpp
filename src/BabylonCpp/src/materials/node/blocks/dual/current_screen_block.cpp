#include <babylon/materials/node/blocks/dual/current_screen_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

CurrentScreenBlock::CurrentScreenBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , texture{nullptr}
    , convertToGammaSpace{false}
    , convertToLinearSpace{false}
    , uv{this, &CurrentScreenBlock::get_uv}
    , rgba{this, &CurrentScreenBlock::get_rgba}
    , rgb{this, &CurrentScreenBlock::get_rgb}
    , r{this, &CurrentScreenBlock::get_r}
    , g{this, &CurrentScreenBlock::get_g}
    , b{this, &CurrentScreenBlock::get_b}
    , a{this, &CurrentScreenBlock::get_a}
    , _samplerName{"textureSampler"}
{
  _isUnique = false;

  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, false,
                NodeMaterialBlockTargets::VertexAndFragment);

  registerOutput("rgba", NodeMaterialBlockConnectionPointTypes::Color4,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("a", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);

  _inputs[0]->_prioritizeVertex = false;
}

CurrentScreenBlock::~CurrentScreenBlock() = default;

std::string CurrentScreenBlock::getClassName() const
{
  return "CurrentScreenBlock";
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_uv()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_rgba()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_rgb()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_r()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_g()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_b()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& CurrentScreenBlock::get_a()
{
  return _outputs[5];
}

void CurrentScreenBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("textureSampler");
}

NodeMaterialBlockTargets& CurrentScreenBlock::get_target()
{
  if (!uv()->isConnected()) {
    _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
    return _currentTarget;
  }

  if (uv()->sourceBlock()->isInput()) {
    _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
    return _currentTarget;
  }

  _currentTarget = NodeMaterialBlockTargets::Fragment;
  return _currentTarget;
}

void CurrentScreenBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                        const NodeMaterialPtr& /*nodeMaterial*/,
                                        NodeMaterialDefines& defines, bool /*useInstances*/,
                                        SubMesh* /*subMesh*/)
{
  defines.setValue(_linearDefineName, convertToGammaSpace, true);
  defines.setValue(_gammaDefineName, convertToLinearSpace, true);
}

bool CurrentScreenBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                                 const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  if (texture && !texture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void CurrentScreenBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  auto uvInput = uv();

  if (uvInput->connectedPoint()->ownerBlock()->isInput()) {
    auto uvInputOwnerBlock
      = std::static_pointer_cast<InputBlock>(uvInput->connectedPoint()->ownerBlock());

    if (!uvInputOwnerBlock->isAttribute()) {
      state._emitUniformFromString(uvInput->associatedVariableName, "vec2");
    }
  }

  _mainUVName = "vMain" + uvInput->associatedVariableName();

  state._emitVaryingFromString(_mainUVName, "vec2");

  state.compilationString += StringTools::printf("%s = %s.xy;\r\n", _mainUVName.c_str(),
                                                 uvInput->associatedVariableName().c_str());

  for (const auto& output : _outputs) {
    if (!output->isConnectedInVertexShader()) {
      return;
    }
  }

  _writeTextureRead(state, true);

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name, true);
    }
  }
}

void CurrentScreenBlock::_writeTextureRead(NodeMaterialBuildState& state, bool vertexMode)
{
  auto uvInput = uv();

  if (vertexMode) {
    if (state.target == NodeMaterialBlockTargets::Fragment) {
      return;
    }

    state.compilationString
      += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                             _samplerName.c_str(), uvInput->associatedVariableName().c_str());
    return;
  }

  if (uv()->ownerBlock()->target() == NodeMaterialBlockTargets::Fragment) {
    state.compilationString
      += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                             _samplerName.c_str(), uvInput->associatedVariableName().c_str());
    return;
  }

  state.compilationString
    += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                           _samplerName.c_str(), _mainUVName.c_str());
}

void CurrentScreenBlock::_writeOutput(NodeMaterialBuildState& state,
                                      const NodeMaterialConnectionPointPtr& output,
                                      const std::string& swizzle, bool vertexMode)
{
  if (vertexMode) {
    if (state.target == NodeMaterialBlockTargets::Fragment) {
      return;
    }

    state.compilationString
      += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                             _tempTextureRead.c_str(), swizzle.c_str());

    return;
  }

  if (uv()->ownerBlock()->target() == NodeMaterialBlockTargets::Fragment) {
    state.compilationString
      += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                             _tempTextureRead.c_str(), swizzle.c_str());
    return;
  }

  state.compilationString
    += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                           _tempTextureRead.c_str(), swizzle.c_str());

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _linearDefineName.c_str());
  state.compilationString
    += StringTools::printf("%s = toGammaSpace(%s);\r\n", output->associatedVariableName().c_str(),
                           output->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _gammaDefineName.c_str());
  state.compilationString
    += StringTools::printf("%s = toLinearSpace(%s);\r\n", output->associatedVariableName().c_str(),
                           output->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
}

CurrentScreenBlock& CurrentScreenBlock::_buildBlock(NodeMaterialBuildState& state)
{
  using TextureInputBlockType
    = std::variant<TextureBlockPtr, ReflectionTextureBlockPtr, RefractionBlockPtr,
                   CurrentScreenBlockPtr, ParticleTextureBlockPtr>;

  NodeMaterialBlock::_buildBlock(state);

  _tempTextureRead = state._getFreeVariableName("tempTextureRead");

  if (stl_util::index_of(state.sharedData->blockingBlocks,
                         std::static_pointer_cast<NodeMaterialBlock>(shared_from_this()))
      < 0) {
    state.sharedData->blockingBlocks.emplace_back(shared_from_this());
  }
  const auto thisAsCurrentScreenBlock
    = std::static_pointer_cast<CurrentScreenBlock>(shared_from_this());
  auto it = std::find_if(
    state.sharedData->textureBlocks.begin(), state.sharedData->textureBlocks.end(),
    [&thisAsCurrentScreenBlock](const TextureInputBlockType& textureBlock) {
      return (std::holds_alternative<CurrentScreenBlockPtr>(textureBlock)
              && std::get<CurrentScreenBlockPtr>(textureBlock) == thisAsCurrentScreenBlock);
    });
  if (it == state.sharedData->textureBlocks.end()) {
    state.sharedData->textureBlocks.emplace_back(thisAsCurrentScreenBlock);
  }
  if (stl_util::index_of(state.sharedData->blocksWithDefines,
                         std::static_pointer_cast<NodeMaterialBlock>(shared_from_this()))
      < 0) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    state._emit2DSampler(_samplerName);
    _injectVertexCode(state);
    return *this;
  }

  // Fragment
  for (const auto& output : _outputs) {
    if (!output->isConnectedInFragmentShader()) {
      return *this;
    }
  }

  state._emit2DSampler(_samplerName);

  _linearDefineName = state._getFreeDefineName("ISLINEAR");
  _gammaDefineName  = state._getFreeDefineName("ISGAMMA");

  const auto iComments = StringTools::printf("//%s", name().c_str());
  state._emitFunctionFromInclude("helperFunctions", iComments);

  _writeTextureRead(state);

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name);
    }
  }

  return *this;
}

json CurrentScreenBlock::serialize() const
{
  return nullptr;
}

void CurrentScreenBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
