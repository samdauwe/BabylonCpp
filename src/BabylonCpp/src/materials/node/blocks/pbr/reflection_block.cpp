#include <babylon/materials/node/blocks/pbr/reflection_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/spherical_polynomial.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ReflectionBlock::ReflectionBlock(const std::string& iName)
    : ReflectionTextureBaseBlock{iName}
    , worldPositionConnectionPoint{nullptr}
    , worldNormalConnectionPoint{nullptr}
    , cameraPositionConnectionPoint{nullptr}
    , useSphericalHarmonics{true}
    , forceIrradianceInFragment{false}
    , color{this, &ReflectionBlock::get_color}
    , reflection{this, &ReflectionBlock::get_reflection}
    , hasTexture{this, &ReflectionBlock::get_hasTexture}
    , reflectionColor{this, &ReflectionBlock::get_reflectionColor}
    , _scene{nullptr}
{
  _isUnique = true;
}

ReflectionBlock::~ReflectionBlock() = default;

void ReflectionBlock::RegisterConnections(const ReflectionBlockPtr& reflectionBlock)
{
  reflectionBlock->registerInput("position", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                                 NodeMaterialBlockTargets::Vertex);
  reflectionBlock->registerInput("world", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                                 NodeMaterialBlockTargets::Vertex);
  reflectionBlock->registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                                 NodeMaterialBlockTargets::Fragment);
  reflectionBlock->registerInput("color", NodeMaterialBlockConnectionPointTypes::Color3, true,
                                 NodeMaterialBlockTargets::Fragment);

  reflectionBlock->registerOutput(
    "reflection", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "reflection", reflectionBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> ReflectionBlockPtr { return ReflectionBlock::New(iName); },
      "ReflectionBlock"));
}

