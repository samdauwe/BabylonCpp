#include <babylon/materials/node/blocks/color_merger_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ColorMergerBlock::ColorMergerBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , rgbIn{this, &ColorMergerBlock::get_rgbIn}
    , r{this, &ColorMergerBlock::get_r}
    , g{this, &ColorMergerBlock::get_g}
    , b{this, &ColorMergerBlock::get_b}
    , a{this, &ColorMergerBlock::get_a}
    , rgba{this, &ColorMergerBlock::get_rgba}
    , rgbOut{this, &ColorMergerBlock::get_rgbOut}
{
  registerInput("rgb ", NodeMaterialBlockConnectionPointTypes::Color3, true);
  registerInput("r", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("g", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("b", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("a", NodeMaterialBlockConnectionPointTypes::Float, true);

  registerOutput("rgba", NodeMaterialBlockConnectionPointTypes::Color4);
  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3);
}

ColorMergerBlock::~ColorMergerBlock() = default;

std::string ColorMergerBlock::getClassName() const
{
  return "ColorMergerBlock";
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_rgbIn()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_r()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_g()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_b()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_a()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_rgba()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ColorMergerBlock::get_rgbOut()
{
  return _outputs[1];
}

std::string ColorMergerBlock::_inputRename(const std::string& name)
{
  if (name == "rgb ") {
    return "rgbIn";
  }
  return name;
}

ColorMergerBlock& ColorMergerBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& rInput   = r();
  const auto& gInput   = g();
  const auto& bInput   = b();
  const auto& aInput   = a();
  const auto& rgbInput = rgbIn();

  const auto& color4Output = _outputs[0];
  const auto& color3Output = _outputs[1];

  if (rgbInput->isConnected()) {
    if (color4Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(color4Output, state)
           + StringTools::printf(" = vec4(%s, %s);\r\n", rgbInput->associatedVariableName().c_str(),
                                 aInput->isConnected() ? _writeVariable(aInput).c_str() : "0.0");
    }

    if (color3Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(color3Output, state)
           + StringTools::printf(" = %s;\r\n", rgbInput->associatedVariableName().c_str());
    }
  }
  else {
    if (color4Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(color4Output, state)
           + StringTools::printf(" = vec4(%s, %s, %s, %s);\r\n",
                                 rInput->isConnected ? _writeVariable(rInput).c_str() : "0.0",
                                 gInput->isConnected ? _writeVariable(gInput).c_str() : "0.0",
                                 bInput->isConnected ? _writeVariable(bInput).c_str() : "0.0",
                                 aInput->isConnected ? _writeVariable(aInput).c_str() : "0.0");
    }

    if (color3Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(color3Output, state)
           + StringTools::printf(" = vec3(%s, %s, %s);\r\n",
                                 rInput->isConnected ? _writeVariable(rInput).c_str() : "0.0",
                                 gInput->isConnected ? _writeVariable(gInput).c_str() : "0.0",
                                 bInput->isConnected ? _writeVariable(bInput).c_str() : "0.0");
    }
  }

  return *this;
}

} // end of namespace BABYLON
