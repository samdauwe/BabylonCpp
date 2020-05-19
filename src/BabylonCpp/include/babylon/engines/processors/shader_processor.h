#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class ArrayBufferView;
struct ProcessingOptions;
class ProgressEvent;
struct ShaderCodeConditionNode;
class ShaderCodeCursor;
struct ShaderCodeNode;
struct ShaderCodeTestNode;
class ShaderDefineArithmeticOperator;
class ShaderDefineExpression;
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
  static constexpr const char* regexSE       = R"(defined\s*?\((.+?)\))";
  static constexpr const char* regexSERevert = R"(defined\s*?\[(.+?)\])";

public:
  static void Process(const std::string& sourceCode, ProcessingOptions& options,
                      const std::function<void(const std::string& migratedCode)>& callback);

  /**
   * @brief Loads a file from a url.
   * @param url url to load
   * @param onSuccess callback called when the file successfully loads
   * @param onProgress callback called while file is loading (if the server supports this mode)
   * @param offlineProvider defines the offline provider for caching
   * @param useArrayBuffer defines a boolean indicating that date must be returned as ArrayBuffer
   * @param onError callback called when the file fails to load
   * @returns a file request object
   * @hidden
   */
  static void _FileToolsLoadFile(
    const std::string& url,
    const std::function<void(const std::variant<std::string, ArrayBufferView>& data,
                             const std::string& responseURL)>& onSuccess,
    const std::function<void(const ProgressEvent& event)>& onProgress = nullptr,
    bool useArrayBuffer                                               = false,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

private:
  static std::string _ProcessPrecision(std::string source, const ProcessingOptions& options);
  static ShaderDefineExpressionPtr _ExtractOperation(const std::string& expression);
  static ShaderDefineExpressionPtr _BuildSubExpression(std::string expression);
  static ShaderCodeTestNodePtr _BuildExpression(const std::string& line, size_t start);
  static void _MoveCursorWithinIf(ShaderCodeCursor& cursor,
                                  const ShaderCodeConditionNodePtr& rootNode,
                                  ShaderCodeNodePtr ifNode);
  static bool _MoveCursor(ShaderCodeCursor& cursor, const ShaderCodeNodePtr& rootNode);
  static std::vector<std::string>
  _removeCommentsAndEmptyLines(const std::vector<std::string>& sourceCodeLines);
  static std::string
  _EvaluatePreProcessors(const std::string& sourceCode,
                         std::unordered_map<std::string, std::string>& preprocessors,
                         ProcessingOptions& options);
  static std::unordered_map<std::string, std::string>
  _PreparePreProcessors(const ProcessingOptions& options);
  static std::string _ProcessShaderConversion(const std::string& sourceCode,
                                              ProcessingOptions& options);
  static void _ProcessIncludes(const std::string& sourceCode, ProcessingOptions& options,
                               const std::function<void(const std::string& data)>& callback);

}; // end of class ShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSOR_H
