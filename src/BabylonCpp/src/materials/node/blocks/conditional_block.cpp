#include <babylon/materials/node/blocks/conditional_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ConditionalBlock::ConditionalBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , condition{ConditionalBlockConditions::LessThan}
    , a{this, &ConditionalBlock::get_a}
    , b{this, &ConditionalBlock::get_b}
    , _true{this, &ConditionalBlock::get__true}
    , _false{this, &ConditionalBlock::get__false}
    , output{this, &ConditionalBlock::get_output}
{
  registerInput("a", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("b", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("true", NodeMaterialBlockConnectionPointTypes::AutoDetect, true);
  registerInput("false", NodeMaterialBlockConnectionPointTypes::AutoDetect, true);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _linkConnectionTypes(2, 3);
  _outputs[0]->_typeConnectionSource       = _inputs[2];
  _outputs[0]->_defaultConnectionPointType = NodeMaterialBlockConnectionPointTypes::Float;
}

ConditionalBlock::~ConditionalBlock() = default;

std::string ConditionalBlock::getClassName() const
{
  return "ConditionalBlock";
}

NodeMaterialConnectionPointPtr& ConditionalBlock::get_a()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ConditionalBlock::get_b()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ConditionalBlock::get__true()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ConditionalBlock::get__false()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ConditionalBlock::get_output()
{
  return _outputs[0];
}

ConditionalBlock& ConditionalBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto trueStatement  = _true()->isConnected() ? _true()->associatedVariableName() : "1.0";
  const auto falseStatement = _false()->isConnected() ? _false()->associatedVariableName() : "0.0";

  switch (condition) {
    case ConditionalBlockConditions::Equal: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = %s == %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::NotEqual: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = %s != %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::LessThan: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(" = %s < %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
                                 b()->associatedVariableName().c_str(), trueStatement.c_str(),
                                 falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::LessOrEqual: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = %s <= %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::GreaterThan: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(" = %s > %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
                                 b()->associatedVariableName().c_str(), trueStatement.c_str(),
                                 falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::GreaterOrEqual: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = %s >= %s ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::Xor: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = (mod(%s + %s, 2.0) > 0.0) ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::Or: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = (min(%s + %s, 1.0) > 0.0) ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
    case ConditionalBlockConditions::And: {
      state.compilationString
        += _declareOutput(output, state)
           + StringTools::printf(
             " = (%s * %s > 0.0)  ? %s : %s;\r\n", a()->associatedVariableName().c_str(),
             b()->associatedVariableName().c_str(), trueStatement.c_str(), falseStatement.c_str());
      break;
    }
  }

  return *this;
}

json ConditionalBlock::serialize() const
{
  return nullptr;
}

void ConditionalBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
}

std::string ConditionalBlock::_dumpPropertiesCode()
{
  static const auto conditionalBlockConditionsEnumToString
    = [](ConditionalBlockConditions iCondition) -> std::string {
    std::string conditionAsString;
    switch (iCondition) {
      case ConditionalBlockConditions::Equal: {
        conditionAsString = "Equal";
        break;
      }
      case ConditionalBlockConditions::NotEqual: {
        conditionAsString = "NotEqual";
        break;
      }
      case ConditionalBlockConditions::LessThan: {
        conditionAsString = "LessThan";
        break;
      }
      case ConditionalBlockConditions::LessOrEqual: {
        conditionAsString = "LessOrEqual";
        break;
      }
      case ConditionalBlockConditions::GreaterThan: {
        conditionAsString = "GreaterThan";
        break;
      }
      case ConditionalBlockConditions::GreaterOrEqual: {
        conditionAsString = "GreaterOrEqual";
        break;
      }
      case ConditionalBlockConditions::Xor: {
        conditionAsString = "Xor";
        break;
      }
      case ConditionalBlockConditions::Or: {
        conditionAsString = "Or";
        break;
      }
      case ConditionalBlockConditions::And: {
        conditionAsString = "And";
        break;
      }
    }
    return conditionAsString;
  };

  const auto codeString = StringTools::printf(
    "%s.operation = ConditionalBlockConditions::%s;\r\n", _codeVariableName.c_str(),
    conditionalBlockConditionsEnumToString(condition).c_str());
  return codeString;
}

} // end of namespace BABYLON
