#include <babylon/materials/node/blocks/dual/reflection_texture_base_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ReflectionTextureBaseBlock::ReflectionTextureBaseBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , texture{nullptr}
    , position{this, &ReflectionTextureBaseBlock::get_position}
    , worldPosition{this, &ReflectionTextureBaseBlock::get_worldPosition}
    , worldNormal{this, &ReflectionTextureBaseBlock::get_worldNormal}
    , world{this, &ReflectionTextureBaseBlock::get_world}
    , cameraPosition{this, &ReflectionTextureBaseBlock::get_cameraPosition}
    , view{this, &ReflectionTextureBaseBlock::get_view}
{
}

ReflectionTextureBaseBlock::~ReflectionTextureBaseBlock() = default;

std::string ReflectionTextureBaseBlock::getClassName() const
{
  return "ReflectionTextureBaseBlock";
}

BaseTexturePtr ReflectionTextureBaseBlock::_getTexture() const
{
  return texture;
}

void ReflectionTextureBaseBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!position()->isConnected()) {
    auto positionInput = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "position"; });

    if (!positionInput) {
      positionInput = InputBlock::New("position");
      positionInput->setAsAttribute();
    }
    positionInput->output()->connectTo(position);
  }

  if (!world()->isConnected()) {
    auto worldInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->systemValue() == NodeMaterialSystemValues::World;
    });

    if (!worldInput) {
      worldInput = InputBlock::New("world");
      worldInput->setAsSystemValue(NodeMaterialSystemValues::World);
    }
    worldInput->output()->connectTo(world);
  }

  if (!view()->isConnected()) {
    auto viewInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->systemValue() == NodeMaterialSystemValues::View;
    });

    if (!viewInput) {
      viewInput = InputBlock::New("view");
      viewInput->setAsSystemValue(NodeMaterialSystemValues::View);
    }
    viewInput->output()->connectTo(view);
  }
}

void ReflectionTextureBaseBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                                const NodeMaterialPtr& /*nodeMaterial*/,
                                                NodeMaterialDefines& defines, bool /*useInstances*/,
                                                SubMesh* /*subMesh*/)
{
  if (!defines._areTexturesDirty) {
    return;
  }

  const auto iTexture = _getTexture();

  if (!iTexture || !iTexture->getTextureMatrix()) {
    return;
  }

  defines.setValue(_define3DName, iTexture->isCube(), true);
  defines.setValue(_defineLocalCubicName, iTexture->boundingBoxSize() ? true : false, true);
  defines.setValue(_defineExplicitName,
                   iTexture->coordinatesMode() == Constants::TEXTURE_EXPLICIT_MODE, true);
  defines.setValue(_defineSkyboxName, iTexture->coordinatesMode() == Constants::TEXTURE_SKYBOX_MODE,
                   true);
  defines.setValue(_defineCubicName, iTexture->coordinatesMode() == Constants::TEXTURE_CUBIC_MODE,
                   true);
  defines.setValue(_defineSphericalName,
                   iTexture->coordinatesMode() == Constants::TEXTURE_SPHERICAL_MODE, true);
  defines.setValue(_definePlanarName, iTexture->coordinatesMode() == Constants::TEXTURE_PLANAR_MODE,
                   true);
  defines.setValue(_defineProjectionName,
                   iTexture->coordinatesMode() == Constants::TEXTURE_PROJECTION_MODE, true);
  defines.setValue(_defineEquirectangularName,
                   iTexture->coordinatesMode() == Constants::TEXTURE_EQUIRECTANGULAR_MODE, true);
  defines.setValue(_defineEquirectangularFixedName,
                   iTexture->coordinatesMode() == Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MODE,
                   true);
  defines.setValue(
    _defineMirroredEquirectangularFixedName,
    iTexture->coordinatesMode() == Constants::TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE, true);
}

