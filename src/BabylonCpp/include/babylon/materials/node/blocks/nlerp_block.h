#ifndef BABYLON_MATERIALS_NODE_BLOCKS_NLERP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_NLERP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class NLerpBlock;
using NLerpBlockPtr = std::shared_ptr<NLerpBlock>;

/**
 * @brief Block used to normalize lerp between 2 values.
 */
class BABYLON_SHARED_EXPORT NLerpBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static NLerpBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<NLerpBlock>(new NLerpBlock(std::forward<Ts>(args)...));
  }
  ~NLerpBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new NLerpBlock.
   * @param name defines the block name
   */
  NLerpBlock(const std::string& name);

  /**
   * @brief Gets the left operand input component.
   */
  NodeMaterialConnectionPointPtr& get_left();

  /**
   * @brief Gets the right operand input component.
   */
  NodeMaterialConnectionPointPtr& get_right();

  /**
   * @brief Gets the gradient operand input component.
   */
  NodeMaterialConnectionPointPtr& get_gradient();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  NLerpBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<NLerpBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<NLerpBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the gradient operand input component
   */
  ReadOnlyProperty<NLerpBlock, NodeMaterialConnectionPointPtr> gradient;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<NLerpBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class NLerpBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_NLERP_BLOCK_H
