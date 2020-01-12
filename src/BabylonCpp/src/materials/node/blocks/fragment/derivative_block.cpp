#include <babylon/materials/node/blocks/fragment/derivative_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

DerivativeBlock::DerivativeBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , input{this, &DerivativeBlock::get_input}
    , dx{this, &DerivativeBlock::get_dx}
    , dy{this, &DerivativeBlock::get_dy}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect, false);
  registerOutput("dx", NodeMaterialBlockConnectionPointTypes::BasedOnInput);
  registerOutput("dy", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _outputs[1]->_typeConnectionSource = _inputs[0];
}

DerivativeBlock::~DerivativeBlock() = default;

std::string DerivativeBlock::getClassName() const
{
  return "DerivativeBlock";
}

NodeMaterialConnectionPointPtr& DerivativeBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& DerivativeBlock::get_dx()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& DerivativeBlock::get_dy()
{
  return _outputs[1];
}

DerivativeBlock& DerivativeBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iDx = _outputs[0];
  const auto& iDy = _outputs[1];

  state._emitExtension("derivatives", "#extension GL_OES_standard_derivatives : enable");

  if (iDx->hasEndpoints()) {
    state.compilationString
      += _declareOutput(iDx, state)
         + StringTools::printf(" = dFdx(%s);\r\n", input()->associatedVariableName().c_str());
  }

  if (iDy->hasEndpoints()) {
    state.compilationString
      += _declareOutput(iDy, state)
         + StringTools::printf(" = dFdy(%s);\r\n", input()->associatedVariableName().c_str());
  }

  return *this;
}

} // end of namespace BABYLON
