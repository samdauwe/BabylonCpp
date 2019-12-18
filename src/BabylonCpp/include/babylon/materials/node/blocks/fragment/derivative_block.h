#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DERIVATIVE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DERIVATIVE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DerivativeBlock;
using DerivativeBlockPtr = std::shared_ptr<DerivativeBlock>;

/**
 * @brief Block used to get the derivative value on x and y of a given input.
 */
class BABYLON_SHARED_EXPORT DerivativeBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DerivativeBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DerivativeBlock>(new DerivativeBlock(std::forward<Ts>(args)...));
  }
  ~DerivativeBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DerivativeBlock.
   * @param name defines the block name
   */
  DerivativeBlock(const std::string& name);

  /**
   * @brief Gets the input component.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the derivative output on x.
   */
  NodeMaterialConnectionPointPtr& get_dx();

  /**
   * @brief Gets the derivative output on y.
   */
  NodeMaterialConnectionPointPtr& get_dy();

  /**
   * @brief Hidden
   */
  DerivativeBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<DerivativeBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the derivative output on x
   */
  ReadOnlyProperty<DerivativeBlock, NodeMaterialConnectionPointPtr> dx;

  /**
   * Gets the derivative output on y
   */
  ReadOnlyProperty<DerivativeBlock, NodeMaterialConnectionPointPtr> dy;

}; // end of class DerivativeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DERIVATIVE_BLOCK_H
