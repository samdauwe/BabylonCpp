#include <babylon/engines/processors/shader_code_inliner.h>

#include <babylon/core/logging.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

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
    BABYLON_LOGF_INFO("ShaderCodeInliner", "Start inlining process (code size=%lull)...",
                      _sourceCode.size());
  }
  _collectFunctions();
  _processInlining(_numMaxIterations);
  if (debug) {
    BABYLON_LOG_INFO("ShaderCodeInliner", "End of inlining process.");
  }
}

void ShaderCodeInliner::_collectFunctions()
{
  // TODO Implement
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
    BABYLON_LOGF_INFO("ShaderCodeInliner", "numMaxIterations is %d after inlining process",
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
          BABYLON_LOGF_INFO(
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
          BABYLON_LOGF_INFO(
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
