#ifndef BABYLON_MATERIALS_NODE_BLOCKS_MAX_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_MAX_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class MaxBlock;
using MaxBlockPtr = std::shared_ptr<MaxBlock>;

/**
 * @brief Block used to get the max of 2 values.
 */
class BABYLON_SHARED_EXPORT MaxBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static MaxBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<MaxBlock>(new MaxBlock(std::forward<Ts>(args)...));
  }
  ~MaxBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new MaxBlock.
   * @param name defines the block name
   */
  MaxBlock(const std::string& name);

  /**
   * @brief Gets the left operand input component.
   */
  NodeMaterialConnectionPointPtr& get_left();

  /**
   * @brief Gets the right operand input component.
   */
  NodeMaterialConnectionPointPtr& get_right();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  MaxBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<MaxBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<MaxBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<MaxBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class MaxBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_MAX_BLOCK_H
