#include <babylon/materials/node/blocks/pbr/sub_surface_block.h>

#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/blocks/pbr/reflection_block.h>
#include <babylon/materials/node/blocks/pbr/refraction_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

SubSurfaceBlock::SubSurfaceBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , useMaskFromThicknessTexture{false}
    , minThickness{this, &SubSurfaceBlock::get_minThickness}
    , maxThickness{this, &SubSurfaceBlock::get_maxThickness}
    , thicknessTexture{this, &SubSurfaceBlock::get_thicknessTexture}
    , tintColor{this, &SubSurfaceBlock::get_tintColor}
    , translucencyIntensity{this, &SubSurfaceBlock::get_translucencyIntensity}
    , translucencyDiffusionDistance{this, &SubSurfaceBlock::get_translucencyDiffusionDistance}
    , refraction{this, &SubSurfaceBlock::get_refraction}
    , subsurface{this, &SubSurfaceBlock::get_subsurface}
{
  _isUnique = true;
}

SubSurfaceBlock::~SubSurfaceBlock() = default;

void SubSurfaceBlock::RegisterConnections(const SubSurfaceBlockPtr& subSurfaceBlock)
{
  subSurfaceBlock->registerInput("minThickness", NodeMaterialBlockConnectionPointTypes::Float,
                                 false, NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput("maxThickness", NodeMaterialBlockConnectionPointTypes::Float, true,
                                 NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput("thicknessTexture", NodeMaterialBlockConnectionPointTypes::Color4,
                                 true, NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput("tintColor", NodeMaterialBlockConnectionPointTypes::Color3, true,
                                 NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput("translucencyIntensity",
                                 NodeMaterialBlockConnectionPointTypes::Float, true,
                                 NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput("translucencyDiffusionDistance",
                                 NodeMaterialBlockConnectionPointTypes::Color3, true,
                                 NodeMaterialBlockTargets::Fragment);
  subSurfaceBlock->registerInput(
    "refraction", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "refraction", subSurfaceBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> RefractionBlockPtr { return RefractionBlock::New(iName); },
      "RefractionBlock"));

  subSurfaceBlock->registerOutput(
    "subsurface", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "subsurface", subSurfaceBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> SubSurfaceBlockPtr { return SubSurfaceBlock::New(iName); },
      "SubSurfaceBlock"));
}

void SubSurfaceBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("subSurfaceOut");
  state._excludeVariableName("vThicknessParam");
  state._excludeVariableName("vTintColor");
  state._excludeVariableName("vSubSurfaceIntensity");
}

std::string SubSurfaceBlock::getClassName() const
{
  return "SubSurfaceBlock";
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_minThickness()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_maxThickness()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_thicknessTexture()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_tintColor()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_translucencyIntensity()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_translucencyDiffusionDistance()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_refraction()
{
  return _inputs[6];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_subsurface()
{
  return _outputs[0];
}

void SubSurfaceBlock::autoConfigure(const NodeMaterialPtr& /*material*/)
{
  if (!minThickness()->isConnected()) {
    auto minThicknessInput
      = InputBlock::New("SubSurface min thickness", NodeMaterialBlockTargets::Fragment,
                        NodeMaterialBlockConnectionPointTypes::Float);
    minThicknessInput->value = 0;
    minThicknessInput->output()->connectTo(minThickness);
  }
}

void SubSurfaceBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                     NodeMaterialDefines& defines, bool /*useInstances*/,
                                     SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  const auto translucencyEnabled
    = translucencyDiffusionDistance()->isConnected() || translucencyIntensity()->isConnected();

  defines.setValue("SUBSURFACE", translucencyEnabled || refraction()->isConnected(), true);
  defines.setValue("SS_TRANSLUCENCY", translucencyEnabled, true);
  defines.setValue("SS_THICKNESSANDMASK_TEXTURE", thicknessTexture()->isConnected(), true);
  defines.setValue("SS_MASK_FROM_THICKNESS_TEXTURE", useMaskFromThicknessTexture, true);
}

std::string SubSurfaceBlock::GetCode(NodeMaterialBuildState& state,
                                     const SubSurfaceBlockPtr& ssBlock,
                                     const ReflectionBlockPtr& reflectionBlock,
                                     const std::string& worldPosVarName)
{
  std::string code = "";

  const auto minThickness = ssBlock->minThickness()->isConnected() ?
                              ssBlock->minThickness()->associatedVariableName() :
                              "0.";
  const auto maxThickness = ssBlock->maxThickness()->isConnected() ?
                              ssBlock->maxThickness()->associatedVariableName() :
                              "1.";
  const auto thicknessTexture = ssBlock->thicknessTexture()->isConnected() ?
                                  ssBlock->thicknessTexture()->associatedVariableName() :
                                  "vec4(0.)";
  const auto tintColor = ssBlock->tintColor()->isConnected() ?
                           ssBlock->tintColor()->associatedVariableName() :
                           "vec3(1.)";
  const auto translucencyIntensity = ssBlock->translucencyIntensity()->isConnected() ?
                                       ssBlock->translucencyIntensity()->associatedVariableName() :
                                       "1.";
  const auto translucencyDiffusionDistance
    = ssBlock->translucencyDiffusionDistance()->isConnected() ?
        ssBlock->translucencyDiffusionDistance()->associatedVariableName() :
        "vec3(1.)";

  RefractionBlockPtr refractionBlock = nullptr;
  if (ssBlock->refraction()->isConnected()) {
    refractionBlock = std::static_pointer_cast<RefractionBlock>(
      ssBlock->refraction()->connectedPoint()->ownerBlock());
  }

  const auto refractionTintAtDistance
    = refractionBlock->tintAtDistance()->isConnected() ?
        refractionBlock->tintAtDistance()->associatedVariableName() :
        "1.";
  const auto refractionIntensity = refractionBlock->intensity()->isConnected() ?
                                     refractionBlock->intensity()->associatedVariableName() :
                                     "1.";
  const auto refractionView = refractionBlock->view()->isConnected() ?
                                refractionBlock->view()->associatedVariableName() :
                                "";

  code += refractionBlock->getCode(state);

  code += StringTools::printf(
    R"(subSurfaceOutParams subSurfaceOut;

    #ifdef SUBSURFACE
        vec2 vThicknessParam = vec2(%s, %s - %s);
        vec4 vTintColor = vec4(%s, %s);
        vec3 vSubSurfaceIntensity = vec3(%s, %s, 0.);

        subSurfaceBlock(
            vSubSurfaceIntensity,
            vThicknessParam,
            vTintColor,
            normalW,
            specularEnvironmentReflectance,
        #ifdef SS_THICKNESSANDMASK_TEXTURE
            %s,
        #endif
        #ifdef REFLECTION
            #ifdef SS_TRANSLUCENCY
                %s,
                #ifdef USESPHERICALFROMREFLECTIONMAP
                    #if !defined(NORMAL) || !defined(USESPHERICALINVERTEX)
                        reflectionOut.irradianceVector,
                    #endif
                #endif
                #ifdef USEIRRADIANCEMAP
                    irradianceSampler,
                #endif
            #endif
        #endif
        #ifdef SS_REFRACTION
            %s.xyz,
            viewDirectionW,
            %s,
            surfaceAlbedo,
            %s,
            %s,
            %s,
            vLightingIntensity,
            #ifdef SS_LINKREFRACTIONTOTRANSPARENCY
                alpha,
            #endif
            #ifdef %s
                NdotVUnclamped,
            #endif
            #ifdef %s
                roughness,
            #else
                alphaG,
            #endif
            #ifdef %s
                %s,
            #else
                %s,
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
            #ifdef ANISOTROPIC
                anisotropicOut,
            #endif
        #endif
        #ifdef SS_TRANSLUCENCY
            %s,
        #endif
            subSurfaceOut
        );

        #ifdef SS_REFRACTION
            surfaceAlbedo = subSurfaceOut.surfaceAlbedo;
            #ifdef SS_LINKREFRACTIONTOTRANSPARENCY
                alpha = subSurfaceOut.alpha;
            #endif
        #endif
    #else
        subSurfaceOut.specularEnvironmentReflectance = specularEnvironmentReflectance;
    #endif\r\n
   )",
    minThickness.c_str(), maxThickness.c_str(), minThickness.c_str(), //
    tintColor.c_str(), refractionTintAtDistance.c_str(),              //
    refractionIntensity.c_str(), translucencyIntensity.c_str(),       //
    thicknessTexture.c_str(),                                         //
    reflectionBlock->_reflectionMatrixName.c_str(),                   //
    worldPosVarName.c_str(),                                          //
    refractionView.c_str(),                                           //
    refractionBlock->_vRefractionInfosName.c_str(),                   //
    refractionBlock->_refractionMatrixName.c_str(),                   //
    refractionBlock->_vRefractionMicrosurfaceInfosName.c_str(),       //
    !refractionBlock->_defineLODRefractionAlpha.empty() ?
      refractionBlock->_defineLODRefractionAlpha.c_str() :
      "IGNORE", //
    !refractionBlock->_defineLinearSpecularRefraction.empty() ?
      refractionBlock->_defineLinearSpecularRefraction.c_str() :
      "IGNORE",                                                                                  //
    !refractionBlock->_define3DName.empty() ? refractionBlock->_define3DName.c_str() : "IGNORE", //
    refractionBlock->_cubeSamplerName.c_str(),                                                   //
    refractionBlock->_2DSamplerName.c_str(),                                                     //
    !refractionBlock->_define3DName.empty() ? refractionBlock->_define3DName.c_str() : "IGNORE", //
    refractionBlock->_cubeSamplerName.c_str(),                                                   //
    refractionBlock->_cubeSamplerName.c_str(),                                                   //
    refractionBlock->_2DSamplerName.c_str(),                                                     //
    refractionBlock->_2DSamplerName.c_str(),                                                     //
    translucencyDiffusionDistance.c_str()                                                        //
  );

  return code;
}

SubSurfaceBlock& SubSurfaceBlock::_buildBlock(NodeMaterialBuildState& state)
{
  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  return *this;
}

} // end of namespace BABYLON
