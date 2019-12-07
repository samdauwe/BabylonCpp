#ifndef BABYLON_MATERIALS_NODE_BLOCKS_LERP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_LERP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class LerpBlock;
using LerpBlockPtr = std::shared_ptr<LerpBlock>;

/**
 * @brief Block used to lerp 2 values.
 */
class BABYLON_SHARED_EXPORT LerpBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static LerpBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<LerpBlock>(new LerpBlock(std::forward<Ts>(args)...));
  }
  ~LerpBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new LerpBlock.
   * @param name defines the block name
   */
  LerpBlock(const std::string& name);

  /**
   * @brief Gets the left operand input component.
   */
  NodeMaterialConnectionPointPtr& get_left();

  /**
   * @brief Gets the right operand input component.
   */
  NodeMaterialConnectionPointPtr& get_right();

  /**
   * @brief Gets the right operand input component.
   */
  NodeMaterialConnectionPointPtr& get_gradient();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  LerpBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<LerpBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<LerpBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the gradient operand input component
   */
  ReadOnlyProperty<LerpBlock, NodeMaterialConnectionPointPtr> gradient;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<LerpBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class LerpBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_LERP_BLOCK_H
