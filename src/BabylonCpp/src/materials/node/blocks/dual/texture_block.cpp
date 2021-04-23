#include <babylon/materials/node/blocks/dual/texture_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

TextureBlock::TextureBlock(const std::string& iName, bool fragmentOnly)
    : NodeMaterialBlock{iName, fragmentOnly ? NodeMaterialBlockTargets::Fragment :
                                              NodeMaterialBlockTargets::VertexAndFragment}
    , texture{this, &TextureBlock::get_texture, &TextureBlock::set_texture}
    , convertToGammaSpace{false}
    , convertToLinearSpace{false}
    , uv{this, &TextureBlock::get_uv}
    , rgba{this, &TextureBlock::get_rgba}
    , rgb{this, &TextureBlock::get_rgb}
    , r{this, &TextureBlock::get_r}
    , g{this, &TextureBlock::get_g}
    , b{this, &TextureBlock::get_b}
    , a{this, &TextureBlock::get_a}
    , _isMixed{this, &TextureBlock::get__isMixed}
    , _texture{nullptr}
    , _currentTarget{NodeMaterialBlockTargets::VertexAndFragment}
{
  _fragmentOnly = fragmentOnly;

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

  _inputs[0]->_prioritizeVertex = !fragmentOnly;
}

TextureBlock::~TextureBlock() = default;

std::string TextureBlock::getClassName() const
{
  return "TextureBlock";
}

TexturePtr& TextureBlock::get_texture()
{
  return _texture;
}

