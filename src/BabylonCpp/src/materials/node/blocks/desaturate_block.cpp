#include <babylon/materials/node/blocks/desaturate_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

DesaturateBlock::DesaturateBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , color{this, &DesaturateBlock::get_color}
    , level{this, &DesaturateBlock::get_level}
    , output{this, &DesaturateBlock::get_output}
{
  registerInput("color", NodeMaterialBlockConnectionPointTypes::Color3);
  registerInput("level", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Color3);
}

DesaturateBlock::~DesaturateBlock() = default;

std::string DesaturateBlock::getClassName() const
{
  return "DesaturateBlock";
}

NodeMaterialConnectionPointPtr& DesaturateBlock::get_color()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& DesaturateBlock::get_level()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& DesaturateBlock::get_output()
{
  return _outputs[0];
}

DesaturateBlock& DesaturateBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput  = _outputs[0];
  const auto& iColor   = color();
  const auto colorName = iColor->associatedVariableName();
  const auto tempMin   = state._getFreeVariableName("colorMin");
  const auto tempMax   = state._getFreeVariableName("colorMax");
  const auto tempMerge = state._getFreeVariableName("colorMerge");

  state.compilationString
    += StringTools::printf("float %s = min(min(%s.x, %s.y), %s.z);\r\n", tempMin.c_str(),
                           colorName.c_str(), colorName.c_str(), colorName.c_str());
  state.compilationString
    += StringTools::printf("float %s = max(max(%s.x, %s.y), %s.z);\r\n", tempMax.c_str(),
                           colorName.c_str(), colorName.c_str(), colorName.c_str());
  state.compilationString += StringTools::printf(
    "float %s = 0.5 * (%s + %s);\r\n", tempMerge.c_str(), tempMin.c_str(), tempMax.c_str());
  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = mix(%s, vec3(%s, %s, %s), %s);\r\n", colorName.c_str(),
                             tempMerge.c_str(), tempMerge.c_str(), tempMerge.c_str(),
                             level()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
