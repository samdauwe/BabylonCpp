#include <babylon/engines/processors/shader_code_node.h>

#include <babylon/engines/processors/ishader_processor.h>
#include <babylon/engines/processors/shader_processing_options.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

bool ShaderCodeNode::isValid(
  const std::unordered_map<std::string, std::string>& /*preprocessors*/) const
{
  return true;
}

std::string ShaderCodeNode::process(std::unordered_map<std::string, std::string>& preprocessors,
                                    ProcessingOptions& options)
{
  std::ostringstream result;
  if (!line.empty()) {
    auto value     = line;
    auto processor = options.processor;
    if (processor) {
      // This must be done before other replacements to avoid mistakenly changing something that was
      // already changed.
      if (processor->lineProcessor) {
        value = processor->lineProcessor(value, options.isFragment, options.processingContext);
      }

      if (processor->attributeProcessor && StringTools::startsWith(line, "attribute")) {
        value = processor->attributeProcessor(line, preprocessors, options.processingContext);
      }
      else if (processor->varyingProcessor && StringTools::startsWith(line, "varying")) {
        value = processor->varyingProcessor(line, options.isFragment, preprocessors,
                                            options.processingContext);
      }
      else if ((processor->uniformProcessor || processor->uniformBufferProcessor)
               && StringTools::startsWith(line, "uniform")
               && !options.lookForClosingBracketForUniformBuffer) {
        auto reSearch = R"(uniform\s+(?:(?:highp)?|(?:lowp)?)\s*(\S+)\s+(\S+)\s*;)";
        std::regex regex(reSearch, std::regex::optimize);

        if (std::regex_search(line, regex)) { // uniform
          if (processor->uniformProcessor) {
            value = processor->uniformProcessor(line, options.isFragment, preprocessors,
                                                options.processingContext);
          }
        }
        else { // Uniform buffer
          if (processor->uniformBufferProcessor) {
            value = processor->uniformBufferProcessor(line, options.isFragment,
                                                      options.processingContext);
            options.lookForClosingBracketForUniformBuffer = true;
          }
        }
      }

      if (options.lookForClosingBracketForUniformBuffer && StringTools::indexOf(line, "}") != -1) {
        options.lookForClosingBracketForUniformBuffer = false;
        if (processor->endOfUniformBufferProcessor) {
          value = processor->endOfUniformBufferProcessor(line, options.isFragment,
                                                         options.processingContext);
        }
      }
    }

    result << value << "\r\n";
  }

  for (const auto& child : children) {
    result << child->process(preprocessors, options);
  }

  if (!additionalDefineKey.empty()) {
    preprocessors[additionalDefineKey]
      = !additionalDefineValue.empty() ? additionalDefineValue : "true";
  }

  return result.str();
}

} // end of namespace BABYLON
