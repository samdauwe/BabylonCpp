#ifndef BABYLON_MATERIALS_NODE_BLOCKS_MOD_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_MOD_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ModBlock;
using ModBlockPtr = std::shared_ptr<ModBlock>;

/**
 * @brief Block used to compute value of one parameter modulo another.
 */
class BABYLON_SHARED_EXPORT ModBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ModBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ModBlock>(new ModBlock(std::forward<Ts>(args)...));
  }
  ~ModBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ModBlock.
   * @param name defines the block name
   */
  ModBlock(const std::string& name);

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
  ModBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<ModBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<ModBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ModBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ModBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_MOD_BLOCK_H
