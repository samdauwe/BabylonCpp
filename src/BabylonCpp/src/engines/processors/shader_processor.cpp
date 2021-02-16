#include <babylon/engines/processors/shader_processor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/processors/expressions/operators/shader_define_and_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_arithmetic_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_is_defined_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_or_operator.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>
#include <babylon/engines/processors/ishader_processor.h>
#include <babylon/engines/processors/shader_code_condition_node.h>
#include <babylon/engines/processors/shader_code_cursor.h>
#include <babylon/engines/processors/shader_code_node.h>
#include <babylon/engines/processors/shader_code_test_node.h>
#include <babylon/engines/processors/shader_processing_options.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

void ShaderProcessor::Initialize(ProcessingOptions& options)
{
  if (options.processor && options.processor->initializeShaders) {
    options.processor->initializeShaders(options.processingContext);
  }
}

void ShaderProcessor::Process(const std::string& sourceCode, ProcessingOptions& options,
                              const std::function<void(const std::string& migratedCode)>& callback,
                              ThinEngine* engine)
{
  _ProcessIncludes(sourceCode, options,
                   [&options, callback, &engine](const std::string& codeWithIncludes) -> void {
                     const auto migratedCode
                       = _ProcessShaderConversion(codeWithIncludes, options, engine);
                     callback(migratedCode);
                   });
}

std::unordered_map<std::string, std::string>
ShaderProcessor::Finalize(const std::string& vertexCode, const std::string& fragmentCode,
                          ProcessingOptions& options)
{
  if (!options.processor || !options.processor->finalizeShaders) {
    return {{"vertexCode", vertexCode}, {"fragmentCode", fragmentCode}};
  }

  return options.processor->finalizeShaders(vertexCode, fragmentCode, options.processingContext);
}

std::string ShaderProcessor::_ProcessPrecision(std::string source, const ProcessingOptions& options)
{
  const auto shouldUseHighPrecisionShader = options.shouldUseHighPrecisionShader;

  if (StringTools::indexOf(source, "precision highp float") == -1) {
    if (!shouldUseHighPrecisionShader) {
      source = "precision mediump float;\n" + source;
    }
    else {
      source = "precision highp float;\n" + source;
    }
  }
  else {
    if (!shouldUseHighPrecisionShader) { // Moving highp to mediump
      source = StringTools::replace(source, "precision highp float", "precision mediump float");
    }
  }

  return source;
}

ShaderDefineExpressionPtr ShaderProcessor::_ExtractOperation(const std::string& expression)
{
  auto reStr = R"(defined\((.+)\))";
  std::regex regex(reStr, std::regex::optimize);
  std::smatch match;

  if (std::regex_search(expression, match, regex) && !match.empty()) {
    return std::make_shared<ShaderDefineIsDefinedOperator>(StringTools::trimCopy(match[1]),
                                                           expression[0] == '!');
  }

  const std::vector<std::string> operators{"==", ">=", "<=", "<", ">"};
  std::string matchedOperator = "";
  int indexOperator           = 0;

  for (const auto& _operator : operators) {
    matchedOperator = _operator;
    indexOperator   = StringTools::indexOf(expression, _operator);

    if (indexOperator > -1) {
      break;
    }
  }

  if (indexOperator == -1) {
    return std::make_shared<ShaderDefineIsDefinedOperator>(expression);
  }

  auto _indexOperator = static_cast<size_t>(indexOperator);
  auto define         = StringTools::trimCopy(expression.substr(0, _indexOperator));
  auto value = StringTools::trimCopy(expression.substr(_indexOperator + matchedOperator.size()));

  return std::make_shared<ShaderDefineArithmeticOperator>(define, matchedOperator, value);
}

