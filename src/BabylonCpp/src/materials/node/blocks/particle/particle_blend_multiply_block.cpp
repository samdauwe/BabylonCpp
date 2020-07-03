#include <babylon/materials/node/blocks/particle/particle_blend_multiply_block.h>

#include <babylon/materials/node/enums/node_material_block_targets.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ParticleBlendMultiplyBlock::ParticleBlendMultiplyBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , color{this, &ParticleBlendMultiplyBlock::get_color}
    , alphaTexture{this, &ParticleBlendMultiplyBlock::get_alphaTexture}
    , alphaColor{this, &ParticleBlendMultiplyBlock::get_alphaColor}
    , blendColor{this, &ParticleBlendMultiplyBlock::get_blendColor}
{
  _isUnique = true;

  registerInput("color", NodeMaterialBlockConnectionPointTypes::Color4, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("alphaTexture", NodeMaterialBlockConnectionPointTypes::Float, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("alphaColor", NodeMaterialBlockConnectionPointTypes::Float, false,
                NodeMaterialBlockTargets::Fragment);

  registerOutput("blendColor", NodeMaterialBlockConnectionPointTypes::Color4,
                 NodeMaterialBlockTargets::Fragment);
}

ParticleBlendMultiplyBlock::~ParticleBlendMultiplyBlock() = default;

std::string ParticleBlendMultiplyBlock::getClassName() const
{
  return "ParticleBlendMultiplyBlock";
}

NodeMaterialConnectionPointPtr& ParticleBlendMultiplyBlock::get_color()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ParticleBlendMultiplyBlock::get_alphaTexture()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ParticleBlendMultiplyBlock::get_alphaColor()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ParticleBlendMultiplyBlock::get_blendColor()
{
  return _outputs[0];
}

void ParticleBlendMultiplyBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("sourceAlpha");
}

ParticleBlendMultiplyBlock& ParticleBlendMultiplyBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    return *this;
  }

  state.compilationString += StringTools::printf(
    R"(
     #ifdef BLENDMULTIPLYMODE
         %s;
         float sourceAlpha = %s * %s;
         %s.rgb = %s.rgb * sourceAlpha + vec3(1.0) * (1.0 - sourceAlpha);
         %s.a = %s.a;
     #else
         %s = %s;
     #endif
    )",
    _declareOutput(blendColor, state).c_str(), alphaColor()->associatedVariableName().c_str(),
    alphaTexture()->associatedVariableName().c_str(),
    blendColor()->associatedVariableName().c_str(), color()->associatedVariableName().c_str(),
    blendColor()->associatedVariableName().c_str(), color()->associatedVariableName().c_str(),
    _declareOutput(blendColor, state).c_str(), color()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
