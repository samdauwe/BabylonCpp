#include <babylon/materials/node/blocks/trigonometry_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

TrigonometryBlock::TrigonometryBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , operation{TrigonometryBlockOperations::Cos}
    , input{this, &TrigonometryBlock::get_input}
    , output{this, &TrigonometryBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

TrigonometryBlock::~TrigonometryBlock() = default;

std::string TrigonometryBlock::getClassName() const
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

  const auto& iOutput = _outputs[0];
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
    case TrigonometryBlockOperations::Sqrt: {
      _operation = "sqrt";
      break;
    }
    case TrigonometryBlockOperations::Log: {
      _operation = "log";
      break;
    }
    case TrigonometryBlockOperations::Tan: {
      _operation = "tan";
      break;
    }
    case TrigonometryBlockOperations::ArcTan: {
      _operation = "atan";
      break;
    }
    case TrigonometryBlockOperations::ArcCos: {
      _operation = "acos";
      break;
    }
    case TrigonometryBlockOperations::ArcSin: {
      _operation = "asin";
      break;
    }
    case TrigonometryBlockOperations::Fract: {
      _operation = "fract";
      break;
    }
    case TrigonometryBlockOperations::Sign: {
      _operation = "sign";
      break;
    }
    case TrigonometryBlockOperations::Radians: {
      _operation = "radians";
      break;
    }
    case TrigonometryBlockOperations::Degrees: {
      _operation = "degrees";
      break;
    }
  }

  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = %s(%s);\r\n", _operation.c_str(),
                                                   input()->associatedVariableName().c_str());

  return *this;
}

json TrigonometryBlock::serialize() const
{
  return nullptr;
}

void TrigonometryBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                     const std::string& /*rootUrl*/)
{
}

std::string TrigonometryBlock::_dumpPropertiesCode()
{
  const auto codeString
    = StringTools::printf("%s.operation = static_cast<TrigonometryBlockOperations>(%d);\r\n",
                          _codeVariableName.c_str(), operation);
  return codeString;
}

} // end of namespace BABYLON
