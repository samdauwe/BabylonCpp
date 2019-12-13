#ifndef BABYLON_MATERIALS_NODE_BLOCKS_ONE_MINUS_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_ONE_MINUS_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class OneMinusBlock;
using OneMinusBlockPtr = std::shared_ptr<OneMinusBlock>;

/**
 * @brief Block used to get the opposite (1 - x) of a value.
 */
class BABYLON_SHARED_EXPORT OneMinusBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static OneMinusBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<OneMinusBlock>(new OneMinusBlock(std::forward<Ts>(args)...));
  }
  ~OneMinusBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new OneMinusBlock.
   * @param name defines the block name
   */
  OneMinusBlock(const std::string& name);

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
  OneMinusBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<OneMinusBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<OneMinusBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class OneMinusBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_ONE_MINUS_BLOCK_H
