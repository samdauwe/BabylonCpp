#include <babylon/engines/processors/expressions/shader_define_expression.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

std::unordered_map<std::string, uint32_t> ShaderDefineExpression::_OperatorPriority = {
  {")", 0},  //
  {"(", 1},  //
  {"||", 2}, //
  {"&&", 3}  //
};

std::vector<std::string> ShaderDefineExpression::_Stack
  = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

bool ShaderDefineExpression::isTrue(
  const std::unordered_map<std::string, std::string>& /*preprocessors*/) const
{
  return true;
}

std::string ShaderDefineExpression::postfixToInfix(const std::vector<std::string>& postfix)
{
  std::vector<std::string> stack;

  for (const auto& c : postfix) {
    if (!stl_util::contains(ShaderDefineExpression::_OperatorPriority, c)) {
      stack.emplace_back(c);
    }
    else {
      const auto &v1 = stack[stack.size() - 1], &v2 = stack[stack.size() - 2];

      stack.pop_back();
      stack.pop_back();
      stack.emplace_back(StringTools::printf("(%s%s%s)", v2.c_str(), c.c_str(), v1.c_str()));
    }
  }

  return stack.back();
}

std::vector<std::string> ShaderDefineExpression::infixToPostfix(const std::string& infix)
{
  std::vector<std::string> result;

  auto stackIdx = -1;

  const auto pushOperand = [](std::string& operand, std::vector<std::string>& result) -> void {
    operand = StringTools::trim(operand);
    if (operand != "") {
      result.emplace_back(operand);
      operand = "";
    }
  };

  const auto push = [](const std::string& s, int& stackIdx) -> void {
    if (stackIdx < static_cast<int>(ShaderDefineExpression::_Stack.size()) - 1) {
      ShaderDefineExpression::_Stack[static_cast<size_t>(++stackIdx)] = s;
    }
  };

  const auto& peek = [](int stackIdx) -> std::string {
    if (stackIdx >= 0) {
      auto _stackIdx = static_cast<size_t>(stackIdx);
      if (_stackIdx < ShaderDefineExpression::_Stack.size()) {
        return ShaderDefineExpression::_Stack[_stackIdx];
      }
    }
    return "";
  };

  const auto pop = [](int& stackIdx) -> std::string {
    if (stackIdx == -1) {
      return "!!INVALID EXPRESSION!!";
    }
    else {
      return ShaderDefineExpression::_Stack[static_cast<size_t>(stackIdx--)];
    }
  };

  size_t idx          = 0;
  std::string operand = "";

  while (idx < infix.size()) {
    const auto c      = std::to_string(infix[idx]);
    std::string token = idx < infix.size() - 1 ? infix.substr(idx, 2) : "";

    if (c == "(") {
      operand = "";
      push(c, stackIdx);
    }
    else if (c == ")") {
      pushOperand(operand, result);
      while (stackIdx != -1 && peek(stackIdx) != "(") {
        result.emplace_back(pop(stackIdx));
      }
      pop(stackIdx);
    }
    else if (ShaderDefineExpression::_OperatorPriority[token] > 1) {
      pushOperand(operand, result);
      while (stackIdx != -1
             && ShaderDefineExpression::_OperatorPriority[peek(stackIdx)]
                  >= ShaderDefineExpression::_OperatorPriority[token]) {
        result.emplace_back(pop(stackIdx));
      }
      push(token, stackIdx);
      ++idx;
    }
    else {
      operand += c;
    }
    ++idx;
  }

  pushOperand(operand, result);

  while (stackIdx != -1) {
    if (peek(stackIdx) == "(") {
      pop(stackIdx);
    }
    else {
      result.emplace_back(pop(stackIdx));
    }
  }

  return result;
}

} // end of namespace BABYLON
