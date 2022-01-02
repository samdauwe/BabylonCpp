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
      /* if (processor->lineProcessor) */ {
        value = processor->lineProcessor(value, options.isFragment);
      }

      if (/* processor->attributeProcessor && */ StringTools::startsWith(line, "attribute")) {
        value = processor->attributeProcessor(line);
      }
      else if (/* processor->varyingProcessor && */ StringTools::startsWith(line, "varying")) {
        value = processor->varyingProcessor(line, options.isFragment);
      }
      else if (/* (processor->uniformProcessor || processor->uniformBufferProcessor) && */
               StringTools::startsWith(line, "uniform")) {
        auto reSearch = "uniform (.+) (.+)";
        std::regex regex(reSearch, std::regex::optimize);

        if (std::regex_search(line, regex)) { // uniform
          /* if (processor->uniformProcessor) */ {
            value = processor->uniformProcessor(line, options.isFragment);
          }
        }
        else { // Uniform buffer
          /* if (processor.uniformBufferProcessor) */ {
            value = processor->uniformBufferProcessor(line, options.isFragment);
            options.lookForClosingBracketForUniformBuffer = true;
          }
        }
      }

      /* if (processor->endOfUniformBufferProcessor) */ {
        if (options.lookForClosingBracketForUniformBuffer
            && StringTools::indexOf(line, "}") != -1) {
          options.lookForClosingBracketForUniformBuffer = false;
          value = processor->endOfUniformBufferProcessor(line, options.isFragment);
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