ShaderDefineExpressionPtr ShaderProcessor::_BuildSubExpression(std::string expression)
{
#if 0 // BabylonJS 4.2.0-alpha.11
  expression = StringTools::regexReplace(expression, regexSE, "defined[$1]");

  const auto postfix = ShaderDefineExpression::infixToPostfix(expression);

  std::vector<std::variant<std::string, ShaderDefineExpressionPtr>> stack;

  for (const auto& c : postfix) {
    if (c != "||" && c != "&&") {
      stack.emplace_back(c);
    }
    else if (stack.size() >= 2) {
      auto v1 = stack[stack.size() - 1];
      auto v2 = stack[stack.size() - 2];

      stack.pop_back();
      stack.pop_back();

      std::shared_ptr<ShaderDefineAndOperator> andOperator = nullptr;
      std::shared_ptr<ShaderDefineOrOperator> orOperator   = nullptr;
      if (c == "&&") {
        andOperator = std::make_shared<ShaderDefineAndOperator>();
      }
      else {
        orOperator = std::make_shared<ShaderDefineOrOperator>();
      }

      if (std::holds_alternative<std::string>(v1)) {
        v1 = StringTools::regexReplace(std::get<std::string>(v1), regexSERevert, "defined($1)");
      }

      if (std::holds_alternative<std::string>(v2)) {
        v2 = StringTools::regexReplace(std::get<std::string>(v2), regexSERevert, "defined($1)");
      }

      if (andOperator) {
        andOperator->leftOperand = std::holds_alternative<std::string>(v2) ?
                                     _ExtractOperation(std::get<std::string>(v2)) :
                                     std::get<ShaderDefineExpressionPtr>(v2);
        andOperator->rightOperand = std::holds_alternative<std::string>(v1) ?
                                      _ExtractOperation(std::get<std::string>(v1)) :
                                      std::get<ShaderDefineExpressionPtr>(v1);
      }
      else {
        orOperator->leftOperand = std::holds_alternative<std::string>(v2) ?
                                    _ExtractOperation(std::get<std::string>(v2)) :
                                    std::get<ShaderDefineExpressionPtr>(v2);
        orOperator->rightOperand = std::holds_alternative<std::string>(v1) ?
                                     _ExtractOperation(std::get<std::string>(v1)) :
                                     std::get<ShaderDefineExpressionPtr>(v1);
      }

      auto _operator = andOperator ? std::static_pointer_cast<ShaderDefineExpression>(andOperator) :
                                     std::static_pointer_cast<ShaderDefineExpression>(orOperator);
      stack.emplace_back(_operator);
    }
  }

  auto& result = stack.back();

  if (std::holds_alternative<std::string>(result)) {
    result = StringTools::regexReplace(std::get<std::string>(result), regexSERevert, "defined($1)");
  }

  // note: stack.length !== 1 if there was an error in the parsing

  return std::holds_alternative<std::string>(result) ?
           _ExtractOperation(std::get<std::string>(result)) :
           std::get<ShaderDefineExpressionPtr>(result);
#else // BabylonJS 4.1.0
  auto indexOr = StringTools::indexOf(expression, "||");
  if (indexOr == -1) {
    auto indexAnd = StringTools::indexOf(expression, "&&");
    if (indexAnd > -1) {
      auto _indexAnd   = static_cast<size_t>(indexAnd);
      auto andOperator = std::make_shared<ShaderDefineAndOperator>();
      auto leftPart    = StringTools::trimCopy(expression.substr(0, _indexAnd));
      auto rightPart   = StringTools::trimCopy(expression.substr(_indexAnd + 2));

      andOperator->leftOperand  = _BuildSubExpression(leftPart);
      andOperator->rightOperand = _BuildSubExpression(rightPart);

      return andOperator;
    }
    else {
      return _ExtractOperation(expression);
    }
  }
  else {
    auto _indexOr   = static_cast<size_t>(indexOr);
    auto orOperator = std::make_shared<ShaderDefineOrOperator>();
    auto leftPart   = StringTools::trimCopy(expression.substr(0, _indexOr));
    auto rightPart  = StringTools::trimCopy(expression.substr(_indexOr + 2));

    orOperator->leftOperand  = _BuildSubExpression(leftPart);
    orOperator->rightOperand = _BuildSubExpression(rightPart);

    return orOperator;
  }
#endif
}

