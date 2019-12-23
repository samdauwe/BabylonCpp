#include <babylon/materials/node/blocks/dual/reflection_texture_block.h>

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

ReflectionTextureBlock::ReflectionTextureBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , texture{nullptr}
    , position{this, &ReflectionTextureBlock::get_position}
    , worldPosition{this, &ReflectionTextureBlock::get_worldPosition}
    , worldNormal{this, &ReflectionTextureBlock::get_worldNormal}
    , world{this, &ReflectionTextureBlock::get_world}
    , cameraPosition{this, &ReflectionTextureBlock::get_cameraPosition}
    , view{this, &ReflectionTextureBlock::get_view}
    , rgb{this, &ReflectionTextureBlock::get_rgb}
    , r{this, &ReflectionTextureBlock::get_r}
    , g{this, &ReflectionTextureBlock::get_g}
    , b{this, &ReflectionTextureBlock::get_b}
{
  registerInput("position", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("world", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                NodeMaterialBlockTargets::Vertex);

  registerInput("cameraPosition", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                NodeMaterialBlockTargets::Fragment);

  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
}

ReflectionTextureBlock::~ReflectionTextureBlock() = default;

std::string ReflectionTextureBlock::getClassName() const
{
  return "ReflectionTextureBlock";
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_position()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_worldPosition()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_worldNormal()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_world()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_cameraPosition()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_view()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_rgb()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_r()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_g()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_b()
{
  return _outputs[3];
}

void ReflectionTextureBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!position()->isConnected()) {
    auto positionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->isAttribute() && inputBlock->name == "position";
        });

    if (!positionInput) {
      positionInput = InputBlock::New("position");
      positionInput->setAsAttribute();
    }
    positionInput->output()->connectTo(position());
  }

  if (!world()->isConnected()) {
    auto worldInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->systemValue() == NodeMaterialSystemValues::World;
        });

    if (!worldInput) {
      worldInput = InputBlock::New("world");
      worldInput->setAsSystemValue(NodeMaterialSystemValues::World);
    }
    worldInput->output()->connectTo(world());
  }

  if (!cameraPosition()->isConnected()) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->systemValue() == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition());
  }

  if (!view()->isConnected()) {
    auto viewInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->systemValue() == NodeMaterialSystemValues::View;
        });

    if (!viewInput) {
      viewInput = InputBlock::New("view");
      viewInput->setAsSystemValue(NodeMaterialSystemValues::View);
    }
    viewInput->output()->connectTo(view());
  }
}

void ReflectionTextureBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                            const NodeMaterialPtr& /*nodeMaterial*/,
                                            NodeMaterialDefines& defines, bool /*useInstances*/)
{
  if (!defines._areTexturesDirty) {
    return;
  }

  if (!texture || !texture->getTextureMatrix()) {
    return;
  }

  defines.setValue(_define3DName, texture->isCube());
  defines.setValue(_defineLocalCubicName, texture->boundingBoxSize() ? true : false);
  defines.setValue(_defineExplicitName,
                   texture->coordinatesMode() == Constants::TEXTURE_EXPLICIT_MODE);
  defines.setValue(_defineSkyboxName, texture->coordinatesMode() == Constants::TEXTURE_SKYBOX_MODE);
  defines.setValue(_defineCubicName, texture->coordinatesMode() == Constants::TEXTURE_CUBIC_MODE);
  defines.setValue(_defineSphericalName,
                   texture->coordinatesMode() == Constants::TEXTURE_SPHERICAL_MODE);
  defines.setValue(_definePlanarName, texture->coordinatesMode() == Constants::TEXTURE_PLANAR_MODE);
  defines.setValue(_defineProjectionName,
                   texture->coordinatesMode() == Constants::TEXTURE_PROJECTION_MODE);
  defines.setValue(_defineEquirectangularName,
                   texture->coordinatesMode() == Constants::TEXTURE_EQUIRECTANGULAR_MODE);
  defines.setValue(_defineEquirectangularFixedName,
                   texture->coordinatesMode() == Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MODE);
  defines.setValue(_defineMirroredEquirectangularFixedName,
                   texture->coordinatesMode()
                     == Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE);
}