void TextureBlock::set_texture(const TexturePtr& iTexture)
{
  if (_texture == iTexture) {
    return;
  }

  const auto scene
    = iTexture && iTexture->getScene() ? iTexture->getScene() : Engine::LastCreatedScene();

  if (!iTexture && scene) {
    scene->markAllMaterialsAsDirty(
      Constants::MATERIAL_TextureDirtyFlag,
      [this](Material* mat) -> bool { return mat->hasTexture(_texture); });
  }

  _texture = iTexture;

  if (iTexture && scene) {
    scene->markAllMaterialsAsDirty(
      Constants::MATERIAL_TextureDirtyFlag,
      [iTexture](Material* mat) -> bool { return mat->hasTexture(iTexture); });
  }
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
  if (_fragmentOnly) {
    _currentTarget = NodeMaterialBlockTargets::Fragment;
    return _currentTarget;
  }

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
    if (parent->target() == NodeMaterialBlockTargets::Fragment) {
      _currentTarget = NodeMaterialBlockTargets::Fragment;
      return _currentTarget;
    }

    if (parent->target() == NodeMaterialBlockTargets::Vertex) {
      _currentTarget = NodeMaterialBlockTargets::VertexAndFragment;
      return _currentTarget;
    }

    if (parent->target() == NodeMaterialBlockTargets::Neutral
        || parent->target() == NodeMaterialBlockTargets::VertexAndFragment) {
      auto& parentBlock = parent->ownerBlock();

      if (parentBlock->target() == NodeMaterialBlockTargets::Fragment) {
        _currentTarget = NodeMaterialBlockTargets::Fragment;
        return _currentTarget;
      }

      parent = nullptr;
      for (const auto& input : parentBlock->inputs()) {
        if (input->connectedPoint()) {
          parent = input->connectedPoint();
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
    if (material->mode() == NodeMaterialModes::PostProcess) {
      auto uvInput
        = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
            return inputBlock->isAttribute() && inputBlock->name() == "uv";
          });

      if (uvInput) {
        uvInput->connectTo(shared_from_this());
      }
    }
    else {
      const auto attributeName
        = material->mode() == NodeMaterialModes::Particle ? "particle_uv" : "uv";

      auto uvInput = material->getInputBlockByPredicate(
        [&attributeName](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->isAttribute() && inputBlock->name() == attributeName;
        });

      if (!uvInput) {
        uvInput = InputBlock::New("uv");
        uvInput->setAsAttribute(attributeName);
      }
      uvInput->output()->connectTo(uv);
    }
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
                                  NodeMaterialDefines& defines, bool /*useInstances*/,
                                  SubMesh* /*subMesh*/)
{
  if (!defines._areTexturesDirty) {
    return;
  }

  const auto iTexture = texture();
  if (!iTexture || !iTexture->getTextureMatrix()) {
    defines.setValue(_defineName, false);
    defines.setValue(_mainUVDefineName, true);
    return;
  }

  defines.setValue(_linearDefineName, convertToGammaSpace);
  defines.setValue(_gammaDefineName, convertToLinearSpace);
  if (_isMixed) {
    if (!iTexture->getTextureMatrix()->isIdentityAs3x2()) {
      defines.setValue(_defineName, true);
    }
    else {
      defines.setValue(_defineName, false);
      defines.setValue(_mainUVDefineName, true);
    }
  }
}

bool TextureBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                           const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  if (texture() && !texture()->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void TextureBlock::bind(Effect* effect, const NodeMaterialPtr& /*nodeMaterial*/, Mesh* /*mesh*/,
                        SubMesh* /*subMesh*/)
{
  const auto iTexture = texture();
  if (iTexture) {
    return;
  }

  if (_isMixed) {
    effect->setFloat(_textureInfoName, iTexture->level);
    effect->setMatrix(_textureTransformName, *iTexture->getTextureMatrix());
  }
  effect->setTexture(_samplerName, iTexture);
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
  _mainUVDefineName = "VMAIN" + StringTools::toUpperCase(uvInput->associatedVariableName());

  _mainUVName           = StringTools::printf("vMain%s", uvInput->associatedVariableName().c_str());
  _transformedUVName    = state._getFreeVariableName("transformedUV");
  _textureTransformName = state._getFreeVariableName("textureTransform");
  _textureInfoName      = state._getFreeVariableName("textureInfoName");

  state._emitVaryingFromString(_transformedUVName, "vec2", _defineName);
  state._emitVaryingFromString(_mainUVName, "vec2", _mainUVDefineName);

  state._emitUniformFromString(_textureTransformName, "mat4", _defineName);

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _defineName.c_str());
  state.compilationString += StringTools::printf(
    "%s = vec2(%s * vec4(%s.xy, 1.0, 0.0));\r\n", _transformedUVName.c_str(),
    _textureTransformName.c_str(), uvInput->associatedVariableName().c_str());
  state.compilationString
    += StringTools::printf("#elif defined(%s)\r\n", _mainUVDefineName.c_str());
  state.compilationString += StringTools::printf("%s = %s.xy;\r\n", _mainUVName.c_str(),
                                                 uvInput->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";

  for (const auto& o : _outputs) {
    if (!o->isConnectedInVertexShader()) {
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

void TextureBlock::_writeTextureRead(NodeMaterialBuildState& state, bool vertexMode)
{
  const auto& uvInput = uv();

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

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _defineName.c_str());
  state.compilationString
    += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                           _samplerName.c_str(), _transformedUVName.c_str());
  state.compilationString
    += StringTools::printf("#elif defined(%s)\r\n", _mainUVDefineName.c_str());
  state.compilationString
    += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                           _samplerName.c_str(), _mainUVName.c_str());
  state.compilationString += "#endif\r\n";
}

void TextureBlock::_generateConversionCode(NodeMaterialBuildState& state,
                                           const NodeMaterialConnectionPointPtr& output,
                                           const std::string& swizzle)
{
  if (swizzle != "a") { // no conversion if the output is "a" (alpha)
    state.compilationString += StringTools::printf("#ifdef %s\r\n", _linearDefineName.c_str());
    state.compilationString
      += StringTools::printf("%s = toGammaSpace(%s);\r\n", output->associatedVariableName().c_str(),
                             output->associatedVariableName().c_str());
    state.compilationString += "#endif\r\n";

    state.compilationString += StringTools::printf("#ifdef %s\r\n", _gammaDefineName.c_str());
    state.compilationString += StringTools::printf("%s = toLinearSpace(%s);\r\n",
                                                   output->associatedVariableName().c_str(),
                                                   output->associatedVariableName().c_str());
    state.compilationString += "#endif\r\n";
  }
}

void TextureBlock::_writeOutput(NodeMaterialBuildState& state,
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

  const auto complement = StringTools::printf(" * %s", _textureInfoName.c_str());

  state.compilationString
    += StringTools::printf("%s = %s.%s%s;\r\n", _declareOutput(output, state).c_str(),
                           _tempTextureRead.c_str(), swizzle.c_str(), complement.c_str());
  _generateConversionCode(state, output, swizzle);
}

TextureBlock& TextureBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex || _fragmentOnly
      || (state.target == NodeMaterialBlockTargets::Fragment && _tempTextureRead == "")) {
    _tempTextureRead  = state._getFreeVariableName("tempTextureRead");
    _linearDefineName = state._getFreeDefineName("ISLINEAR");
    _gammaDefineName  = state._getFreeDefineName("ISGAMMA");
  }

  if ((!_isMixed && state.target == NodeMaterialBlockTargets::Fragment)
      || (_isMixed && state.target == NodeMaterialBlockTargets::Vertex)) {
    _samplerName = state._getFreeVariableName(name() + "Sampler");

    state._emit2DSampler(_samplerName);

    // Declarations
    state.sharedData->blockingBlocks.emplace_back(shared_from_this());
    state.sharedData->textureBlocks.emplace_back(
      std::static_pointer_cast<TextureBlock>(shared_from_this()));
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
    state.sharedData->bindableBlocks.emplace_back(shared_from_this());
  }

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    _injectVertexCode(state);
    return *this;
  }

  // Fragment
  for (const auto& o : _outputs) {
    if (!o->isConnectedInFragmentShader()) {
      return *this;
    }
  }

  if (_isMixed) {
    // Reexport the sampler
    state._emit2DSampler(_samplerName);
  }

  auto iComments = StringTools::printf("//%s", name().c_str());
  state._emitFunctionFromInclude("helperFunctions", iComments);

  if (_isMixed) {
    state._emitUniformFromString(_textureInfoName, "float");
  }

  _writeTextureRead(state);

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name);
    }
  }

  return *this;
}