ShaderCodeTestNodePtr ShaderProcessor::_BuildExpression(const std::string& line, size_t start)
{
  auto node       = std::make_shared<ShaderCodeTestNode>();
  auto command    = line.substr(0, start);
  auto expression = line.substr(start);

  const auto indexOfComment = StringTools::indexOf(expression, "//");
  if ((indexOfComment + 1) != 0) {
    expression = StringTools::trimCopy(expression.substr(0, (indexOfComment + 1) - 1));
  }
  else {
    expression = StringTools::trimCopy(expression.substr(0, (expression.size() + 1) - 1));
  }

  if (command == "#ifdef") {
    node->testExpression = std::make_shared<ShaderDefineIsDefinedOperator>(expression);
  }
  else if (command == "#ifndef") {
    node->testExpression = std::make_shared<ShaderDefineIsDefinedOperator>(expression, true);
  }
  else {
    node->testExpression = _BuildSubExpression(expression);
  }

  return node;
}

void ShaderProcessor::_MoveCursorWithinIf(ShaderCodeCursor& cursor,
                                          const ShaderCodeConditionNodePtr& rootNode,
                                          ShaderCodeNodePtr ifNode)
{
  auto line = cursor.currentLine();
  while (_MoveCursor(cursor, ifNode)) {
    line        = cursor.currentLine();
    auto first5 = StringTools::toLowerCase(line.substr(0, 5));

    if (first5 == "#else") {
      auto elseNode = std::make_shared<ShaderCodeNode>();
      rootNode->children.emplace_back(elseNode);
      _MoveCursor(cursor, elseNode);
      return;
    }
    else if (first5 == "#elif") {
      auto elifNode = _BuildExpression(line, 5);

      rootNode->children.emplace_back(elifNode);
      ifNode = elifNode;
    }
  }
}

bool ShaderProcessor::_MoveCursor(ShaderCodeCursor& cursor, const ShaderCodeNodePtr& rootNode)
{
  while (cursor.canRead()) {
    ++cursor.lineIndex;
    const auto& line      = cursor.currentLine();
    const auto reKeywords = R"((#ifdef)|(#else)|(#elif)|(#endif)|(#ifndef)|(#if))";
    std::regex regex(reKeywords, std::regex::optimize);
    std::smatch matches;

    if (std::regex_search(line, matches, regex) && !matches.empty()) {
      auto keyword = matches[0].str();

      if (keyword == "#ifdef") {
        auto newRootNode = std::make_shared<ShaderCodeConditionNode>();
        rootNode->children.emplace_back(newRootNode);

        auto ifNode = _BuildExpression(line, 6);
        newRootNode->children.emplace_back(ifNode);
        _MoveCursorWithinIf(cursor, newRootNode, ifNode);
      }
      else if (keyword == "#else" || keyword == "#elif") {
        return true;
      }
      else if (keyword == "#endif") {
        return false;
      }
      else if (keyword == "#ifndef") {
        auto newRootNode = std::make_shared<ShaderCodeConditionNode>();
        rootNode->children.emplace_back(newRootNode);

        auto ifNode = _BuildExpression(line, 7);
        newRootNode->children.emplace_back(ifNode);
        _MoveCursorWithinIf(cursor, newRootNode, ifNode);
      }
      else if (keyword == "#if") {
        auto newRootNode = std::make_shared<ShaderCodeConditionNode>();
        auto ifNode      = _BuildExpression(line, 3);
        rootNode->children.emplace_back(newRootNode);

        newRootNode->children.emplace_back(ifNode);
        _MoveCursorWithinIf(cursor, newRootNode, ifNode);
      }
    }
    else {
      auto newNode  = std::make_shared<ShaderCodeNode>();
      newNode->line = line;
      rootNode->children.emplace_back(newNode);

      // Detect additional defines
      if (line.size() >= 2 && line[0] == '#' && line[1] == 'd') {
        auto split                   = StringTools::split(StringTools::replace(line, ";", ""), ' ');
        newNode->additionalDefineKey = split[1];

        if (split.size() == 3) {
          newNode->additionalDefineValue = split[2];
        }
      }
    }
  }
  return false;
}