bool ReflectionTextureBaseBlock::isReady(AbstractMesh* /*mesh*/,
                                         const NodeMaterialPtr& /*nodeMaterial*/,
                                         const NodeMaterialDefines& /*defines*/,
                                         bool /*useInstances*/)
{
  const auto iTexture = _getTexture();

  if (iTexture && !iTexture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void ReflectionTextureBaseBlock::bind(const EffectPtr& effect,
                                      const NodeMaterialPtr& /*nodeMaterial*/, Mesh* mesh,
                                      SubMesh* /*subMesh*/)
{
  const auto iTexture = _getTexture();

  if (!mesh || !iTexture) {
    return;
  }

  effect->setMatrix(_reflectionMatrixName, *iTexture->getReflectionTextureMatrix());

  if (iTexture->isCube()) {
    effect->setTexture(_cubeSamplerName, iTexture);
  }
  else {
    effect->setTexture(_2DSamplerName, iTexture);
  }
}

std::string ReflectionTextureBaseBlock::handleVertexSide(NodeMaterialBuildState& state)
{
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
  _defineEquirectangularFixedName = state._getFreeDefineName("REFLECTIONMAP_EQUIRECTANGULAR_FIXED");
  _defineSkyboxName               = state._getFreeDefineName("REFLECTIONMAP_SKYBOX");
  _defineOppositeZ                = state._getFreeDefineName("REFLECTIONMAP_OPPOSITEZ");

  _reflectionMatrixName = state._getFreeVariableName("reflectionMatrix");

  state._emitUniformFromString(_reflectionMatrixName, "mat4");

  std::string code = "";

  const auto worldPosVaryingName = "v_" + worldPosition()->associatedVariableName();
  if (state._emitVaryingFromString(worldPosVaryingName, "vec4")) {
    code += StringTools::printf("%s = %s;\r\n", worldPosVaryingName.c_str(),
                                worldPosition()->associatedVariableName().c_str());
  }

  _positionUVWName = state._getFreeVariableName("positionUVW");
  _directionWName  = state._getFreeVariableName("directionW");

  if (state._emitVaryingFromString(_positionUVWName, "vec3", _defineSkyboxName)) {
    code += StringTools::printf("#ifdef %s\r\n", _defineSkyboxName.c_str());
    code += StringTools::printf("%s = %s.xyz;\r\n", _positionUVWName.c_str(),
                                position()->associatedVariableName().c_str());
    code += "#endif\r\n";
  }

  if (state._emitVaryingFromString(
        _directionWName, "vec3",
        StringTools::printf("defined(%s) || defined(%s)", _defineEquirectangularFixedName.c_str(),
                            _defineMirroredEquirectangularFixedName.c_str()))) {
    code += StringTools::printf("#if defined(%s) || defined(%s)\r\n",
                                _defineEquirectangularFixedName.c_str(),
                                _defineMirroredEquirectangularFixedName.c_str());
    code += StringTools::printf("%s = normalize(vec3(%s * vec4(%s.xyz, 0.0)));\r\n",
                                _directionWName.c_str(), world()->associatedVariableName().c_str(),
                                position()->associatedVariableName().c_str());
    code += "#endif\r\n";
  }

  return code;
}

void ReflectionTextureBaseBlock::handleFragmentSideInits(NodeMaterialBuildState& state)
{
  state.sharedData->blockingBlocks.emplace_back(shared_from_this());
  // TODO FIXME
  // state.sharedData->textureBlocks.emplace_back(
  //  std::static_pointer_cast<ReflectionTextureBlock>(shared_from_this()));

  // Samplers
  _cubeSamplerName = state._getFreeVariableName(name() + "CubeSampler");
  state.samplers.emplace_back(_cubeSamplerName);

  _2DSamplerName = state._getFreeVariableName(name() + "2DSampler");
  state.samplers.emplace_back(_2DSamplerName);

  state._samplerDeclaration += StringTools::printf("#ifdef %s\r\n", _define3DName.c_str());
  state._samplerDeclaration
    += StringTools::printf("uniform samplerCube %s;\r\n", _cubeSamplerName.c_str());
  state._samplerDeclaration += "#else\r\n";
  state._samplerDeclaration
    += StringTools::printf("uniform sampler2D %s;\r\n", _2DSamplerName.c_str());
  state._samplerDeclaration += "#endif\r\n";

  // Fragment
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());

  const auto iComments = StringTools::printf("//%s", name().c_str());
  state._emitFunction("ReciprocalPI", "#define RECIPROCAL_PI2 0.15915494", "");
  EmitFunctionFromIncludeOptions emitFunctionFromIncludeOptions;
  emitFunctionFromIncludeOptions.replaceStrings
    = {{"vec3 computeReflectionCoords", "void DUMMYFUNC"}};
  state._emitFunctionFromInclude("reflectionFunction", iComments, emitFunctionFromIncludeOptions);

  _reflectionColorName  = state._getFreeVariableName("reflectionColor");
  _reflectionVectorName = state._getFreeVariableName("reflectionUVW");
  _reflectionCoordsName = state._getFreeVariableName("reflectionCoords");
}

std::string ReflectionTextureBaseBlock::handleFragmentSideCodeReflectionCoords(
  std::string worldNormalVarName, std::string worldPos, bool onlyReflectionVector)
{
  if (worldPos.empty()) {
    worldPos = StringTools::printf("v_%s", worldPosition()->associatedVariableName().c_str());
  }
  const auto reflectionMatrix = _reflectionMatrixName;
  const auto direction        = StringTools::printf("normalize(%s)", _directionWName.c_str());
  const auto positionUVW      = _positionUVWName;
  const auto vEyePosition     = cameraPosition()->associatedVariableName();
  const auto iView            = view()->associatedVariableName();

  worldNormalVarName += ".xyz";

  auto code = StringTools::printf(
    R"(
      #ifdef %s
          vec3 %s = computeMirroredFixedEquirectangularCoords(%s, %s, %s);
      #endif

      #ifdef %s
          vec3 %s = computeFixedEquirectangularCoords(%s, %s, %s);
      #endif

      #ifdef %s
          vec3 %s = computeEquirectangularCoords(%s, %s, %s.xyz, %s);
      #endif

      #ifdef %s
          vec3 %s = computeSphericalCoords(%s, %s, %s, %s);
      #endif

      #ifdef %s
          vec3 %s = computePlanarCoords(%s, %s, %s.xyz, %s);
      #endif

      #ifdef %s
          #ifdef %s
              vec3 %s = computeCubicLocalCoords(%s, %s, %s.xyz, %s, vReflectionSize, vReflectionPosition);
          #else
          vec3 %s = computeCubicCoords(%s, %s, %s.xyz, %s);
          #endif
      #endif

      #ifdef %s
          vec3 %s = computeProjectionCoords(%s, %s, %s);
      #endif

      #ifdef %s
          vec3 %s = computeSkyBoxCoords(%s, %s);
      #endif

      #ifdef %s
          vec3 %s = vec3(0, 0, 0);
      #endif

      #ifdef %s
          %s.z *= -1.0;
      #endif\r\n
    )",
    _defineMirroredEquirectangularFixedName.c_str(), //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    direction.c_str(),                       //
    _defineEquirectangularFixedName.c_str(), //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    direction.c_str(),                  //
    _defineEquirectangularName.c_str(), //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    vEyePosition.c_str(), reflectionMatrix.c_str(), //
    _defineSphericalName.c_str(),                   //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(), iView.c_str(),
    reflectionMatrix.c_str(),  //
    _definePlanarName.c_str(), //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    vEyePosition.c_str(), reflectionMatrix.c_str(), //
    _defineCubicName.c_str(),                       //
    _defineLocalCubicName.c_str(),                  //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    vEyePosition.c_str(), reflectionMatrix.c_str(), //
    _reflectionVectorName.c_str(), worldPos.c_str(), worldNormalVarName.c_str(),
    vEyePosition.c_str(), reflectionMatrix.c_str(),                                           //
    _defineProjectionName.c_str(),                                                            //
    _reflectionVectorName.c_str(), worldPos.c_str(), iView.c_str(), reflectionMatrix.c_str(), //
    _defineSkyboxName.c_str(),                                                                //
    _reflectionVectorName.c_str(), positionUVW.c_str(),
    reflectionMatrix.c_str(),      //
    _defineExplicitName.c_str(),   //
    _reflectionVectorName.c_str(), //
    _defineOppositeZ.c_str(),      //
    _reflectionVectorName.c_str()  //
  );

  if (!onlyReflectionVector) {
    code += StringTools::printf(
      R"(
        #ifdef %s
            vec3 %s = %s;
        #else
            vec2 %s = %s.xy;
            #ifdef %s
                %s /= %s.z;
            #endif
            %s.y = 1.0 - %s.y;
        #endif\r\n
       )",
      _define3DName.c_str(),                                        //
      _reflectionCoordsName.c_str(), _reflectionVectorName.c_str(), //
      _reflectionCoordsName.c_str(), _reflectionVectorName.c_str(), //
      _defineProjectionName.c_str(),                                //
      _reflectionCoordsName.c_str(), _reflectionVectorName.c_str(), //
      _reflectionCoordsName.c_str(), _reflectionCoordsName.c_str()  //
    );
  }

  return code;
}