bool ReflectionTextureBlock::isReady(AbstractMesh* /*mesh*/,
                                     const NodeMaterialPtr& /*nodeMaterial*/,
                                     const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  if (texture && !texture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void ReflectionTextureBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& /*nodeMaterial*/,
                                  Mesh* mesh)
{
  if (!mesh || !texture) {
    return;
  }

  effect->setMatrix(_reflectionMatrixName, *texture->getReflectionTextureMatrix());

  if (texture->isCube()) {
    effect->setTexture(_cubeSamplerName, texture);
  }
  else {
    effect->setTexture(_2DSamplerName, texture);
  }
}

void ReflectionTextureBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  auto worldPosVaryingName
    = String::printf("v_%s", worldPosition()->associatedVariableName().c_str());
  if (state._emitVaryingFromString(worldPosVaryingName, "vec4")) {
    state.compilationString += String::printf("%s = %s;\r\n", worldPosVaryingName.c_str(),
                                              worldPosition()->associatedVariableName().c_str());
  }

  auto worldNormalVaryingName
    = String::printf("v_%s", worldNormal()->associatedVariableName().c_str());
  if (state._emitVaryingFromString(worldNormalVaryingName, "vec4")) {
    state.compilationString += String::printf("%s = %s;\r\n", worldNormalVaryingName.c_str(),
                                              worldNormal()->associatedVariableName().c_str());
  }

  _positionUVWName = state._getFreeVariableName("positionUVW");
  _directionWName  = state._getFreeVariableName("directionW");

  if (state._emitVaryingFromString(_positionUVWName, "vec3", _defineSkyboxName)) {
    state.compilationString += String::printf("#ifdef %s\r\n", _defineSkyboxName.c_str());
    state.compilationString += String::printf("%s = %s;\r\n", _positionUVWName.c_str(),
                                              position()->associatedVariableName().c_str());
    state.compilationString += "#endif\r\n";
  }

  if (state._emitVaryingFromString(
        _directionWName, "vec3",
        String::printf("defined(%s) || defined(%s)", _defineEquirectangularFixedName.c_str(),
                       _defineMirroredEquirectangularFixedName.c_str()))) {
    state.compilationString += String::printf("#if defined(%s) || defined(%s)\r\n",
                                              _defineEquirectangularFixedName.c_str(),
                                              _defineMirroredEquirectangularFixedName.c_str());
    state.compilationString += String::printf(
      "%s = normalize(vec3(%s * vec4(%s, 0.0)));\r\n", _directionWName.c_str(),
      world()->associatedVariableName().c_str(), position()->associatedVariableName().c_str());
    state.compilationString += "#endif\r\n";
  }
}

void ReflectionTextureBlock::_writeOutput(NodeMaterialBuildState& state,
                                          const NodeMaterialConnectionPointPtr& output,
                                          const std::string& swizzle)
{
  state.compilationString
    += String::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                      _reflectionColorName.c_str(), swizzle.c_str());
}

