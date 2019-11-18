#include <babylon/engines/processors/expressions/operators/shader_define_is_defined_operator.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

ShaderDefineIsDefinedOperator::ShaderDefineIsDefinedOperator() : define{""}, _not{false}
{
}

ShaderDefineIsDefinedOperator::ShaderDefineIsDefinedOperator(const std::string& iDefine, bool iNot)
    : define{iDefine}, _not{iNot}
{
}

ShaderDefineIsDefinedOperator::ShaderDefineIsDefinedOperator(
  const ShaderDefineIsDefinedOperator& other)
    : define{other.define}, _not{other._not}
{
}

ShaderDefineIsDefinedOperator::ShaderDefineIsDefinedOperator(ShaderDefineIsDefinedOperator&& other)
    : define{std::move(other.define)}, _not{std::move(other._not)}
{
}

ShaderDefineIsDefinedOperator&
ShaderDefineIsDefinedOperator::operator=(const ShaderDefineIsDefinedOperator& other)
{
  if (&other != this) {
    define = other.define;
    _not   = other._not;
  }

  return *this;
}

ShaderDefineIsDefinedOperator&
ShaderDefineIsDefinedOperator::operator=(ShaderDefineIsDefinedOperator&& other)
{
  if (&other != this) {
    define = std::move(other.define);
    _not   = std::move(other._not);
  }

  return *this;
}

ShaderDefineIsDefinedOperator::~ShaderDefineIsDefinedOperator() = default;

bool ShaderDefineIsDefinedOperator::isTrue(
  const std::unordered_map<std::string, std::string>& preprocessors) const
{
  auto condition = stl_util::contains(preprocessors, define);

  if (_not) {
    condition = !condition;
  }

  return condition;
}

} // namespace BABYLON