std::string ReflectionTextureBaseBlock::handleFragmentSideCodeReflectionColor(
  const std::string& lodVarName, const std::string& swizzleLookupTexture)
{
  const auto colorType
    = "vec"
      + (swizzleLookupTexture.empty() ? "4" : std::to_string(swizzleLookupTexture.size() - 1));

  auto code
    = StringTools::printf(R"(%s %s;
              #ifdef %s\r\n)",
                          colorType.c_str(), _reflectionColorName.c_str(), _define3DName.c_str());

  if (!lodVarName.empty()) {
    code += StringTools::printf("%s = textureCubeLodEXT(%s, %s, %s)%s;\r\n",
                                _reflectionColorName.c_str(), _cubeSamplerName.c_str(),
                                _reflectionVectorName.c_str(), lodVarName.c_str(),
                                swizzleLookupTexture.c_str());
  }
  else {
    code += StringTools::printf("%s = textureCube(%s, %s)%s;\r\n", _reflectionColorName.c_str(),
                                _cubeSamplerName.c_str(), _reflectionVectorName.c_str(),
                                swizzleLookupTexture.c_str());
  }

  code += R"(
             #else\r\n)";

  if (!lodVarName.empty()) {
    code += StringTools::printf("%s = texture2DLodEXT(%s, %s, %s)%s;\r\n",
                                _reflectionColorName.c_str(), _2DSamplerName.c_str(),
                                _reflectionCoordsName.c_str(), lodVarName.c_str(),
                                swizzleLookupTexture.c_str());
  }
  else {
    code += StringTools::printf("%s = texture2D(%s, %s)%s;\r\n", _reflectionColorName.c_str(),
                                _2DSamplerName.c_str(), _reflectionCoordsName.c_str(),
                                swizzleLookupTexture.c_str());
  }

  code += "#endif\r\n";

  return code;
}