ReflectionTextureBlock& ReflectionTextureBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (!texture) {
    if (state.target == NodeMaterialBlockTargets::Fragment) {
      for (const auto& output : _outputs) {
        if (output->hasEndpoints()) {
          state.compilationString += String::printf(
            "%s = vec3(0.).%s;\r\n", _declareOutput(output, state).c_str(), output->name.c_str());
        }
      }
    }
    return *this;
  }

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    _define3DName              = state._getFreeDefineName("REFLECTIONMAP_3D");
    _defineCubicName           = state._getFreeDefineName("REFLECTIONMAP_CUBIC");
    _defineSphericalName       = state._getFreeDefineName("REFLECTIONMAP_SPHERICAL");
    _definePlanarName          = state._getFreeDefineName("REFLECTIONMAP_PLANAR");
    _defineProjectionName      = state._getFreeDefineName("REFLECTIONMAP_PROJECTION");
    _defineExplicitName        = state._getFreeDefineName("REFLECTIONMAP_EXPLICIT");
    _defineEquirectangularName = state._getFreeDefineName("REFLECTIONMAP_EQUIRECTANGULAR");
    _defineLocalCubicName      = state._getFreeDefineName("USE_LOCAL_REFLECTIONMAP_CUBIC");
    _defineMirroredEquirectangularFixedName
      = state._getFreeDefineName("REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED");
    _defineEquirectangularFixedName
      = state._getFreeDefineName("REFLECTIONMAP_EQUIRECTANGULAR_FIXED");
    _defineSkyboxName = state._getFreeDefineName("REFLECTIONMAP_SKYBOX");

    // Vertex
    _injectVertexCode(state);
    return *this;
  }

  state.sharedData->blockingBlocks.emplace_back(
    std::static_pointer_cast<NodeMaterialBlock>(shared_from_this()));
  state.sharedData->textureBlocks.emplace_back(
    std::static_pointer_cast<ReflectionTextureBlock>(shared_from_this()));

  // Samplers
  _cubeSamplerName = state._getFreeVariableName(name + "CubeSampler");
  state.samplers.emplace_back(_cubeSamplerName);

  _2DSamplerName = state._getFreeVariableName(name + "2DSampler");
  state.samplers.emplace_back(_2DSamplerName);

  state._samplerDeclaration += String::printf("#ifdef %s\r\n", _define3DName.c_str());
  state._samplerDeclaration
    += String::printf("uniform samplerCube %s;\r\n", _cubeSamplerName.c_str());
  state._samplerDeclaration += "#else\r\n";
  state._samplerDeclaration += String::printf("uniform sampler2D %s;\r\n", _2DSamplerName.c_str());
  state._samplerDeclaration += "#endif\r\n";

  // Fragment
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());

  auto comments = String::printf("//%s", name.c_str());
  state._emitFunction("ReciprocalPI", "#define RECIPROCAL_PI2 0.15915494", "");
  state._emitFunctionFromInclude("reflectionFunction", comments);

  _reflectionColorName    = state._getFreeVariableName("reflectionColor");
  _reflectionCoordsName   = state._getFreeVariableName("reflectionUVW");
  _reflection2DCoordsName = state._getFreeVariableName("reflectionUV");
  _reflectionMatrixName   = state._getFreeVariableName("reflectionMatrix");

  state._emitUniformFromString(_reflectionMatrixName, "mat4");

  // Code
  auto worldPos     = String::printf("v_%s", worldPosition()->associatedVariableName().c_str());
  auto _worldNormal = String::printf("v_%s.xyz", worldNormal()->associatedVariableName().c_str());
  auto reflectionMatrix = _reflectionMatrixName;
  auto direction        = String::printf("normalize(%s)", _directionWName.c_str());
  auto _positionUVW     = _positionUVWName;
  auto vEyePosition     = cameraPosition()->associatedVariableName();
  auto _view            = view()->associatedVariableName();

  state.compilationString += String::printf("vec3 %s;\r\n", _reflectionColorName.c_str());
  state.compilationString
    += String::printf("#ifdef %s \r\n", _defineMirroredEquirectangularFixedName.c_str());
  state.compilationString += String::printf(
    "    vec3 %s = computeMirroredFixedEquirectangularCoords(%s, %s, %s);\r\n",
    _reflectionCoordsName.c_str(), worldPos.c_str(), _worldNormal.c_str(), direction.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString
    += String::printf("#ifdef %s\r\n", _defineEquirectangularFixedName.c_str());
  state.compilationString += String::printf(
    "    vec3 %s = computeFixedEquirectangularCoords(%s, %s, %s);\r\n",
    _reflectionCoordsName.c_str(), worldPos.c_str(), _worldNormal.c_str(), direction.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineEquirectangularName.c_str());
  state.compilationString
    += String::printf("    vec3 %s = computeEquirectangularCoords(%s, %s, %s, %s);\r\n",
                      _reflectionCoordsName.c_str(), worldPos.c_str(), _worldNormal.c_str(),
                      vEyePosition.c_str(), reflectionMatrix.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineSphericalName.c_str());
  state.compilationString += String::printf(
    "    vec3 %s = computeSphericalCoords(%s, %s, %s, %s);\r\n", _reflectionCoordsName.c_str(),
    worldPos.c_str(), _worldNormal.c_str(), _view.c_str(), reflectionMatrix.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _definePlanarName.c_str());
  state.compilationString += String::printf(
    "    vec3 %s = computePlanarCoords(%s, %s, %s, %s);\r\n", _reflectionCoordsName.c_str(),
    worldPos.c_str(), _worldNormal.c_str(), vEyePosition.c_str(), reflectionMatrix.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineCubicName.c_str());
  state.compilationString += String::printf("    #ifdef %s\r\n", _defineLocalCubicName.c_str());
  state.compilationString += String::printf(
    "        vec3 %s = computeCubicLocalCoords(%s, %s, %s, %s, vReflectionSize, "
    "vReflectionPosition);\r\n",
    _reflectionCoordsName.c_str(), worldPos.c_str(), _worldNormal.c_str(), vEyePosition.c_str(),
    reflectionMatrix.c_str());
  state.compilationString += "    #else\r\n";
  state.compilationString += String::printf(
    "       vec3 %s = computeCubicCoords(%s, %s, %s.xyz, %s);\r\n", _reflectionCoordsName.c_str(),
    worldPos.c_str(), _worldNormal.c_str(), vEyePosition.c_str(), reflectionMatrix.c_str());
  state.compilationString += "    #endif\r\n";
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineProjectionName.c_str());
  state.compilationString += String::printf(
    "    vec3 %s = computeProjectionCoords(%s, %s, %s);\r\n", _reflectionCoordsName.c_str(),
    worldPos.c_str(), _view.c_str(), reflectionMatrix.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineSkyboxName.c_str());
  state.compilationString += String::printf("    vec3 %s = computeSkyBoxCoords(%s, %s);\r\n",
                                            _reflectionCoordsName.c_str(), _positionUVW.c_str(),
                                            reflectionMatrix.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _defineExplicitName.c_str());
  state.compilationString
    += String::printf("    vec3 %s = vec3(0, 0, 0);\r\n", _reflectionCoordsName.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf("#ifdef %s\r\n", _define3DName.c_str());
  state.compilationString
    += String::printf("%s = textureCube(%s, %s).rgb;\r\n", _reflectionColorName.c_str(),
                      _cubeSamplerName.c_str(), _reflectionCoordsName.c_str());
  state.compilationString += "#else\r\n";
  state.compilationString += String::printf("vec2 %s = %s.xy;\r\n", _reflection2DCoordsName.c_str(),
                                            _reflectionCoordsName.c_str());

  state.compilationString += String::printf("#ifdef %s\r\n", _defineProjectionName.c_str());
  state.compilationString += String::printf("%s /= %s.z;\r\n", _reflection2DCoordsName.c_str(),
                                            _reflectionCoordsName.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += String::printf(
    "%s.y = 1.0 - %s.y;\r\n", _reflection2DCoordsName.c_str(), _reflection2DCoordsName.c_str());
  state.compilationString
    += String::printf("%s = texture2D(%s, %s).rgb;\r\n", _reflectionColorName.c_str(),
                      _2DSamplerName.c_str(), _reflection2DCoordsName.c_str());
  state.compilationString += "#endif\r\n";

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name);
    }
  }

  return *this;
}

std::string ReflectionTextureBlock::_dumpPropertiesCode()
{
  if (!texture) {
    return "";
  }

  std::string codeString;

  if (texture->isCube()) {
    codeString = String::printf("%s.texture = CubeTexture::New(\"%s\");\r\n",
                                _codeVariableName.c_str(), texture->name.c_str());
  }
  else {
    codeString = String::printf("%s.texture = Texture::New(\"%s\");\r\n", _codeVariableName.c_str(),
                                texture->name.c_str());
  }
  codeString += String::printf("%s.texture.coordinatesMode = %u;\r\n", _codeVariableName.c_str(),
                               texture->coordinatesMode());

  return codeString;
}

json ReflectionTextureBlock::serialize() const
{
  return nullptr;
}

void ReflectionTextureBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                          const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
