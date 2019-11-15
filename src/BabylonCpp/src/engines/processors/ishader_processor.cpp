#include <babylon/engines/processors/ishader_processor.h>

namespace BABYLON {

IShaderProcessor::~IShaderProcessor() = default;

std::string IShaderProcessor::attributeProcessor(const std::string& /*attribute*/)
{
  return "";
}

std::string IShaderProcessor::varyingProcessor(const std::string& /*varying*/, bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::uniformProcessor(const std::string& /*uniform*/, bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::uniformBufferProcessor(const std::string& /*uniformBuffer*/,
                                                     bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::endOfUniformBufferProcessor(const std::string& /*closingBracketLine*/,
                                                          bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::lineProcessor(const std::string& /*line*/, bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::preProcessor(const std::string& /*code*/,
                                           const std::vector<std::string>& /*defines*/,
                                           bool /*isFragment*/)
{
  return "";
}

std::string IShaderProcessor::postProcessor(std::string /*code*/,
                                            const std::vector<std::string>& /*defines*/,
                                            bool /*isFragment*/)
{
  return "";
}

} // end of namespace BABYLON
