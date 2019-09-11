#ifndef BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class AddBlock;
using AddBlockPtr = std::shared_ptr<AddBlock>;

/**
 * @brief Block used to add 2 vectors.
 */
class BABYLON_SHARED_EXPORT AddBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static AddBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<AddBlock>(new AddBlock(std::forward<Ts>(args)...));
  }
  ~AddBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new AddBlock.
   * @param name defines the block name
   */
  AddBlock(const std::string& name);

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
  AddBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<AddBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<AddBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<AddBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class AddBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H
