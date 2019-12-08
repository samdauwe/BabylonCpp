#include <babylon/materials/node/blocks/dual/texture_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

TextureBlock::TextureBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , texture{nullptr}
    , uv{this, &TextureBlock::get_uv}
    , rgba{this, &TextureBlock::get_rgba}
    , rgb{this, &TextureBlock::get_rgb}
    , r{this, &TextureBlock::get_r}
    , g{this, &TextureBlock::get_g}
    , b{this, &TextureBlock::get_b}
    , a{this, &TextureBlock::get_a}
    , _isMixed{this, &TextureBlock::get__isMixed}
    , _currentTarget{NodeMaterialBlockTargets::VertexAndFragment}
{
  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, false,
                NodeMaterialBlockTargets::VertexAndFragment);

  registerOutput("rgba", NodeMaterialBlockConnectionPointTypes::Color4,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("a", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
}

TextureBlock::~TextureBlock() = default;

std::string TextureBlock::getClassName() const
{
  return "TextureBlock";
}

NodeMaterialConnectionPointPtr& TextureBlock::get_uv()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_rgba()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_rgb()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_r()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_g()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_b()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& TextureBlock::get_a()
{
  return _outputs[5];
}

NodeMaterialBlockTargets& TextureBlock::get_target()
{
  // TextureBlock has a special optimizations for uvs that come from the vertex shaders as they can
  // be packed into a single varyings. But we need to detect uvs coming from fragment then
  if (!uv()->isConnected()) {
    _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
    return _currentTarget;
  }

  if (uv()->sourceBlock()->isInput()) {
    _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
    return _currentTarget;
  }

  auto parent = uv()->connectedPoint();

  while (parent) {
    if (parent->target == NodeMaterialBlockTargets::Fragment) {
      _currentTarget = NodeMaterialBlockTargets::Fragment;
      return _currentTarget;
    }

    if (parent->target == NodeMaterialBlockTargets::Vertex) {
      _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
      return _currentTarget;
    }

    if (parent->target == NodeMaterialBlockTargets::Neutral
        || parent->target == NodeMaterialBlockTargets::VertexAndFragment) {
      auto& parentBlock = parent->ownerBlock();

      parent = nullptr;
      for (const auto& input : parentBlock->inputs()) {
        if (input->connectedPoint()) {
          parent = input->connectedPoint;
          break;
        }
      }
    }
  }

  _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
  return _currentTarget;
}

void TextureBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!uv()->isConnected()) {
    auto uvInput = material->getInputBlockByPredicate(
      [](const InputBlockPtr& inputBlock) -> bool { return inputBlock->isAttribute() && inputBlock->name == "uv"; });

    if (!uvInput) {
      uvInput = InputBlock::New("uv");
      uvInput->setAsAttribute();
    }
    uvInput->output()->connectTo(uv);
  }
}

void TextureBlock::initializeDefines(AbstractMesh* /*mesh*/,
                                     const NodeMaterialPtr& /*nodeMaterial*/,
                                     NodeMaterialDefines& defines, bool /*useInstances*/)
{
  if (!defines._areTexturesDirty) {
    return;
  }

  defines.setValue(_mainUVDefineName, false);
}

void TextureBlock::prepareDefines(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                                  NodeMaterialDefines& defines, bool /*useInstances*/)
{
  if (!defines._areTexturesDirty) {
    return;
  }

  if (!texture || !texture->getTextureMatrix()) {
    return;
  }

  if (!texture->getTextureMatrix()->isIdentityAs3x2()) {
    defines.setValue(_defineName, true);
  }
  else {
    defines.setValue(_defineName, false);
    defines.setValue(_mainUVDefineName, true);
  }
}

bool TextureBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                           const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  if (texture && !texture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void TextureBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& /*nodeMaterial*/,
                        Mesh* mesh)
{
  if (!mesh || !texture) {
    return;
  }

  if (_isMixed) {
    effect->setFloat(_textureInfoName, texture->level);
    effect->setMatrix(_textureTransformName, *texture->getTextureMatrix());
  }
  effect->setTexture(_samplerName, texture);
}

bool TextureBlock::get__isMixed() const
{
  return target() != NodeMaterialBlockTargets::Fragment;
}

void TextureBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  const auto& uvInput = uv();

  // Inject code in vertex
  _defineName       = state._getFreeDefineName("UVTRANSFORM");
  _mainUVDefineName = state._getFreeDefineName(
    String::printf("vMain%s", uvInput->associatedVariableName().c_str()));

  if (uvInput->connectedPoint()->ownerBlock()->isInput) {
    auto uvInputOwnerBlock
      = std::static_pointer_cast<InputBlock>(uvInput->connectedPoint()->ownerBlock());

    if (!uvInputOwnerBlock->isAttribute()) {
      state._emitUniformFromString(uvInput->associatedVariableName(), "vec2");
    }
  }

  _mainUVName           = String::printf("vMain%s", uvInput->associatedVariableName().c_str());
  _transformedUVName    = state._getFreeVariableName("transformedUV");
  _textureTransformName = state._getFreeVariableName("textureTransform");
  _textureInfoName      = state._getFreeVariableName("textureInfoName");

  state._emitVaryingFromString(_transformedUVName, "vec2", _defineName);
  state._emitVaryingFromString(_mainUVName, "vec2", _mainUVDefineName);

  state._emitUniformFromString(_textureTransformName, "mat4", _defineName);

  state.compilationString += String::printf("#ifdef %s\r\n", _defineName.c_str());
  state.compilationString
    += String::printf("%s = vec2(%s * vec4(%s.xy, 1.0, 0.0));\r\n", _transformedUVName.c_str(),
                      _textureTransformName.c_str(), uvInput->associatedVariableName().c_str());
  state.compilationString += "#else\r\n";
  state.compilationString += String::printf("%s = %s.xy;\r\n", _mainUVName.c_str(),
                                            uvInput->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
}

void TextureBlock::_writeOutput(NodeMaterialBuildState& state,
                                const NodeMaterialConnectionPointPtr& output,
                                const std::string& swizzle)
{
  const auto& uvInput = uv();

  if (uv()->ownerBlock()->target() == NodeMaterialBlockTargets::Fragment) {
    state.compilationString += String::printf(
      "%s = texture2D(%s, %s).%s;\r\n", _declareOutput(output, state).c_str(), _samplerName.c_str(),
      uvInput->associatedVariableName().c_str(), swizzle.c_str());
    return;
  }

  const auto complement = String::printf(" * %s", _textureInfoName.c_str());

  state.compilationString += String::printf("#ifdef %s\r\n", _defineName.c_str());
  state.compilationString += String::printf(
    "%s = texture2D(%s, %s).%s%s;\r\n", _declareOutput(output, state).c_str(), _samplerName.c_str(),
    _transformedUVName.c_str(), swizzle.c_str(), complement.c_str());
  state.compilationString += "#else\r\n";
  state.compilationString += String::printf(
    "%s = texture2D(%s, vMain%s).%s%s;\r\n", _declareOutput(output, state).c_str(),
    _samplerName.c_str(), uvInput->associatedVariableName().c_str(), swizzle.c_str(),
    complement.c_str());
  state.compilationString += "#endif\r\n";
}

TextureBlock& TextureBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    _injectVertexCode(state);
    return *this;
  }

  state.sharedData->blockingBlocks.emplace_back(
    std::static_pointer_cast<NodeMaterialBlock>(shared_from_this()));
  state.sharedData->textureBlocks.emplace_back(
    std::static_pointer_cast<TextureBlock>(shared_from_this()));

  _samplerName = state._getFreeVariableName(name + "Sampler");
  state.samplers.emplace_back(_samplerName);
  state._samplerDeclaration += String::printf("uniform sampler2D %s;\r\n", _samplerName.c_str());

  // Fragment
  state.sharedData->bindableBlocks.emplace_back(this);
  if (_isMixed()) {
    state.sharedData->blocksWithDefines.emplace_back(this);
    state._emitUniformFromString(_textureInfoName, "float");
  }

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name);
    }
  }

  return *this;
}

std::string TextureBlock::_dumpPropertiesCode()
{
  if (!texture) {
    return "";
  }

  const auto codeString = String::printf("%s.texture = new BABYLON.Texture(\"%s\");\r\n",
                                         _codeVariableName.c_str(), texture->name.c_str());

  return codeString;
}

json TextureBlock::serialize() const
{
  return nullptr;
}

void TextureBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