std::vector<std::string>
ShaderProcessor::_removeCommentsAndEmptyLines(const std::vector<std::string>& sourceCodeLines)
{
  std::vector<std::string> result;
  for (const auto& sourceCodeLine : sourceCodeLines) {
    const auto line = StringTools::stripComments(sourceCodeLine, "//");

    // Ignore empty lines
    if (!line.empty()) {
      result.emplace_back(line);
    }
  }

  return result;
}

std::string
ShaderProcessor::_EvaluatePreProcessors(const std::string& sourceCode,
                                        std::unordered_map<std::string, std::string>& preprocessors,
                                        ProcessingOptions& options)
{
  auto rootNode = std::make_shared<ShaderCodeNode>();
  ShaderCodeCursor cursor;

  cursor.lineIndex = -1;
  cursor.lines     = _removeCommentsAndEmptyLines(StringTools::split(sourceCode, '\n'));

  // Decompose (We keep it in 2 steps so it is easier to maintain and perf hit is insignificant)
  _MoveCursor(cursor, rootNode);

  // Recompose
  return rootNode->process(preprocessors, options);
}

std::unordered_map<std::string, std::string>
ShaderProcessor::_PreparePreProcessors(const ProcessingOptions& options)
{
  const auto& defines = options.defines;
  std::unordered_map<std::string, std::string> preprocessors;

  for (const auto& define : defines) {
    auto keyValue = StringTools::replace(StringTools::replace(define, "#define", ""), ";", "");
    keyValue      = StringTools::trim(keyValue);
    auto split    = StringTools::split(keyValue, ' ');
    preprocessors[split[0]] = split.size() > 1 ? split[1] : "";
  }

  preprocessors["GL_ES"]              = "true";
  preprocessors["__VERSION__"]        = options.version;
  preprocessors[options.platformName] = "true";

  return preprocessors;
}

std::string ShaderProcessor::_ProcessShaderConversion(const std::string& sourceCode,
                                                      ProcessingOptions& options,
                                                      ThinEngine* engine)
{
  auto preparedSourceCode = _ProcessPrecision(sourceCode, options);

  if (!options.processor) {
    return preparedSourceCode;
  }

  // Already converted
  if (StringTools::indexOf(preparedSourceCode, "#version 3") != -1) {
    return StringTools::replace(preparedSourceCode, "#version 300 es", "");
  }

  const auto& defines = options.defines;

  auto preprocessors = _PreparePreProcessors(options);

  // General pre processing
  if (options.processor->preProcessor) {
    preparedSourceCode = options.processor->preProcessor(
      preparedSourceCode, defines, options.isFragment, options.processingContext);
  }

  preparedSourceCode = _EvaluatePreProcessors(preparedSourceCode, preprocessors, options);

  // Post processing
  if (options.processor->postProcessor) {
    preparedSourceCode = options.processor->postProcessor(
      preparedSourceCode, defines, options.isFragment, options.processingContext, engine);
  }

  return preparedSourceCode;
}

