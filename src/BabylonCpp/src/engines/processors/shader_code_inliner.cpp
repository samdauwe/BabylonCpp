#include <babylon/engines/processors/shader_code_inliner.h>

#include <babylon/core/logging.h>
#include <babylon/misc/code_string_parsing_tools.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

static std::string toString(const IInlineFunctionDescr& descr)
{
  std::ostringstream oss;
  oss << "{\"name\":\"" << descr.name << "\",\"name\":\"" << descr.type << "\",\"parameters\":";
  if (descr.parameters.empty()) {
    oss << "["
        << "]";
  }
  else {
    oss << "[\"" << StringTools::join(descr.parameters, "\",") << "\"]";
  }
  oss << ",\"body\":\"" << descr.body << "\",\"callIndex\":" << descr.callIndex << "}";
  return oss.str();
}

static std::string toString(const std::vector<IInlineFunctionDescr>& descr)
{
  std::vector<std::string> descriptions;
  for (const auto& descriptor : descr) {
    descriptions.emplace_back(toString(descriptor));
  }
  std::ostringstream oss;
  if (descriptions.empty()) {
    oss << "["
        << "]";
  }
  else {
    oss << "[" << StringTools::join(descriptions, ",") << "]";
  }
  return oss.str();
}

ShaderCodeInliner::ShaderCodeInliner(const std::string& sourceCode, size_t numMaxIterations)
    : debug{false}, code{this, &ShaderCodeInliner::get_code}
{
  _sourceCode       = sourceCode;
  _numMaxIterations = numMaxIterations;
  _functionDescr    = {};
  inlineToken       = "#define inline";
}

ShaderCodeInliner::~ShaderCodeInliner() = default;

std::string ShaderCodeInliner::get_code() const
{
  return _sourceCode;
}

void ShaderCodeInliner::processCode()
{
  if (debug) {
    BABYLON_LOGF_DEBUG("ShaderCodeInliner", "Start inlining process (code size=%lull)...",
                       _sourceCode.size());
  }
  _collectFunctions();
  _processInlining(_numMaxIterations);
  if (debug) {
    BABYLON_LOG_DEBUG("ShaderCodeInliner", "End of inlining process.");
  }
}

void ShaderCodeInliner::_collectFunctions()
{
  size_t startIndex = 0;

  while (startIndex < _sourceCode.size()) {
    // locate the function to inline and extract its name
    const auto inlineTokenIndex = StringTools::indexOf(_sourceCode, inlineToken, startIndex);
    if (inlineTokenIndex < 0) {
      break;
    }

    const auto funcParamsStartIndex
      = StringTools::indexOf(_sourceCode, "(", inlineTokenIndex + inlineToken.size());
    if (funcParamsStartIndex < 0) {
      if (debug) {
        BABYLON_LOGF_WARN("ShaderCodeInliner",
                          "Could not find the opening parenthesis after the token. startIndex=%zu",
                          startIndex);
      }
      startIndex = inlineTokenIndex + inlineToken.size();
      continue;
    }

    const auto funcNameMatch = StringTools::regexMatch(
      _sourceCode.substr(inlineTokenIndex + inlineToken.size(), funcParamsStartIndex),
      std::regex(ShaderCodeInliner::_RegexpFindFunctionNameAndType, std::regex::optimize));
    if (funcNameMatch.empty()) {
      if (debug) {
        BABYLON_LOGF_WARN(
          "ShaderCodeInliner", "Could not extract the name/type of the function from: %s",
          _sourceCode.substr(inlineTokenIndex + inlineToken.size(), funcParamsStartIndex).c_str());
      }
      startIndex = inlineTokenIndex + inlineToken.size();
      continue;
    }
    const auto funcType = funcNameMatch.size() > 3 ? funcNameMatch[3] : "";
    const auto funcName = funcNameMatch.size() > 4 ? funcNameMatch[4] : "";

    // extract the parameters of the function as a whole string (without the leading / trailing
    // parenthesis)
    const auto funcParamsEndIndex
      = CodeStringParsingTools::ExtractBetweenMarkers('(', ')', _sourceCode, funcParamsStartIndex);
    if (funcParamsEndIndex < 0) {
      if (debug) {
        BABYLON_LOGF_WARN(
          "ShaderCodeInliner",
          "Could not extract the parameters the function '%s' (type=%s). funcParamsStartIndex=%d",
          funcName.c_str(), funcType.c_str(), funcParamsStartIndex);
      }
      startIndex = inlineTokenIndex + inlineToken.size();
      continue;
    }
    const auto funcParams = _sourceCode.substr(funcParamsStartIndex + 1, funcParamsEndIndex);

    // extract the body of the function (with the curly brackets)
    const auto funcBodyStartIndex
      = CodeStringParsingTools::SkipWhitespaces(_sourceCode, funcParamsEndIndex + 1);
    if (funcBodyStartIndex == _sourceCode.size()) {
      if (debug) {
        BABYLON_LOGF_WARN(
          "ShaderCodeInliner",
          "Could not extract the body of the function '%s' (type=%s). funcParamsEndIndex=%d",
          funcName.c_str(), funcType.c_str(), funcParamsEndIndex);
      }
      startIndex = inlineTokenIndex + inlineToken.size();
      continue;
    }

    const auto funcBodyEndIndex
      = CodeStringParsingTools::ExtractBetweenMarkers('{', '}', _sourceCode, funcBodyStartIndex);
    if (funcBodyEndIndex < 0) {
      if (debug) {
        BABYLON_LOGF_WARN(
          "ShaderCodeInliner",
          "Could not extract the body of the function '%s' (type=%s). funcBodyStartIndex=%lu",
          funcName.c_str(), funcType.c_str(), funcBodyStartIndex);
      }
      startIndex = inlineTokenIndex + inlineToken.size();
      continue;
    }
    const auto funcBody = _sourceCode.substr(funcBodyStartIndex, funcBodyEndIndex + 1);

    // process the parameters: extract each names
    auto params = StringTools::split(CodeStringParsingTools::RemoveComments(funcParams), ",");
    std::vector<std::string> paramNames{};

    for (auto& iParam : params) {
      const auto& param = StringTools::trim(iParam);
      const auto idx    = StringTools::lastIndexOf(param, " ");

      if (idx >= 0) {
        paramNames.emplace_back(param.substr(idx + 1));
      }
    }

    if (funcType != "void") {
      // for functions that return a value, we will replace "return" by "tempvarname = ",
      // tempvarname being a unique generated name
      paramNames.emplace_back("return");
    }

    // collect the function
    _functionDescr.emplace_back(IInlineFunctionDescr{
      funcName,   // name
      funcType,   // type
      paramNames, // parameters
      funcBody,   // body
      0,          // callIndex
    });

    startIndex = funcBodyEndIndex + 1;

    // remove the function from the source code
    const auto partBefore = inlineTokenIndex > 0 ? _sourceCode.substr(0, inlineTokenIndex) : "";
    const auto partAfter  = funcBodyEndIndex + 1 < static_cast<int>(_sourceCode.size()) - 1 ?
                              _sourceCode.substr(funcBodyEndIndex + 1) :
                              "";

    _sourceCode = partBefore + partAfter;

    startIndex -= funcBodyEndIndex + 1 - inlineTokenIndex;
  }

  if (debug) {
    BABYLON_LOGF_DEBUG("ShaderCodeInliner",
                       "Collect functions: %zu functions found. functionDescr=, %s",
                       _functionDescr.size(), toString(_functionDescr).c_str());
  }
}