std::string ReflectionBlock::getClassName() const
{
  return "ReflectionBlock";
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_position()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_worldPosition()
{
  return worldPositionConnectionPoint;
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_worldNormal()
{
  return worldNormalConnectionPoint;
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_world()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_cameraPosition()
{
  return cameraPositionConnectionPoint;
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_view()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_color()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ReflectionBlock::get_reflection()
{
  return _outputs[0];
}

bool ReflectionBlock::get_hasTexture() const
{
  return _getTexture() != nullptr;
}

std::string ReflectionBlock::get_reflectionColor() const
{
  return color()->isConnected() ? color()->associatedVariableName() : "vec3(1., 1., 1.)";
}

BaseTexturePtr ReflectionBlock::_getTexture() const
{
  if (texture) {
    return texture;
  }

  return _scene->environmentTexture();
}

void ReflectionBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                     NodeMaterialDefines& defines, bool /*useInstances*/,
                                     SubMesh* /*subMesh*/)
{
  ReflectionTextureBaseBlock::prepareDefines(mesh, nodeMaterial, defines);

  const auto reflectionTexture = _getTexture();
  const auto iReflection       = reflectionTexture && reflectionTexture->getTextureMatrix();

  defines.setValue("REFLECTION", iReflection, true);

  if (!iReflection) {
    return;
  }

  defines.setValue(_defineLODReflectionAlpha, reflectionTexture->lodLevelInAlpha, true);
  defines.setValue(_defineLinearSpecularReflection, reflectionTexture->linearSpecularLOD(), true);
  defines.setValue(_defineOppositeZ,
                   _scene->useRightHandedSystem() ? !reflectionTexture->invertZ :
                                                    reflectionTexture->invertZ,
                   true);

  defines.setValue("SPHERICAL_HARMONICS", useSphericalHarmonics, true);
  defines.setValue("GAMMAREFLECTION", reflectionTexture->gammaSpace(), true);
  defines.setValue("RGBDREFLECTION", reflectionTexture->isRGBD(), true);

  if (reflectionTexture && reflectionTexture->coordinatesMode() != TextureConstants::SKYBOX_MODE) {
    if (reflectionTexture->isCube()) {
      defines.setValue("USESPHERICALFROMREFLECTIONMAP", true);
      defines.setValue("USEIRRADIANCEMAP", false);
      if (forceIrradianceInFragment || _scene->getEngine()->getCaps().maxVaryingVectors <= 8) {
        defines.setValue("USESPHERICALINVERTEX", false);
      }
      else {
        defines.setValue("USESPHERICALINVERTEX", true);
      }
    }
  }
}

void ReflectionBlock::bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh,
                           SubMesh* subMesh)
{
  ReflectionTextureBaseBlock::bind(effect, nodeMaterial, mesh);

  const auto reflectionTexture = _getTexture();

  if (!reflectionTexture || !subMesh) {
    return;
  }

  if (reflectionTexture->isCube()) {
    effect->setTexture(_cubeSamplerName, reflectionTexture);
  }
  else {
    effect->setTexture(_2DSamplerName, reflectionTexture);
  }

  const auto width = static_cast<float>(reflectionTexture->getSize().width);

  effect->setFloat3(_vReflectionMicrosurfaceInfosName, width,
                    reflectionTexture->lodGenerationScale(),
                    reflectionTexture->lodGenerationOffset());
  effect->setFloat2(_vReflectionFilteringInfoName, width, Scalar::Log2(width));

  auto defines = std::static_pointer_cast<NodeMaterialDefines>(subMesh->_materialDefines);

  const auto polynomials = reflectionTexture->sphericalPolynomial();
  if ((*defines)["USESPHERICALFROMREFLECTIONMAP"] && polynomials) {
    if ((*defines)["SPHERICAL_HARMONICS"]) {
      const auto& preScaledHarmonics = polynomials->preScaledHarmonics();
      effect->setVector3("vSphericalL00", preScaledHarmonics.l00);
      effect->setVector3("vSphericalL1_1", preScaledHarmonics.l1_1);
      effect->setVector3("vSphericalL10", preScaledHarmonics.l10);
      effect->setVector3("vSphericalL11", preScaledHarmonics.l11);
      effect->setVector3("vSphericalL2_2", preScaledHarmonics.l2_2);
      effect->setVector3("vSphericalL2_1", preScaledHarmonics.l2_1);
      effect->setVector3("vSphericalL20", preScaledHarmonics.l20);
      effect->setVector3("vSphericalL21", preScaledHarmonics.l21);
      effect->setVector3("vSphericalL22", preScaledHarmonics.l22);
    }
    else {
      effect->setFloat3("vSphericalX", polynomials->x.x, polynomials->x.y, polynomials->x.z);
      effect->setFloat3("vSphericalY", polynomials->y.x, polynomials->y.y, polynomials->y.z);
      effect->setFloat3("vSphericalZ", polynomials->z.x, polynomials->z.y, polynomials->z.z);
      effect->setFloat3("vSphericalXX_ZZ", polynomials->xx.x - polynomials->zz.x,
                        polynomials->xx.y - polynomials->zz.y,
                        polynomials->xx.z - polynomials->zz.z);
      effect->setFloat3("vSphericalYY_ZZ", polynomials->yy.x - polynomials->zz.x,
                        polynomials->yy.y - polynomials->zz.y,
                        polynomials->yy.z - polynomials->zz.z);
      effect->setFloat3("vSphericalZZ", polynomials->zz.x, polynomials->zz.y, polynomials->zz.z);
      effect->setFloat3("vSphericalXY", polynomials->xy.x, polynomials->xy.y, polynomials->xy.z);
      effect->setFloat3("vSphericalYZ", polynomials->yz.x, polynomials->yz.y, polynomials->yz.z);
      effect->setFloat3("vSphericalZX", polynomials->zx.x, polynomials->zx.y, polynomials->zx.z);
    }
  }
}

std::string ReflectionBlock::handleVertexSide(NodeMaterialBuildState& state)
{
  auto code = ReflectionTextureBaseBlock::handleVertexSide(state);

  EmitFunctionFromIncludeOptions replaceOptions;
  replaceOptions.replaceStrings
    = {{R"(uniform vec3 vSphericalL00;[\s\S]*?uniform vec3 vSphericalL22;)", ""},
       {R"(uniform vec3 vSphericalX;[\s\S]*?uniform vec3 vSphericalZX;)", ""}};

  state._emitFunctionFromInclude("harmonicsFunctions", StringTools::printf("//%s", name().c_str()),
                                 replaceOptions);

  const auto reflectionVectorName = state._getFreeVariableName("reflectionVector");

  _vEnvironmentIrradianceName = state._getFreeVariableName("vEnvironmentIrradiance");

  state._emitVaryingFromString(
    _vEnvironmentIrradianceName, "vec3",
    "defined(USESPHERICALFROMREFLECTIONMAP) && defined(USESPHERICALINVERTEX)");

  state._emitUniformFromString("vSphericalL00", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL1_1", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL10", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL11", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL2_2", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL2_1", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL20", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL21", "vec3", "SPHERICAL_HARMONICS");
  state._emitUniformFromString("vSphericalL22", "vec3", "SPHERICAL_HARMONICS");

  state._emitUniformFromString("vSphericalX", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalY", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalZ", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalXX_ZZ", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalYY_ZZ", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalZZ", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalXY", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalYZ", "vec3", "SPHERICAL_HARMONICS", true);
  state._emitUniformFromString("vSphericalZX", "vec3", "SPHERICAL_HARMONICS", true);

  code += StringTools::printf(
    R"(
       #if defined(USESPHERICALFROMREFLECTIONMAP) && defined(USESPHERICALINVERTEX)
          vec3 %s = vec3(%s * vec4(normalize(%s).xyz, 0)).xyz;
          #ifdef %s
              %s.z *= -1.0;
          #endif
          %s = computeEnvironmentIrradiance(%s);
      #endif\r\n
     )",
    reflectionVectorName.c_str(), _reflectionMatrixName.c_str(),
    worldNormal()->associatedVariableName().c_str(), //
    _defineOppositeZ.c_str(),                        //
    reflectionVectorName.c_str(),                    //
    _vEnvironmentIrradianceName.c_str(), reflectionVectorName.c_str());

  return code;
}

std::string ReflectionBlock::getCode(NodeMaterialBuildState& state,
                                     const std::string& normalVarName)
{
  std::string code = "";

  handleFragmentSideInits(state);

  const auto iComment = StringTools::printf("//%s", name().c_str());

  EmitFunctionFromIncludeOptions replaceOptions;
  replaceOptions.replaceStrings
    = {{R"(uniform vec3 vSphericalL00;[\s\S]*?uniform vec3 vSphericalL22;)", ""},
       {R"(uniform vec3 vSphericalX;[\s\S]*?uniform vec3 vSphericalZX;)", ""}};
  state._emitFunctionFromInclude("harmonicsFunctions", iComment.c_str(), replaceOptions);

  state._emitFunction("sampleReflection",
                      StringTools::printf(
                        R"(
                            #ifdef %s
                                #define sampleReflection(s, c) textureCube(s, c)
                            #else
                                #define sampleReflection(s, c) texture2D(s, c)
                            #endif\r\n
                          )",
                        _define3DName.c_str()),
                      iComment.c_str());

  state._emitFunction("sampleReflectionLod",
                      StringTools::printf(
                        R"(
                            #ifdef %s
                                #define sampleReflectionLod(s, c, l) textureCubeLodEXT(s, c, l)
                            #else
                                #define sampleReflectionLod(s, c, l) texture2DLodEXT(s, c, l)
                            #endif\r\n
                          )",
                        _define3DName.c_str()),
                      iComment.c_str());

  const auto computeReflectionCoordsFunc = StringTools::printf(
    R"(
        vec3 computeReflectionCoordsPBR(vec4 worldPos, vec3 worldNormal) {
            %s
            return %s;
        }\r\n
      )",
    handleFragmentSideCodeReflectionCoords("worldNormal", "worldPos", true).c_str(),
    _reflectionVectorName.c_str());

  state._emitFunction("computeReflectionCoordsPBR", computeReflectionCoordsFunc, iComment.c_str());

  _vReflectionMicrosurfaceInfosName = state._getFreeVariableName("vReflectionMicrosurfaceInfos");

  state._emitUniformFromString(_vReflectionMicrosurfaceInfosName, "vec3");

  _vReflectionInfosName = state._getFreeVariableName("vReflectionInfos");

  _vReflectionFilteringInfoName = state._getFreeVariableName("vReflectionFilteringInfo");

  state._emitUniformFromString(_vReflectionFilteringInfoName, "vec2");

  code += StringTools::printf(
    R"(
      #ifdef REFLECTION
        vec2 %s = vec2(1., 0.);

        reflectionOutParams reflectionOut;

        reflectionBlock(
            %s,
            %s,
            alphaG,
            %s,
            %s,
            %s,
        #ifdef ANISOTROPIC
            anisotropicOut,
        #endif
        #if defined(%s) && !defined(%s)
            NdotVUnclamped,
        #endif
        #ifdef %s
            roughness,
        #endif
        #ifdef %s
            %s,
        #else
            %s,
        #endif
        #if defined(NORMAL) && defined(USESPHERICALINVERTEX)
            %s,
        #endif
        #ifdef USESPHERICALFROMREFLECTIONMAP
            #if !defined(NORMAL) || !defined(USESPHERICALINVERTEX)
                %s,
            #endif
        #endif
        #ifdef USEIRRADIANCEMAP
            irradianceSampler, // ** not handled **
        #endif
        #ifndef LODBASEDMICROSFURACE
            #ifdef %s
                %s,
                %s,
            #else
                %s,
                %s,
            #endif
        #endif
            reflectionOut
        );
      #endif\r\n
     )",
    _vReflectionInfosName.c_str(),                                                              //
    StringTools::printf("v_%s.xyz", worldPosition()->associatedVariableName().c_str()).c_str(), //
    normalVarName.c_str(),                                                                      //
    _vReflectionMicrosurfaceInfosName.c_str(),                                                  //
    _vReflectionInfosName.c_str(),                                                              //
    reflectionColor().c_str(),                                                                  //
    _defineLODReflectionAlpha.c_str(), _defineSkyboxName.c_str(),                               //
    _defineLinearSpecularReflection.c_str(),                                                    //
    _define3DName.c_str(),                                                                      //
    _cubeSamplerName.c_str(),                                                                   //
    _2DSamplerName.c_str(),                                                                     //
    _vEnvironmentIrradianceName.c_str(),                                                        //
    _reflectionMatrixName.c_str(),                                                              //
    _define3DName.c_str(),                                                                      //
    _cubeSamplerName.c_str(),                                                                   //
    _cubeSamplerName.c_str(),
    _2DSamplerName.c_str(), //
    _2DSamplerName.c_str()  //
  );

  return code;
}

ReflectionBlock& ReflectionBlock::_buildBlock(NodeMaterialBuildState& state)
{
  _scene = state.sharedData->scene;

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    _defineLODReflectionAlpha       = state._getFreeDefineName("LODINREFLECTIONALPHA");
    _defineLinearSpecularReflection = state._getFreeDefineName("LINEARSPECULARREFLECTION");
  }

  return *this;
}

std::string ReflectionBlock::_dumpPropertiesCode()
{
  auto codeString = ReflectionTextureBaseBlock::_dumpPropertiesCode();

  codeString += StringTools::printf("%s.useSphericalHarmonics = %s;\r\n", _codeVariableName.c_str(),
                                    useSphericalHarmonics ? "true" : "false");
  codeString
    += StringTools::printf("%s.forceIrradianceInFragment = %s;\r\n", _codeVariableName.c_str(),
                           forceIrradianceInFragment ? "true" : "false");

  return codeString;
}

json ReflectionBlock::serialize() const
{
  return nullptr;
}

void ReflectionBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                   const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
