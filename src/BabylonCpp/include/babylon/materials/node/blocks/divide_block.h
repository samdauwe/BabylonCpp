#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DIVIDE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DIVIDE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DivideBlock;
using DivideBlockPtr = std::shared_ptr<DivideBlock>;

/**
 * @brief Block used to divide 2 vectors.
 */
class BABYLON_SHARED_EXPORT DivideBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DivideBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DivideBlock>(
      new DivideBlock(std::forward<Ts>(args)...));
  }
  ~DivideBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DivideBlock.
   * @param name defines the block name
   */
  DivideBlock(const std::string& name);

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
  DivideBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<DivideBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<DivideBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<DivideBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class DivideBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DIVIDE_BLOCK_H
