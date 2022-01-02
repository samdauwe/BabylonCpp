#ifndef BABYLON_MATERIALS_NODE_BLOCKS_SUBTRACT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_SUBTRACT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class SubtractBlock;
using SubtractBlockPtr = std::shared_ptr<SubtractBlock>;

/**
 * @brief Block used to subtract 2 vectors.
 */
class BABYLON_SHARED_EXPORT SubtractBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static SubtractBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<SubtractBlock>(new SubtractBlock(std::forward<Ts>(args)...));
  }
  ~SubtractBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new SubtractBlock.
   * @param name defines the block name
   */
  SubtractBlock(const std::string& name);

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
  SubtractBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<SubtractBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<SubtractBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<SubtractBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class SubtractBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_SUBTRACT_BLOCK_H
