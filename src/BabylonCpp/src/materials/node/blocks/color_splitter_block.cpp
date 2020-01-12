#include <babylon/materials/node/blocks/color_splitter_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ColorSplitterBlock::ColorSplitterBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , rgba{this, &ColorSplitterBlock::get_rgba}
    , rgbIn{this, &ColorSplitterBlock::get_rgbIn}
    , rgbOut{this, &ColorSplitterBlock::get_rgbOut}
    , r{this, &ColorSplitterBlock::get_r}
    , g{this, &ColorSplitterBlock::get_g}
    , b{this, &ColorSplitterBlock::get_b}
    , a{this, &ColorSplitterBlock::get_a}
{
  registerInput("rgba", NodeMaterialBlockConnectionPointTypes::Color4, true);
  registerInput("rgb", NodeMaterialBlockConnectionPointTypes::Color3, true);

  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("a", NodeMaterialBlockConnectionPointTypes::Float);
}

ColorSplitterBlock::~ColorSplitterBlock() = default;

std::string ColorSplitterBlock::getClassName() const
{
  return "ColorSplitterBlock";
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_rgba()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_rgbIn()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_rgbOut()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_r()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_g()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_b()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& ColorSplitterBlock::get_a()
{
  return _outputs[4];
}

std::string ColorSplitterBlock::_inputRename(const std::string& iName)
{
  if (iName == "rgb ") {
    return "rgbIn";
  }
  return iName;
}

std::string ColorSplitterBlock::_outputRename(const std::string& iName)
{
  if (iName == "rgb") {
    return "rgbOut";
  }
  return iName;
}

ColorSplitterBlock& ColorSplitterBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& input = rgba()->isConnected() ? rgba() : rgbIn();

  if (!input->isConnected()) {
    return *this;
  }

  const auto& rgbOutput = _outputs[0];
  const auto& rOutput   = _outputs[1];
  const auto& gOutput   = _outputs[2];
  const auto& bOutput   = _outputs[3];
  const auto& aOutput   = _outputs[4];

  if (rgbOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(rgbOutput, state)
         + StringTools::printf(" = %s.rgb;\r\n", input->associatedVariableName().c_str());
  }
  if (rOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(rOutput, state)
         + StringTools::printf(" = %s.r;\r\n", input->associatedVariableName().c_str());
  }
  if (gOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(gOutput, state)
         + StringTools::printf(" = %s.g;\r\n", input->associatedVariableName().c_str());
  }
  if (bOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(bOutput, state)
         + StringTools::printf(" = %s.b;\r\n", input->associatedVariableName().c_str());
  }
  if (aOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(aOutput, state)
         + StringTools::printf(" = %s.a;\r\n", input->associatedVariableName().c_str());
  }

  return *this;
}

} // end of namespace BABYLON
