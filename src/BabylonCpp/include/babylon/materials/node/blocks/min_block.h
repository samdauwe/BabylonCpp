#ifndef BABYLON_MATERIALS_NODE_BLOCKS_MIN_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_MIN_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class MinBlock;
using MinBlockPtr = std::shared_ptr<MinBlock>;

/**
 * @brief Block used to get the min of 2 values.
 */
class BABYLON_SHARED_EXPORT MinBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static MinBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<MinBlock>(new MinBlock(std::forward<Ts>(args)...));
  }
  ~MinBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new MinBlock.
   * @param name defines the block name
   */
  MinBlock(const std::string& name);

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
  MinBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<MinBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<MinBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<MinBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class MinBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_MIN_BLOCK_H
