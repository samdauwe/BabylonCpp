#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DISTANCE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DISTANCE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DistanceBlock;
using DistanceBlockPtr = std::shared_ptr<DistanceBlock>;

/**
 * @brief Block used to get the distance between 2 values.
 */
class BABYLON_SHARED_EXPORT DistanceBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DistanceBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DistanceBlock>(new DistanceBlock(std::forward<Ts>(args)...));
  }
  ~DistanceBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DistanceBlock.
   * @param name defines the block name
   */
  DistanceBlock(const std::string& name);

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
  DistanceBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<DistanceBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<DistanceBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<DistanceBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class DistanceBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DISTANCE_BLOCK_H
