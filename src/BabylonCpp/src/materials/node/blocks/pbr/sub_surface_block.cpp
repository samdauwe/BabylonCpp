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
    , thickness{this, &SubSurfaceBlock::get_thickness}
    , tintColor{this, &SubSurfaceBlock::get_tintColor}
    , translucencyIntensity{this, &SubSurfaceBlock::get_translucencyIntensity}
    , translucencyDiffusionDist{this, &SubSurfaceBlock::get_translucencyDiffusionDist}
    , refraction{this, &SubSurfaceBlock::get_refraction}
    , subsurface{this, &SubSurfaceBlock::get_subsurface}
{
  _isUnique = true;
}

SubSurfaceBlock::~SubSurfaceBlock() = default;

void SubSurfaceBlock::RegisterConnections(const SubSurfaceBlockPtr& subSurfaceBlock)
{
  subSurfaceBlock->registerInput("thickness", NodeMaterialBlockConnectionPointTypes::Float, false,
                                 NodeMaterialBlockTargets::Fragment);
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

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_thickness()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_tintColor()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_translucencyIntensity()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_translucencyDiffusionDist()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_refraction()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& SubSurfaceBlock::get_subsurface()
{
  return _outputs[0];
}

void SubSurfaceBlock::autoConfigure(const NodeMaterialPtr& /*material*/)
{
  if (!thickness()->isConnected()) {
    auto thicknessInput
      = InputBlock::New("SubSurface thickness", NodeMaterialBlockTargets::Fragment,
                        NodeMaterialBlockConnectionPointTypes::Float);
    thicknessInput->value = std::make_shared<AnimationValue>(0.f);
    thicknessInput->output()->connectTo(thickness);
  }
}

void SubSurfaceBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                     NodeMaterialDefines& defines, bool /*useInstances*/,
                                     SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  const auto translucencyEnabled
    = translucencyDiffusionDist()->isConnected() || translucencyIntensity()->isConnected();

  defines.setValue("SUBSURFACE", translucencyEnabled || refraction()->isConnected(), true);
  defines.setValue("SS_TRANSLUCENCY", translucencyEnabled, true);
  defines.setValue("SS_THICKNESSANDMASK_TEXTURE", false, true);
  defines.setValue("SS_MASK_FROM_THICKNESS_TEXTURE", false, true);
  defines.setValue("SS_MASK_FROM_THICKNESS_TEXTURE_GLTF", false, true);
}

std::string SubSurfaceBlock::GetCode(NodeMaterialBuildState& state,
                                     const SubSurfaceBlockPtr& ssBlock,
                                     const ReflectionBlockPtr& reflectionBlock,
                                     const std::string& worldPosVarName)
{
  std::string code = "";

  const auto thickness             = ssBlock && ssBlock->thickness()->isConnected() ?
                                       ssBlock->thickness()->associatedVariableName() :
                                       "0.";
  const auto tintColor             = ssBlock && ssBlock->tintColor()->isConnected() ?
                                       ssBlock->tintColor()->associatedVariableName() :
                                       "vec3(1.)";
  const auto translucencyIntensity = ssBlock && ssBlock->translucencyIntensity()->isConnected() ?
                                       ssBlock->translucencyIntensity()->associatedVariableName() :
                                       "1.";
  const auto translucencyDiffusionDistance
    = ssBlock && ssBlock->translucencyDiffusionDist()->isConnected() ?
        ssBlock->translucencyDiffusionDist()->associatedVariableName() :
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
  const auto refractionView      = refractionBlock->view()->isConnected() ?
                                     refractionBlock->view()->associatedVariableName() :
                                     "";

  code += refractionBlock->getCode(state);

  code += StringTools::printf(
    R"(subSurfaceOutParams subSurfaceOut;

    #ifdef SUBSURFACE
        vec2 vThicknessParam = vec2(0., %s);
        vec4 vTintColor = vec4(%s, %s);
        vec3 vSubSurfaceIntensity = vec3(%s, %s, 0.);

        subSurfaceBlock(
            vSubSurfaceIntensity,
            vThicknessParam,
            vTintColor,
            normalW,
            specularEnvironmentReflectance,
        #ifdef SS_THICKNESSANDMASK_TEXTURE
            vec4(0.),
        #endif
        #ifdef REFLECTION
            #ifdef SS_TRANSLUCENCY
                %s,
                #ifdef USESPHERICALFROMREFLECTIONMAP
                    #if !defined(NORMAL) || !defined(USESPHERICALINVERTEX)
                        reflectionOut.irradianceVector,
                    #endif
                    #if defined(REALTIME_FILTERING)
                        %s,
                        %s,
                    #endif
                #endif
                #ifdef USEIRRADIANCEMAP
                    irradianceSampler,
                #endif
            #endif
        #endif
        #if defined(SS_REFRACTION) || defined(SS_TRANSLUCENCY)
            surfaceAlbedo,
        #endif
        #ifdef SS_REFRACTION
            %s.xyz,
            viewDirectionW,
            %s,
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
            #endif
            alphaG,
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
            #ifdef REALTIME_FILTERING
                %s,
            #endif
            #ifdef SS_USE_LOCAL_REFRACTIONMAP_CUBIC
                vRefractionPosition,
                vRefractionSize,
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
    thickness.c_str(),                                          //
    tintColor.c_str(), refractionTintAtDistance.c_str(),        //
    refractionIntensity.c_str(), translucencyIntensity.c_str(), //
    reflectionBlock->_reflectionMatrixName.c_str(),             //
    reflectionBlock->_cubeSamplerName.c_str(),                  //
    reflectionBlock->_vReflectionFilteringInfoName.c_str(),     //
    worldPosVarName.c_str(),                                    //
    refractionView.c_str(),                                     //
    refractionBlock->_vRefractionInfosName.c_str(),             //
    refractionBlock->_refractionMatrixName.c_str(),             //
    refractionBlock->_vRefractionMicrosurfaceInfosName.c_str(), //
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
    refractionBlock ? refractionBlock->_vRefractionFilteringInfoName.c_str() : "",               //
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
