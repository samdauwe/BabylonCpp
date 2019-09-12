#ifndef BABYLON_MATERIALS_NODE_BLOCKS_CROSS_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_CROSS_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class CrossBlock;
using CrossBlockPtr = std::shared_ptr<CrossBlock>;

/**
 * @brief Block used to apply a cross product between 2 vectors.
 */
class BABYLON_SHARED_EXPORT CrossBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static CrossBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<CrossBlock>(
      new CrossBlock(std::forward<Ts>(args)...));
  }
  ~CrossBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new CrossBlock.
   * @param name defines the block name
   */
  CrossBlock(const std::string& name);

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
  CrossBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<CrossBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<CrossBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<CrossBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class CrossBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_CROSS_BLOCK_H