bool ShaderCodeInliner::_processInlining(size_t iNumMaxIterations)
{
  auto numMaxIterations = static_cast<int>(iNumMaxIterations);

  while (numMaxIterations-- >= 0) {
    if (!_replaceFunctionCallsByCode()) {
      break;
    }
  }

  if (debug) {
    BABYLON_LOGF_DEBUG("ShaderCodeInliner", "numMaxIterations is %d after inlining process",
                       numMaxIterations);
  }

  return numMaxIterations >= 0;
}

bool ShaderCodeInliner::_replaceFunctionCallsByCode()
{
  auto doAgain = false;

  for (auto& func : _functionDescr) {
    const auto& name       = func.name;
    const auto& type       = func.type;
    const auto& parameters = func.parameters;
    const auto& body       = func.body;

    size_t startIndex = 0;

    while (startIndex < _sourceCode.size()) {
      // Look for the function name in the source code
      const auto functionCallIndex = StringTools::indexOf(_sourceCode, name, startIndex);

      if (functionCallIndex < 0) {
        break;
      }

      // Make sure "name" is not part of a bigger string
      if (functionCallIndex == 0
          || CodeStringParsingTools::IsIdentifierChar(
            std::to_string(_sourceCode[functionCallIndex - 1]))) {
        startIndex = functionCallIndex + name.size();
        continue;
      }

      // Find the opening parenthesis
      const auto callParamsStartIndex
        = CodeStringParsingTools::SkipWhitespaces(_sourceCode, functionCallIndex + name.size());
      if (callParamsStartIndex == _sourceCode.size() || _sourceCode[callParamsStartIndex] != '(') {
        startIndex = functionCallIndex + name.size();
        continue;
      }

      // extract the parameters of the function call as a whole string (without the leading /
      // trailing parenthesis)
      const auto callParamsEndIndex = CodeStringParsingTools::ExtractBetweenMarkers(
        '(', ')', _sourceCode, callParamsStartIndex);
      if (callParamsEndIndex < 0) {
        if (debug) {
          BABYLON_LOGF_WARN("ShaderCodeInliner",
                            "Could not extract the parameters of the function call. Function '%s' "
                            "(type=%s). callParamsStartIndex=%lull",
                            name.c_str(), type.c_str(), callParamsStartIndex);
        }
        startIndex = functionCallIndex + name.size();
        continue;
      }
      const auto callParams = _sourceCode.substr(callParamsStartIndex + 1, callParamsEndIndex);

      // process the parameter call: extract each names

      // this function split the parameter list used in the function call at ',' boundaries by
      // taking care of potential parenthesis like in:
      //      myfunc(a, vec2(1., 0.), 4.)
      const auto splitParameterCall = [](const std::string& s) -> std::vector<std::string> {
        std::vector<std::string> parameters;
        auto curIdx = 0ull, startParamIdx = 0ull;
        while (curIdx < s.size()) {
          if (s[curIdx] == '(') {
            const auto idx2 = CodeStringParsingTools::ExtractBetweenMarkers('(', ')', s, curIdx);
            if (idx2 < 0) {
              return parameters;
            }
            curIdx = idx2;
          }
          else if (s[curIdx] == ',') {
            parameters.emplace_back(s.substr(startParamIdx, curIdx));
            startParamIdx = curIdx + 1;
          }
          ++curIdx;
        }
        if (startParamIdx < curIdx) {
          parameters.emplace_back(s.substr(startParamIdx, curIdx));
        }
        return parameters;
      };

      auto params = splitParameterCall(CodeStringParsingTools::RemoveComments(callParams));

      if (params.empty()) {
        if (debug) {
          BABYLON_LOGF_WARN(
            "ShaderCodeInliner",
            "Invalid function call: can't extract the parameters of the function call. Function "
            "'%s' (type=%s). callParamsStartIndex=%lu, callParams=%s",
            name.c_str(), type.c_str(), callParamsStartIndex, callParams.c_str());
        }
        startIndex = functionCallIndex + name.size();
        continue;
      }

      std::vector<std::string> paramNames{};

      for (size_t p = 0; p < params.size(); ++p) {
        const auto param = StringTools::trim(params[p]);
        paramNames.emplace_back(param);
      }

      const auto retParamName = type != "void" ? name + '_' + std::to_string(func.callIndex++) : "";

      if (!retParamName.empty()) {
        paramNames.emplace_back(retParamName + " =");
      }

      if (paramNames.size() != parameters.size()) {
        if (debug) {
          BABYLON_LOGF_WARN("ShaderCodeInliner",
                            "Invalid function call: not the same number of parameters for the call "
                            "than the number expected by the function. Function '%s' (type=%s). "
                            "function parameters=%s, call parameters=%s",
                            name.c_str(), type.c_str(), StringTools::join(parameters, ",").c_str(),
                            StringTools::join(paramNames, ",").c_str());
        }
        startIndex = functionCallIndex + name.size();
        continue;
      }

      startIndex = callParamsEndIndex + 1;

      // replace the function call by the body function
      const auto funcBody = _replaceNames(body, parameters, paramNames);

      auto partBefore = functionCallIndex > 0 ? _sourceCode.substr(0, functionCallIndex) : "";
      auto partAfter  = callParamsEndIndex + 1 < static_cast<int>(_sourceCode.size()) - 1 ?
                          _sourceCode.substr(callParamsEndIndex + 1) :
                          "";

      if (!retParamName.empty()) {
        // case where the function returns a value. We generate:
        // FUNCTYPE retParamName;
        // {function body}
        // and replace the function call by retParamName
        const auto injectDeclarationIndex
          = CodeStringParsingTools::FindBackward(_sourceCode, functionCallIndex - 1, '\n');

        partBefore       = _sourceCode.substr(0, injectDeclarationIndex + 1);
        auto partBetween = _sourceCode.substr(injectDeclarationIndex + 1, functionCallIndex);

        _sourceCode = partBefore + type + " " + retParamName + ";\n" + funcBody + "\n" + partBetween
                      + retParamName + partAfter;

        if (debug) {
          BABYLON_LOGF_DEBUG("ShaderCodeInliner",
                             "Replace function call by code. Function '%s' (type=%s). "
                             "injectDeclarationIndex=%d, call parameters=%s",
                             name.c_str(), type.c_str(), injectDeclarationIndex,
                             StringTools::join(paramNames, ',').c_str());
        }
      }
      else {
        // simple case where the return value of the function is "void"
        _sourceCode = partBefore + funcBody + partAfter;

        startIndex += funcBody.size() - (callParamsEndIndex + 1 - functionCallIndex);

        if (debug) {
          BABYLON_LOGF_DEBUG("ShaderCodeInliner",
                             "Replace function call by code. Function '%s' (type=%s). "
                             "functionCallIndex=%d, call parameters=%s",
                             name.c_str(), type.c_str(), functionCallIndex,
                             StringTools::join(paramNames, ',').c_str());
        }
      }

      doAgain = true;
    }
  }

  return doAgain;
}

std::string ShaderCodeInliner::_replaceNames(std::string iCode,
                                             const std::vector<std::string>& sources,
                                             const std::vector<std::string>& destinations) const
{
  // TODO: Make sure "source" is not part of a bigger identifier (for eg, if source=view and we
  // matched it with viewDirection)
  for (size_t i = 0; i < sources.size(); ++i) {
    const auto source       = CodeStringParsingTools::EscapeRegExp(sources[i]);
    const auto& destination = destinations[i];

    iCode = StringTools::regexReplace(iCode, source, destination);
  }

  return iCode;
}

} // end of namespace BABYLON
