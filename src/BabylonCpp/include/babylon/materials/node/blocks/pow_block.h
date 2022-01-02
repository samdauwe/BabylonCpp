#ifndef BABYLON_MATERIALS_NODE_BLOCKS_POW_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_POW_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class PowBlock;
using PowBlockPtr = std::shared_ptr<PowBlock>;

/**
 * @brief Block used to get the value of the first parameter raised to the power of the second.
 */
class BABYLON_SHARED_EXPORT PowBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static PowBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<PowBlock>(new PowBlock(std::forward<Ts>(args)...));
  }
  ~PowBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new PowBlock.
   * @param name defines the block name
   */
  PowBlock(const std::string& name);

  /**
   * @brief Gets the value operand input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the power operand input component.
   */
  NodeMaterialConnectionPointPtr& get_power();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  PowBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value operand input component
   */
  ReadOnlyProperty<PowBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the power operand input component
   */
  ReadOnlyProperty<PowBlock, NodeMaterialConnectionPointPtr> power;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<PowBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class PowBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_POW_BLOCK_H
