#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(CurrentScreenBlock)
FWD_CLASS_SPTR(InputBlock)
FWD_CLASS_SPTR(NodeMaterialBlock)
FWD_CLASS_SPTR(NodeMaterialConnectionPoint)
FWD_CLASS_SPTR(ParticleTextureBlock)
FWD_CLASS_SPTR(ReflectionTextureBlock)
FWD_CLASS_SPTR(RefractionBlock)
FWD_CLASS_SPTR(TextureBlock)

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

  using TextureInputBlockType
    = std::variant<TextureBlockPtr, ReflectionTextureBlockPtr, RefractionBlockPtr,
                   CurrentScreenBlockPtr, ParticleTextureBlockPtr>;

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
  std::vector<TextureInputBlockType> textureBlocks;

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
   * Gets or sets the hosting scene
   */
  Scene* scene;

  /**
   * Gets the compilation hints emitted at compilation time
   */
  NodeMaterialCompilationHints hints;

  /**
   * List of compilation checks
   */
  NodeMaterialCompilationChecks checks;

  /**
   * Is vertex program allowed to be empty?
   */
  bool allowEmptyVertexProgram;

}; // end of struct NodeMaterialBuildStateSharedData

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_BUILD_STATE_SHARED_DATA_H
