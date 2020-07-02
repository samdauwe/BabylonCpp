#include <babylon/materials/node/blocks/particle/particle_ramp_gradient_block.h>

#include <babylon/materials/node/enums/node_material_block_targets.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ParticleRampGradientBlock::ParticleRampGradientBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , color{this, &ParticleRampGradientBlock::get_color}
    , rampColor{this, &ParticleRampGradientBlock::get_rampColor}
{
  _isUnique = true;

  registerInput("color", NodeMaterialBlockConnectionPointTypes::Color4, false,
                NodeMaterialBlockTargets::Fragment);

  registerOutput("rampColor", NodeMaterialBlockConnectionPointTypes::Color4,
                 NodeMaterialBlockTargets::Fragment);
}

ParticleRampGradientBlock::~ParticleRampGradientBlock() = default;

std::string ParticleRampGradientBlock::getClassName() const
{
  return "ParticleRampGradientBlock";
}

NodeMaterialConnectionPointPtr& ParticleRampGradientBlock::get_color()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ParticleRampGradientBlock::get_rampColor()
{
  return _outputs[0];
}

void ParticleRampGradientBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("remapRanges");
  state._excludeVariableName("rampSampler");
  state._excludeVariableName("baseColor");
  state._excludeVariableName("alpha");
  state._excludeVariableName("remappedColorIndex");
  state._excludeVariableName("rampColor");
  state._excludeVariableName("finalAlpha");
}

ParticleRampGradientBlock& ParticleRampGradientBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    return *this;
  }

  state._emit2DSampler("rampSampler");
  state._emitVaryingFromString("remapRanges", "vec4", "RAMPGRADIENT");

  state.compilationString += StringTools::printf(
    R"(
     #ifdef RAMPGRADIENT
         vec4 baseColor = %s;
         float alpha = %s.a;

         float remappedColorIndex = clamp((alpha - remapRanges.x) / remapRanges.y, 0.0, 1.0);

         vec4 rampColor = texture2D(rampSampler, vec2(1.0 - remappedColorIndex, 0.));
         baseColor.rgb *= rampColor.rgb;

         // Remapped alpha
         float finalAlpha = baseColor.a;
         baseColor.a = clamp((alpha * rampColor.a - remapRanges.z) / remapRanges.w, 0.0, 1.0);

         %s = baseColor;
     #else
         %s = %s;
     #endif
    )",
    color()->associatedVariableName().c_str(), color()->associatedVariableName().c_str(),
    _declareOutput(rampColor, state).c_str(), _declareOutput(rampColor, state).c_str(),
    color()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
