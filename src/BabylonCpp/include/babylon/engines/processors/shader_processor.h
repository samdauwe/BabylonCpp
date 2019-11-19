#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct ProcessingOptions;
struct ShaderCodeConditionNode;
class ShaderCodeCursor;
struct ShaderCodeNode;
struct ShaderCodeTestNode;
class ShaderDefineArithmeticOperator;
struct ShaderDefineExpression;
using ShaderCodeConditionNodePtr        = std::shared_ptr<ShaderCodeConditionNode>;
using ShaderCodeNodePtr                 = std::shared_ptr<ShaderCodeNode>;
using ShaderCodeTestNodePtr             = std::shared_ptr<ShaderCodeTestNode>;
using ShaderDefineArithmeticOperatorPtr = std::shared_ptr<ShaderDefineArithmeticOperator>;
using ShaderDefineExpressionPtr         = std::shared_ptr<ShaderDefineExpression>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ShaderProcessor {

public:
  static void Process(const std::string& sourceCode, const ProcessingOptions& options,
                      const std::function<void(const std::string& migratedCode)> callback);

private:
  static std::string ProcessPrecision(const std::string& source, const ProcessingOptions& options);
  static ShaderDefineArithmeticOperatorPtr _ExtractOperation(const std::string& expression);
  static ShaderDefineExpressionPtr _BuildSubExpression(const std::string& expression);
  static ShaderCodeTestNodePtr _BuildSubExpression(const std::string& line,
                                                   const std::string& start);
  static void _MoveCursorWithinIf(const ShaderCodeCursor& cursor,
                                  const ShaderCodeConditionNodePtr& rootNode,
                                  const ShaderCodeNodePtr& ifNode);
  static bool _MoveCursor(const ShaderCodeCursor& cursor, const ShaderCodeNodePtr& rootNode);
  static std::string
  _EvaluatePreProcessors(const std::string& sourceCode,
                         const std::unordered_map<std::string, std::string>& preprocessors,
                         const ProcessingOptions& options);
  static std::unordered_map<std::string, std::string>
  _PreparePreProcessors(const ProcessingOptions& options);
  static std::string _ProcessShaderConversion(const std::string& sourceCode,
                                              const ProcessingOptions& options);
  static void _ProcessIncludes(const std::string& sourceCode, const ProcessingOptions& options,
                               const std::function<void(const std::string& data)>& callback);

}; // end of class ShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H
