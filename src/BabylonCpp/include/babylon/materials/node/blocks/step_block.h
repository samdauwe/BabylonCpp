#ifndef BABYLON_MATERIALS_NODE_BLOCKS_STEP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_STEP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class StepBlock;
using StepBlockPtr = std::shared_ptr<StepBlock>;

/**
 * @brief Block used to step a value.
 */
class BABYLON_SHARED_EXPORT StepBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static StepBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<StepBlock>(new StepBlock(std::forward<Ts>(args)...));
  }
  ~StepBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new StepBlock.
   * @param name defines the block name
   */
  StepBlock(const std::string& name);

  /**
   * @brief Gets the value operand input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the edge operand input component.
   */
  NodeMaterialConnectionPointPtr& get_edge();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  StepBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value operand input component
   */
  ReadOnlyProperty<StepBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the edge operand input component
   */
  ReadOnlyProperty<StepBlock, NodeMaterialConnectionPointPtr> edge;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<StepBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class StepBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_STEP_BLOCK_H
