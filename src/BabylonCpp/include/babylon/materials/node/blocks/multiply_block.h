#ifndef BABYLON_MATERIALS_NODE_BLOCKS_MULTIPLY_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_MULTIPLY_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class MultiplyBlock;
using MultiplyBlockPtr = std::shared_ptr<MultiplyBlock>;

/**
 * @brief Block used to multiply 2 values.
 */
class BABYLON_SHARED_EXPORT MultiplyBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static MultiplyBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<MultiplyBlock>(new MultiplyBlock(std::forward<Ts>(args)...));
  }
  ~MultiplyBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new MultiplyBlock.
   * @param name defines the block name
   */
  MultiplyBlock(const std::string& name);

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
  MultiplyBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<MultiplyBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<MultiplyBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<MultiplyBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class MultiplyBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_MULTIPLY_BLOCK_H