std::string ReflectionTextureBaseBlock::writeOutputs(NodeMaterialBuildState& state,
                                                     const std::string& varName)
{
  std::string code = "";

  if (state.target == NodeMaterialBlockTargets::Fragment) {
    for (const auto& output : _outputs) {
      if (output->hasEndpoints()) {
        code += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                                    varName.c_str(), output->name.c_str());
      }
    }
  }

  return code;
}

ReflectionTextureBaseBlock& ReflectionTextureBaseBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);
  return *this;
}

std::string ReflectionTextureBaseBlock::_dumpPropertiesCode()
{
  if (!texture) {
    return "";
  }

  std::string codeString;

  if (texture->isCube()) {
    codeString = StringTools::printf("%s.texture = CubeTexture::New(\"%s\");\r\n",
                                     _codeVariableName.c_str(), texture->name.c_str());
  }
  else {
    codeString = StringTools::printf("%s.texture = Texture::New(\"%s\");\r\n",
                                     _codeVariableName.c_str(), texture->name.c_str());
  }
  codeString += StringTools::printf("%s.texture.coordinatesMode = %u;\r\n",
                                    _codeVariableName.c_str(), texture->coordinatesMode());

  return codeString;
}

json ReflectionTextureBaseBlock::serialize() const
{
  return nullptr;
}

void ReflectionTextureBaseBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
