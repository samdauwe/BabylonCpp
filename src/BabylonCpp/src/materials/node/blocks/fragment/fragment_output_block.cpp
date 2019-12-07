#include <babylon/materials/node/blocks/fragment/fragment_output_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

FragmentOutputBlock::FragmentOutputBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment, true}
    , rgba{this, &FragmentOutputBlock::get_rgba}
    , rgb{this, &FragmentOutputBlock::get_rgb}
    , a{this, &FragmentOutputBlock::get_a}
{
  registerInput("rgba", NodeMaterialBlockConnectionPointTypes::Color4, true);
  registerInput("rgb", NodeMaterialBlockConnectionPointTypes::Color3, true);
  registerInput("a", NodeMaterialBlockConnectionPointTypes::Float, true);
}

FragmentOutputBlock::~FragmentOutputBlock()
{
}

std::string FragmentOutputBlock::getClassName() const
{
  return "FragmentOutputBlock";
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_rgba()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_rgb()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_a()
{
  return _inputs[2];
}

FragmentOutputBlock& FragmentOutputBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& _rgba                         = rgba();
  const auto& _rgb                          = rgb();
  const auto& _a                            = a();
  state.sharedData->hints.needAlphaBlending = _rgba->isConnected() || _a->isConnected();

  if (_rgba->connectedPoint()) {
    state.compilationString
      += String::printf("gl_FragColor = %s;\r\n", _rgba->associatedVariableName().c_str());
  }
  else if (_rgb->connectedPoint()) {
    if (_a->connectedPoint()) {
      state.compilationString += String::printf("gl_FragColor = vec4(%s, %s);\r\n",
                                                _rgb->associatedVariableName().c_str(),
                                                _a->associatedVariableName().c_str());
    }
    else {
      state.compilationString += String::printf("gl_FragColor = vec4(%s, 1.0);\r\n",
                                                _rgb->associatedVariableName().c_str());
    }
  }
  else {
    state.sharedData->checks.notConnectedNonOptionalInputs.emplace_back(_rgba);
  }

  return *this;
}

} // end of namespace BABYLON
