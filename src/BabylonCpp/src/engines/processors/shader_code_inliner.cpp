#include <babylon/engines/processors/shader_code_inliner.h>

#include <babylon/core/logging.h>
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
      = _extractBetweenMarkers('(', ')', _sourceCode, funcParamsStartIndex);
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
    const auto funcBodyStartIndex = _skipWhitespaces(_sourceCode, funcParamsEndIndex + 1);
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

    const auto funcBodyEndIndex = _extractBetweenMarkers('{', '}', _sourceCode, funcBodyStartIndex);
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
    auto params = StringTools::split(_removeComments(funcParams), ",");
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

int ShaderCodeInliner::_extractBetweenMarkers(char markerOpen, char markerClose,
                                              const std::string& block, size_t startIndex)
{
  auto currPos            = startIndex;
  auto openMarkers        = 0;
  std::string waitForChar = "";

  while (currPos < block.size()) {
    auto currChar = block[currPos];

    if (waitForChar.empty()) {
      if (currChar == markerOpen) {
        ++openMarkers;
      }
      else if (currChar == markerClose) {
        --openMarkers;
      }
      else if ((currChar == '"') || (currChar == '\'') || (currChar == '`')) {
        waitForChar = std::to_string(currChar);
      }
      else if (currChar == '/') {
        if (currPos + 1 < block.size()) {
          const auto nextChar = block[currPos + 1];
          if (nextChar == '/') {
            waitForChar = '\n';
          }
          else if (nextChar == '*') {
            waitForChar = "*/";
          }
        }
      }
    }
    else {
      if (std::to_string(currChar) == waitForChar) {
        if (waitForChar == "\"" || waitForChar == "\'") {
          if (block[currPos - 1] != '\\') {
            waitForChar = "";
          }
        }
        else {
          waitForChar = "";
        }
      }
      else if (waitForChar == "*/" && currChar == '*' && currPos + 1 < block.size()) {
        if (block[currPos + 1] == '/') {
          waitForChar = "";
        }
        if (waitForChar == "") {
          ++currPos;
        }
      }
    }

    ++currPos;
    if (openMarkers == 0) {
      break;
    }
  }

  return openMarkers == 0 ? currPos - 1 : -1;
}

size_t ShaderCodeInliner::_skipWhitespaces(const std::string& s, size_t index)
{
  while (index < s.size()) {
    const auto c = s[index];
    if (c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\u000a' /* && c != '\u00a0' */) {
      break;
    }
    ++index;
  }

  return index;
}

std::string ShaderCodeInliner::_removeComments(const std::string& block)
{
  auto currPos            = 0ull;
  std::string waitForChar = "";
  auto inComments         = false;
  std::vector<char> s{};

  while (currPos < block.size()) {
    auto currChar = block[currPos];

    if (waitForChar.empty()) {
      if ((currChar == '"') || (currChar == '\'') || (currChar == '`')) {
        waitForChar = std::to_string(currChar);
      }
      else if (currChar == '/') {
        if (currPos + 1 < block.size()) {
          const auto nextChar = block[currPos + 1];
          if (nextChar == '/') {
            waitForChar = '\n';
            inComments  = true;
          }
          else if (nextChar == '*') {
            waitForChar = "*/";
            inComments  = true;
          }
        }
      }
      if (!inComments) {
        s.emplace_back(currChar);
      }
    }
    else {
      if (std::to_string(currChar) == waitForChar) {
        if (waitForChar == "\"" || waitForChar == "\'") {
          if (block[currPos - 1] != '\\') {
            waitForChar = "";
          }
          s.emplace_back(currChar);
        }
        else {
          waitForChar = "";
          inComments  = false;
        }
      }
      else if (waitForChar == "*/" && currChar == '*' && currPos + 1 < block.size()) {
        if (block[currPos + 1] == '/') {
          waitForChar = "";
        }
        if (waitForChar == "") {
          inComments = false;
          ++currPos;
        }
      }
      else {
        if (!inComments) {
          s.emplace_back(currChar);
        }
      }
    }

    ++currPos;
  }

  return StringTools::join(s, "");
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

      // Find the opening parenthesis
      const auto callParamsStartIndex
        = _skipWhitespaces(_sourceCode, functionCallIndex + name.size());
      if (callParamsStartIndex == _sourceCode.size() || _sourceCode[callParamsStartIndex] != '(') {
        startIndex = functionCallIndex + name.size();
        continue;
      }

      // extract the parameters of the function call as a whole string (without the leading /
      // trailing parenthesis)
      const auto callParamsEndIndex
        = _extractBetweenMarkers('(', ')', _sourceCode, callParamsStartIndex);
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
      auto params = StringTools::split(_removeComments(callParams), ",");
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
        const auto injectDeclarationIndex = _findBackward(_sourceCode, functionCallIndex - 1, '\n');

        partBefore       = _sourceCode.substr(0, injectDeclarationIndex + 1);
        auto partBetween = _sourceCode.substr(injectDeclarationIndex + 1, functionCallIndex);

        _sourceCode = partBefore + type + " " + retParamName + ";\n" + funcBody + "\n" + partBetween
                      + retParamName + partAfter;

        if (debug) {
          BABYLON_LOGF_DEBUG(
            "ShaderCodeInliner",
            "Replace function call by code. Function '%s' (type=%s). injectDeclarationIndex=%d",
            name.c_str(), type.c_str(), injectDeclarationIndex);
        }
      }
      else {
        // simple case where the return value of the function is "void"
        _sourceCode = partBefore + funcBody + partAfter;

        startIndex += funcBody.size() - (callParamsEndIndex + 1 - functionCallIndex);

        if (debug) {
          BABYLON_LOGF_DEBUG(
            "ShaderCodeInliner",
            "Replace function call by code. Function '%s' (type=%s). functionCallIndex=%d",
            name.c_str(), type.c_str(), functionCallIndex);
        }
      }

      doAgain = true;
    }
  }

  return doAgain;
}

int ShaderCodeInliner::_findBackward(const std::string& s, int index, char c)
{
  while (index >= 0 && s[index] != c) {
    index--;
  }

  return index;
}

std::string ShaderCodeInliner::_escapeRegExp(const std::string& s)
{
  return StringTools::regexReplace(s, R"([.*+?^%s()|[\]\\])", R"(\\$&)");
}

std::string ShaderCodeInliner::_replaceNames(std::string code,
                                             const std::vector<std::string>& sources,
                                             const std::vector<std::string>& destinations)
{

  for (size_t i = 0; i < sources.size(); ++i) {
    const auto source       = _escapeRegExp(sources[i]);
    const auto& destination = destinations[i];

    code = StringTools::regexReplace(code, source, destination);
  }

  return code;
}

} // end of namespace BABYLON
