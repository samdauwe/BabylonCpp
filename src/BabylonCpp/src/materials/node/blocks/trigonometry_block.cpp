#include <babylon/materials/node/blocks/trigonometry_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

TrigonometryBlock::TrigonometryBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , operation{TrigonometryBlockOperations::Cos}
    , input{this, &TrigonometryBlock::get_input}
    , output{this, &TrigonometryBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("output", NodeMaterialBlockConnectionPointTypes::Float);
}

TrigonometryBlock::~TrigonometryBlock()
{
}

const std::string TrigonometryBlock::getClassName() const
{
  return "TrigonometryBlock";
}

NodeMaterialConnectionPointPtr& TrigonometryBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& TrigonometryBlock::get_output()
{
  return _outputs[0];
}

TrigonometryBlock& TrigonometryBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];
  std::string _operation{""};

  switch (operation) {
    case TrigonometryBlockOperations::Cos: {
      _operation = "cos";
      break;
    }
    case TrigonometryBlockOperations::Sin: {
      _operation = "sin";
      break;
    }
    case TrigonometryBlockOperations::Abs: {
      _operation = "abs";
      break;
    }
    case TrigonometryBlockOperations::Exp: {
      _operation = "exp";
      break;
    }
    case TrigonometryBlockOperations::Exp2: {
      _operation = "exp2";
      break;
    }
    case TrigonometryBlockOperations::Round: {
      _operation = "round";
      break;
    }
    case TrigonometryBlockOperations::Floor: {
      _operation = "floor";
      break;
    }
    case TrigonometryBlockOperations::Ceiling: {
      _operation = "ceil";
      break;
    }
  }

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s(%s);\r\n", _operation.c_str(),
                        input()->associatedVariableName().c_str());

  return *this;
}

json TrigonometryBlock::serialize() const
{
  return nullptr;
}

void TrigonometryBlock::_deserialize(const json& /*serializationObject*/,
                                     Scene* /*scene*/,
                                     const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
