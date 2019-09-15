#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class InputBlock;
class NodeMaterialBlock;
class NodeMaterialConnectionPoint;
class ReflectionTextureBlock;
class TextureBlock;
using InputBlockPtr        = std::shared_ptr<InputBlock>;
using NodeMaterialBlockPtr = std::shared_ptr<NodeMaterialBlock>;
using NodeMaterialConnectionPointPtr
  = std::shared_ptr<NodeMaterialConnectionPoint>;
using ReflectionTextureBlockPtr = std::shared_ptr<ReflectionTextureBlock>;
using TextureBlockPtr           = std::shared_ptr<TextureBlock>;

struct BABYLON_SHARED_EXPORT NodeMaterialCompilationHints {
  bool needWorldViewMatrix           = false;
  bool needWorldViewProjectionMatrix = false;
  bool needAlphaBlending             = false;
  bool needAlphaTesting              = false;
}; // end of struct NodeMaterialCompilationHints

struct BABYLON_SHARED_EXPORT NodeMaterialCompilationChecks {
  bool emitVertex   = false;
  bool emitFragment = false;
  std::vector<NodeMaterialConnectionPointPtr> notConnectedNonOptionalInputs;
}; // end of struct NodeMaterialCompilationChecks

/**
 * @brief Class used to store shared data between 2 NodeMaterialBuildState.
 */
struct BABYLON_SHARED_EXPORT NodeMaterialBuildStateSharedData {

  /**
   * @brief Creates a new shared data.
   */
  NodeMaterialBuildStateSharedData();
  ~NodeMaterialBuildStateSharedData();

  /**
   * @brief Emits console errors and exceptions if there is a failing check.
   */
  void emitErrors();

  /**
   * Gets the list of emitted varyings
   */
  std::vector<std::string> temps;

  /**
   * Gets the list of emitted varyings
   */
  std::vector<std::string> varyings;

  /**
   * Gets the varying declaration string
   */
  std::string varyingDeclaration;

  /**
   * Input blocks
   */
  std::vector<InputBlockPtr> inputBlocks;

  /**
   * Input blocks
   */
  std::vector<std::variant<TextureBlockPtr, ReflectionTextureBlockPtr>>
    textureBlocks;

  /**
   * Bindable blocks (Blocks that need to set data to the effect)
   */
  std::vector<NodeMaterialBlockPtr> bindableBlocks;

  /**
   * List of blocks that can provide a compilation fallback
   */
  std::vector<NodeMaterialBlockPtr> blocksWithFallbacks;

  /**
   * List of blocks that can provide a define update
   */
  std::vector<NodeMaterialBlockPtr> blocksWithDefines;

  /**
   * List of blocks that can provide a repeatable content
   */
  std::vector<NodeMaterialBlockPtr> repeatableContentBlocks;

  /**
   * List of blocks that can provide a dynamic list of uniforms
   */
  std::vector<NodeMaterialBlockPtr> dynamicUniformBlocks;

  /**
   * List of blocks that can block the isReady function for the material
   */
  std::vector<NodeMaterialBlockPtr> blockingBlocks;

  /**
   * Gets the list of animated inputs
   */
  std::vector<InputBlockPtr> animatedInputs;

  /**
   * Build Id used to avoid multiple recompilations
   */
  size_t buildId;

  /**
   * List of emitted variables
   */
  std::unordered_map<std::string, unsigned int> variableNames;

  /**
   * List of emitted defines
   */
  std::unordered_map<std::string, unsigned int> defineNames;

  /**
   * Should emit comments?
   */
  bool emitComments;

  /**
   * Emit build activity
   */
  bool verbose;

  /**
   * Gets the compilation hints emitted at compilation time
   */
  NodeMaterialCompilationHints hints;

  /**
   * List of compilation checks
   */
  NodeMaterialCompilationChecks checks;

}; // end of struct NodeMaterialBuildStateSharedData

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H
