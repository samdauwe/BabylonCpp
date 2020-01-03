#include <babylon/engines/processors/shader_processor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/engines/processors/expressions/operators/shader_define_and_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_arithmetic_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_is_defined_operator.h>
#include <babylon/engines/processors/expressions/operators/shader_define_or_operator.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>
#include <babylon/engines/processors/ishader_processor.h>
#include <babylon/engines/processors/processing_options.h>
#include <babylon/engines/processors/shader_code_condition_node.h>
#include <babylon/engines/processors/shader_code_cursor.h>
#include <babylon/engines/processors/shader_code_node.h>
#include <babylon/engines/processors/shader_code_test_node.h>
#include <babylon/misc/file_tools.h>

namespace BABYLON {

void ShaderProcessor::Process(const std::string& sourceCode, ProcessingOptions& options,
                              const std::function<void(const std::string& migratedCode)>& callback)
{
  _ProcessIncludes(sourceCode, options,
                   [&options, callback](const std::string& codeWithIncludes) -> void {
                     const auto migratedCode = _ProcessShaderConversion(codeWithIncludes, options);
                     callback(migratedCode);
                   });
}

std::string ShaderProcessor::_ProcessPrecision(std::string source, const ProcessingOptions& options)
{
  const auto shouldUseHighPrecisionShader = options.shouldUseHighPrecisionShader;

  if (String::indexOf(source, "precision highp float") == -1) {
    if (!shouldUseHighPrecisionShader) {
      source = "precision mediump float;\n" + source;
    }
    else {
      source = "precision highp float;\n" + source;
    }
  }
  else {
    if (!shouldUseHighPrecisionShader) { // Moving highp to mediump
      source = String::replace(source, "precision highp float", "precision mediump float");
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
    return std::make_shared<ShaderDefineIsDefinedOperator>(String::trimCopy(match[1]),
                                                           expression[0] == '!');
  }

  const std::vector<std::string> operators{"==", ">=", "<=", "<", ">"};
  std::string matchedOperator = "";
  int indexOperator           = 0;

  for (const auto& _operator : operators) {
    matchedOperator = _operator;
    indexOperator   = String::indexOf(expression, _operator);

    if (indexOperator > -1) {
      break;
    }
  }

  if (indexOperator == -1) {
    return std::make_shared<ShaderDefineIsDefinedOperator>(expression);
  }

  auto _indexOperator = static_cast<size_t>(indexOperator);
  auto define         = String::trimCopy(expression.substr(0, _indexOperator));
  auto value = String::trimCopy(expression.substr(_indexOperator + matchedOperator.size()));

  return std::make_shared<ShaderDefineArithmeticOperator>(define, matchedOperator, value);
}

ShaderDefineExpressionPtr ShaderProcessor::_BuildSubExpression(const std::string& expression)
{
  auto indexOr = String::indexOf(expression, "||");
  if (indexOr == -1) {
    auto indexAnd = String::indexOf(expression, "&&");
    if (indexAnd > -1) {
      auto _indexAnd   = static_cast<size_t>(indexAnd);
      auto andOperator = std::make_shared<ShaderDefineAndOperator>();
      auto leftPart    = String::trimCopy(expression.substr(0, _indexAnd));
      auto rightPart   = String::trimCopy(expression.substr(_indexAnd + 2));

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
    auto leftPart   = String::trimCopy(expression.substr(0, _indexOr));
    auto rightPart  = String::trimCopy(expression.substr(_indexOr + 2));

    orOperator->leftOperand  = _BuildSubExpression(leftPart);
    orOperator->rightOperand = _BuildSubExpression(rightPart);

    return orOperator;
  }
}

ShaderCodeTestNodePtr ShaderProcessor::_BuildExpression(const std::string& line, size_t start)
{
  auto node       = std::make_shared<ShaderCodeTestNode>();
  auto command    = line.substr(0, start);
  auto expression = String::trimCopy(line.substr(start));

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
    auto first5 = String::toLowerCase(line.substr(0, 5));

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
        auto split                   = String::split(String::replace(line, ";", ""), ' ');
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
    const auto line = String::stripComments(sourceCodeLine, "//");

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
  cursor.lines     = _removeCommentsAndEmptyLines(String::split(sourceCode, '\n'));

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
    auto keyValue           = String::replace(String::replace(define, "#define", ""), ";", "");
    keyValue                = String::trim(keyValue);
    auto split              = String::split(keyValue, ' ');
    preprocessors[split[0]] = split.size() > 1 ? split[1] : "";
  }

  preprocessors["GL_ES"]              = "true";
  preprocessors["__VERSION__"]        = options.version;
  preprocessors[options.platformName] = "true";

  return preprocessors;
}

std::string ShaderProcessor::_ProcessShaderConversion(const std::string& sourceCode,
                                                      ProcessingOptions& options)
{
  auto preparedSourceCode = _ProcessPrecision(sourceCode, options);

  if (!options.processor) {
    return preparedSourceCode;
  }

  // Already converted
  if (String::indexOf(preparedSourceCode, "#version 3") != -1) {
    return String::replace(preparedSourceCode, "#version 300 es", "");
  }

  const auto& defines = options.defines;

  auto preprocessors = _PreparePreProcessors(options);

  // General pre processing
  /* if (options.processor->preProcessor) */ {
    preparedSourceCode
      = options.processor->preProcessor(preparedSourceCode, defines, options.isFragment);
  }

  preparedSourceCode = _EvaluatePreProcessors(preparedSourceCode, preprocessors, options);

  // Post processing
  /* if (options.processor->postProcessor) */ {
    preparedSourceCode
      = options.processor->postProcessor(preparedSourceCode, defines, options.isFragment);
  }

  return preparedSourceCode;
}

void ShaderProcessor::_ProcessIncludes(const std::string& sourceCode, ProcessingOptions& options,
                                       const std::function<void(const std::string& data)>& callback)
{
  static std::string reStr = R"(#include<(.+)>(\((.*)\))*(\[(.*)\])*)";
  static std::regex regex(reStr, std::regex::optimize);
  auto regexBegin = std::sregex_iterator(sourceCode.begin(), sourceCode.end(), regex);
  auto regexEnd   = std::sregex_iterator();

  auto returnValue = sourceCode;

  for (std::sregex_iterator regexIt = regexBegin; regexIt != regexEnd; ++regexIt) {
    auto match       = *regexIt;
    auto includeFile = match[1].str();

    // Uniform declaration
    if (String::indexOf(includeFile, "__decl__") != -1) {
      includeFile = String::replace(includeFile, "__decl__", "");
      if (options.supportsUniformBuffers) {
        includeFile = String::replace(includeFile, "Vertex", "Ubo");
        includeFile = String::replace(includeFile, "Fragment", "Ubo");
      }
      includeFile = includeFile + "Declaration";
    }

    if (stl_util::contains(options.includesShadersStore, includeFile)
        && !options.includesShadersStore[includeFile].empty()) {
      // Substitution
      auto includeContent = options.includesShadersStore[includeFile];
      if (match.size() > 2 && !match[2].str().empty()) {
        auto splits = String::split(match[3].str(), ',');

        for (size_t index = 0; index < splits.size(); index += 2) {
          auto dest = splits[index + 1];

          includeContent = String::regexReplace(includeContent, splits[index], dest);
        }
      }

      if (match.size() > 4 && !match[4].str().empty()) {
        auto indexString = match[5].str();

        if (String::indexOf(indexString, "..") != -1) {
          String::replaceInPlace(indexString, "..", "@");
          auto indexSplits = String::split(indexString, '@');
          auto minIndex    = String::toNumber<int>(indexSplits[0]);
          auto maxIndex
            = String::isDigit(indexSplits[1]) ? String::toNumber<int>(indexSplits[1]) : -1;
          auto sourceIncludeContent = String::slice(includeContent, 0);
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
              += String::regexReplace(sourceIncludeContent, R"(\{X\})", std::to_string(i)) + "\n";
          }
        }
        else {
          if (!options.supportsUniformBuffers) {
            // Ubo replacement
            /*includeContent = includeContent.replace(/light\{X\}.(\w*)/g, (str: string, p1: string)
            => { return p1 + "{X}";
            });*/
          }
          includeContent = String::regexReplace(includeContent, R"(\{X\})", indexString);
        }
      }

      // Replace
      returnValue = String::replace(returnValue, match[0].str(), includeContent);
    }
    else {
      auto includeShaderUrl = options.shadersRepository + "ShadersInclude/" + includeFile + ".fx";

      ShaderProcessor::_FileToolsLoadFile(
        includeShaderUrl,
        [&options, includeFile, returnValue,
         callback](const std::variant<std::string, ArrayBuffer>& fileContent,
                   const std::string & /*responseURL*/) -> void {
          if (std::holds_alternative<std::string>(fileContent)) {
            options.includesShadersStore[includeFile] = std::get<std::string>(fileContent);
            _ProcessIncludes(returnValue, options, callback);
          }
        });
      return;
    }
  }

  callback(returnValue);
}

void ShaderProcessor::_FileToolsLoadFile(
  const std::string& url,
  const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  FileTools::LoadFile(url, onSuccess, onProgress, useArrayBuffer, onError);
}

} // end of namespace BABYLON
