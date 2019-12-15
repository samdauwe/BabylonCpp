#ifndef BABYLON_MATERIALS_NODE_BLOCKS_SMOOTH_STEP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_SMOOTH_STEP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class SmoothStepBlock;
using SmoothStepBlockPtr = std::shared_ptr<SmoothStepBlock>;

/**
 * @brief Block used to smooth step a value.
 */
class BABYLON_SHARED_EXPORT SmoothStepBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static SmoothStepBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<SmoothStepBlock>(new SmoothStepBlock(std::forward<Ts>(args)...));
  }
  ~SmoothStepBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new SmoothStepBlock.
   * @param name defines the block name
   */
  SmoothStepBlock(const std::string& name);

  /**
   * @brief  Gets the value operand input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the first edge operand input component.
   */
  NodeMaterialConnectionPointPtr& get_edge0();

  /**
   * @brief Gets the second edge operand input component.
   */
  NodeMaterialConnectionPointPtr& get_edge1();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  SmoothStepBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   *  Gets the value operand input component
   */
  ReadOnlyProperty<SmoothStepBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the first edge operand input component
   */
  ReadOnlyProperty<SmoothStepBlock, NodeMaterialConnectionPointPtr> edge0;

  /**
   * Gets the second edge operand input component
   */
  ReadOnlyProperty<SmoothStepBlock, NodeMaterialConnectionPointPtr> edge1;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<SmoothStepBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class SmoothStepBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_SMOOTH_STEP_BLOCK_H
