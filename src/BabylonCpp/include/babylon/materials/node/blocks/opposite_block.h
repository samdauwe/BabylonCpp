#ifndef BABYLON_MATERIALS_NODE_BLOCKS_OPPOSITE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_OPPOSITE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class OppositeBlock;
using OppositeBlockPtr = std::shared_ptr<OppositeBlock>;

/**
 * @brief Block used to get the opposite of a value.
 */
class BABYLON_SHARED_EXPORT OppositeBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static OppositeBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<OppositeBlock>(
      new OppositeBlock(std::forward<Ts>(args)...));
  }
  ~OppositeBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new OppositeBlock.
   * @param name defines the block name
   */
  OppositeBlock(const std::string& name);

  /**
   * @brief Gets the input component.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  OppositeBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<OppositeBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<OppositeBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class OppositeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_OPPOSITE_BLOCK_H
