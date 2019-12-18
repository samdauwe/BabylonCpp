#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRONT_FACING_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRONT_FACING_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class FrontFacingBlock;
using FrontFacingBlockPtr = std::shared_ptr<FrontFacingBlock>;

/**
 * @brief Block used to test if the fragment shader is front facing.
 */
class BABYLON_SHARED_EXPORT FrontFacingBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FrontFacingBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FrontFacingBlock>(new FrontFacingBlock(std::forward<Ts>(args)...));
  }
  ~FrontFacingBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new FrontFacingBlock.
   * @param name defines the block name
   */
  FrontFacingBlock(const std::string& name);

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  FrontFacingBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the output component
   */
  ReadOnlyProperty<FrontFacingBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class FrontFacingBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRONT_FACING_BLOCK_H
