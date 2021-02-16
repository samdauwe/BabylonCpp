#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_CODE_INLINER_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_CODE_INLINER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

struct IInlineFunctionDescr {
  std::string name;
  std::string type;
  std::vector<std::string> parameters;
  std::string body;
  size_t callIndex;
}; // end of struct IInlineFunctionDescr

/**
 * @brief Class used to inline functions in shader code
 */
class BABYLON_SHARED_EXPORT ShaderCodeInliner {

public:
  /**
   * @brief Initializes the inliner.
   * @param sourceCode shader code source to inline
   * @param numMaxIterations maximum number of iterations (used to detect recursive calls)
   */
  ShaderCodeInliner(const std::string& sourceCode, size_t numMaxIterations = 20);
  ~ShaderCodeInliner();

  /**
   * @brief Start the processing of the shader code.
   */
  void processCode();

protected:
  /**
   * @brief Gets the code after the inlining process
   */
  std::string get_code() const;

private:
  void _collectFunctions();
  bool _processInlining(size_t numMaxIterations = 20);
  int _extractBetweenMarkers(char markerOpen, char markerClose, const std::string& block,
                             size_t startIndex);
  size_t _skipWhitespaces(const std::string& s, size_t index);
  bool _isIdentifierChar(const std::string& c) const;
  std::string _removeComments(const std::string& block);
  bool _replaceFunctionCallsByCode();
  int _findBackward(const std::string& s, int index, char c) const;
  std::string _escapeRegExp(const std::string& s) const;
  std::string _replaceNames(std::string code, const std::vector<std::string>& sources,
                            const std::vector<std::string>& destinations) const;

public:
  /**
   * Gets or sets the token used to mark the functions to inline
   */
  std::string inlineToken;

  /**
   * Gets or sets the debug mode
   */
  bool debug;

  /**
   * Gets the code after the inlining process
   */
  ReadOnlyProperty<ShaderCodeInliner, std::string> code;

private:
  static constexpr const char* _RegexpFindFunctionNameAndType = R"(((\s+?)(\w+)\s+(\w+)\s*?))";

  std::string _sourceCode;
  std::vector<IInlineFunctionDescr> _functionDescr;
  size_t _numMaxIterations;

}; // end of class ShaderCodeInliner

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_CODE_INLINER_H