void ShaderProcessor::_ProcessIncludes(const std::string& sourceCode, ProcessingOptions& options,
                                       const std::function<void(const std::string& data)>& callback)
{
  static const auto regexShaderIncludeStr = R"(#include\s?<(.+)>(\((.*)\))*(\[(.*)\])*)";
  static std::regex regexShaderInclude(regexShaderIncludeStr, std::regex::optimize);
  auto regexBegin = std::sregex_iterator(sourceCode.begin(), sourceCode.end(), regexShaderInclude);
  auto regexEnd   = std::sregex_iterator();

  auto returnValue    = sourceCode;
  auto keepProcessing = false;

  for (std::sregex_iterator regexIt = regexBegin; regexIt != regexEnd; ++regexIt) {
    auto match       = *regexIt;
    auto includeFile = match[1].str();

    // Uniform declaration
    if (StringTools::indexOf(includeFile, "__decl__") != -1) {
      includeFile = StringTools::replace(includeFile, "__decl__", "");
      if (options.supportsUniformBuffers) {
        includeFile = StringTools::replace(includeFile, "Vertex", "Ubo");
        includeFile = StringTools::replace(includeFile, "Fragment", "Ubo");
      }
      includeFile = includeFile + "Declaration";
    }

    if (stl_util::contains(options.includesShadersStore, includeFile)
        && !options.includesShadersStore[includeFile].empty()) {
      // Substitution
      auto includeContent = options.includesShadersStore[includeFile];
      if (match.size() > 2 && !match[2].str().empty()) {
        auto splits = StringTools::split(match[3].str(), ',');

        for (size_t index = 0; index < splits.size(); index += 2) {
          auto dest = splits[index + 1];

          includeContent = StringTools::regexReplace(includeContent, splits[index], dest);
        }
      }

      if (match.size() > 4 && !match[4].str().empty()) {
        auto indexString = match[5].str();

        if (StringTools::indexOf(indexString, "..") != -1) {
          StringTools::replaceInPlace(indexString, "..", "@");
          auto indexSplits          = StringTools::split(indexString, '@');
          auto minIndex             = StringTools::toNumber<int>(indexSplits[0]);
          auto maxIndex             = StringTools::isDigit(indexSplits[1]) ?
                                        StringTools::toNumber<int>(indexSplits[1]) :
                                        -1;
          auto sourceIncludeContent = StringTools::slice(includeContent, 0);
          includeContent            = "";

          if (maxIndex <= 0) {
            maxIndex = options.indexParameters[indexSplits[1]];
          }

          for (int i = minIndex; i < maxIndex; ++i) {
            if (!options.supportsUniformBuffers) {
              // Ubo replacement
              /*sourceIncludeContent = sourceIncludeContent.replace(/light\{X\}.(\w*)/g, (str:
             string, p1: string) => { return p1 + "{X}";
             });*/
            }
            includeContent
              += StringTools::regexReplace(sourceIncludeContent, R"(\{X\})", std::to_string(i))
                 + "\n";
          }
        }
        else {
          if (!options.supportsUniformBuffers) {
            // Ubo replacement
            /*includeContent = includeContent.replace(/light\{X\}.(\w*)/g, (str: string, p1: string)
            => { return p1 + "{X}";
            });*/
          }
          includeContent = StringTools::regexReplace(includeContent, R"(\{X\})", indexString);
        }
      }

      // Replace
      returnValue = StringTools::replace(returnValue, match[0].str(), includeContent);

      keepProcessing = keepProcessing                                            //
                       || StringTools::indexOf(includeContent, "#include<") >= 0 //
                       || StringTools::indexOf(includeContent, "#include <") >= 0;
    }
    else {
      auto includeShaderUrl = options.shadersRepository + "ShadersInclude/" + includeFile + ".fx";

      ShaderProcessor::_FileToolsLoadFile(
        includeShaderUrl,
        [&options, includeFile, returnValue,
         callback](const std::variant<std::string, ArrayBufferView>& fileContent,
                   const std::string& /*responseURL*/) -> void {
          if (std::holds_alternative<std::string>(fileContent)) {
            options.includesShadersStore[includeFile] = std::get<std::string>(fileContent);
            _ProcessIncludes(returnValue, options, callback);
          }
        });
      return;
    }
  }

  if (keepProcessing) {
    _ProcessIncludes(returnValue, options, callback);
  }
  else {
    callback(returnValue);
  }
}

void ShaderProcessor::_FileToolsLoadFile(
  const std::string& url,
  const std::function<void(const std::variant<std::string, ArrayBufferView>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  FileTools::LoadFile(url, onSuccess, onProgress, useArrayBuffer, onError);
}

} // end of namespace BABYLON
