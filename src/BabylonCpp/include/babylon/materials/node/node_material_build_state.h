#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/materials/node/node_material_block_connection_point_types.h>
#include <babylon/materials/node/node_material_block_targets.h>

namespace BABYLON {

class NodeMaterialBuildState;
struct NodeMaterialBuildStateSharedData;
struct ReplaceStrings;
using NodeMaterialBuildStatePtr           = std::shared_ptr<NodeMaterialBuildState>;
using NodeMaterialBuildStateSharedDataPtr = std::shared_ptr<NodeMaterialBuildStateSharedData>;

struct BABYLON_SHARED_EXPORT StringsReplacement {
  std::string search  = "";
  std::string replace = "";
}; // end of struct StringsReplacement

struct BABYLON_SHARED_EXPORT EmitCodeFromIncludeOptions {
  std::vector<StringsReplacement> replaceStrings = {};
  std::string repeatKey                          = "";
}; // end of struct _emitCodeFromIncludeOptions

struct BABYLON_SHARED_EXPORT EmitFunctionFromIncludeOptions {
  std::string repeatKey                          = "";
  std::optional<bool> removeAttributes           = std::nullopt;
  std::optional<bool> removeUniforms             = std::nullopt;
  std::optional<bool> removeVaryings             = std::nullopt;
  std::optional<bool> removeIfDef                = std::nullopt;
  std::vector<StringsReplacement> replaceStrings = {};
}; // end of struct EmitFunctionFromIncludeOptions

/**
 * @brief Class used to store node based material build state.
 */
class BABYLON_SHARED_EXPORT NodeMaterialBuildState {

public:
  NodeMaterialBuildState();
  ~NodeMaterialBuildState() = default;

  /**
   * @brief Hidden
   */
  std::string _repeatableContentAnchor();

  /**
   * @brief Finalize the compilation strings.
   * @param state defines the current compilation state
   */
  void finalize(const NodeMaterialBuildState& state);

  /**
   * @brief Hidden
   */
  std::string _getFreeVariableName(std::string prefix);

  /**
   * @brief Hidden
   */
  std::string _getFreeDefineName(const std::string& prefix);

  /**
   * @brief Hidden
   */
  void _excludeVariableName(const std::string& name);

  /**
   * @brief Hidden
   */
  std::string _getGLType(NodeMaterialBlockConnectionPointTypes type) const;

  /**
   * @brief Hidden
   */
  void _emitExtension(const std::string& name, const std::string& extension);

  /**
   * @brief Hidden
   */
  void _emitFunction(const std::string& name, std::string code, const std::string& comments);

  /**
   * @brief Hidden
   */
  std::string _emitCodeFromInclude(const std::string& includeName, const std::string& comments,
                                   const std::optional<EmitCodeFromIncludeOptions>& options
                                   = std::nullopt);

  /**
   * @brief Hidden
   */
  void _emitFunctionFromInclude(const std::string& includeName, const std::string& comments,
                                const std::optional<EmitFunctionFromIncludeOptions>& options
                                = std::nullopt,
                                const std::string& storeKey = "");

  /**
   * @brief Hidden
   */
  bool _registerTempVariable(const std::string& name);

  /**
   * @brief Hidden
   */
  bool _emitVaryingFromString(const std::string& name, const std::string& type,
                              const std::string& define = "", bool notDefine = false);

  /**
   * @brief Hidden
   */
  void _emitUniformFromString(const std::string& name, const std::string& type,
                              const std::string& define = "", bool notDefine = false);

protected:
  /**
   * @brief Hidden
   */
  std::string get__repeatableContentAnchor();

public:
  /**
   * Gets or sets a boolean indicating if the current state can emit uniform buffers
   */
  bool supportUniformBuffers;
  /**
   * Gets the list of emitted attributes
   */
  std::vector<std::string> attributes;
  /**
   * Gets the list of emitted uniforms
   */
  std::vector<std::string> uniforms;
  /**
   * Gets the list of emitted uniform buffers
   */
  std::vector<std::string> uniformBuffers;
  /**
   * Gets the list of emitted samplers
   */
  std::vector<std::string> samplers;
  /**
   * Gets the list of emitted functions
   */
  std::unordered_map<std::string, std::string> functions;
  /**
   * Gets the list of emitted extensions
   */
  std::unordered_map<std::string, std::string> extensions;

  /**
   * Gets the target of the compilation state
   */
  NodeMaterialBlockTargets target;
  /**
   * Gets the list of emitted counters
   */
  std::unordered_map<std::string, unsigned int> counters;

  /**
   * Shared data between multiple NodeMaterialBuildState instances
   */
  NodeMaterialBuildStateSharedDataPtr sharedData;

  /** @hidden */
  NodeMaterialBuildStatePtr _vertexState;

  /** @hidden */
  std::string _attributeDeclaration;
  /** @hidden */
  std::string _uniformDeclaration;
  /** @hidden */
  std::string _samplerDeclaration;
  /** @hidden */
  std::string _varyingTransfer;

  /** @hidden */
  std::string _builtCompilationString;

  /**
   * Gets the emitted compilation strings
   */
  std::string compilationString;

private:
  size_t _repeatableContentAnchorIndex;

}; // end of enum class NodeMaterialBuildState

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_H