std::string TextureBlock::_dumpPropertiesCode()
{
  const auto iTexture = texture();
  if (!iTexture) {
    return "";
  }

  auto codeString = StringTools::printf("%s.texture = Texture::New(\"%s\", nullptr, %s, %s);\r\n",
                                        _codeVariableName.c_str(), iTexture->name.c_str(),
                                        iTexture->noMipmap() ? "true" : "false",
                                        iTexture->invertY() ? "true" : "false");
  codeString += StringTools::printf("%s.texture.wrapU = %u;\r\n", _codeVariableName.c_str(),
                                    iTexture->wrapU());
  codeString += StringTools::printf("%s.texture.wrapV = %u;\r\n", _codeVariableName.c_str(),
                                    iTexture->wrapV());
  codeString
    += StringTools::printf("%s.texture.uAng = %f;\r\n", _codeVariableName.c_str(), iTexture->uAng);
  codeString
    += StringTools::printf("%s.texture.vAng = %f;\r\n", _codeVariableName.c_str(), iTexture->vAng);
  codeString
    += StringTools::printf("%s.texture.wAng = %f;\r\n", _codeVariableName.c_str(), iTexture->wAng);
  codeString += StringTools::printf("%s.texture.uOffset = %f;\r\n", _codeVariableName.c_str(),
                                    iTexture->uOffset);
  codeString += StringTools::printf("%s.texture.vOffset = %f;\r\n", _codeVariableName.c_str(),
                                    iTexture->vOffset);
  codeString += StringTools::printf("%s.texture.uScale = %f;\r\n", _codeVariableName.c_str(),
                                    iTexture->uScale);
  codeString += StringTools::printf("%s.texture.vScale = %f;\r\n", _codeVariableName.c_str(),
                                    iTexture->vScale);
  codeString += StringTools::printf("%s.texture.coordinatesMode = %u;\r\n",
                                    _codeVariableName.c_str(), iTexture->coordinatesMode());
  codeString += StringTools::printf("%s.convertToGammaSpace = %s;\r\n", _codeVariableName.c_str(),
                                    convertToGammaSpace ? "true" : "false");
  codeString += StringTools::printf("%s.convertToLinearSpace = %s;\r\n", _codeVariableName.c_str(),
                                    convertToLinearSpace ? "true" : "false");

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
