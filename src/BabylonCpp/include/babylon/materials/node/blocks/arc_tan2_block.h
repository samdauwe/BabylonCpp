#ifndef BABYLON_MATERIALS_NODE_BLOCKS_ARC_TAN2_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_ARC_TAN2_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ArcTan2Block;
using ArcTan2BlockPtr = std::shared_ptr<ArcTan2Block>;

/**
 * @brief Block used to compute arc tangent of 2 values.
 */
class BABYLON_SHARED_EXPORT ArcTan2Block : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ArcTan2BlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ArcTan2Block>(new ArcTan2Block(std::forward<Ts>(args)...));
  }
  ~ArcTan2Block() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ArcTan2Block.
   * @param name defines the block name
   */
  ArcTan2Block(const std::string& name);

  /**
   * @brief Gets the x operand input component.
   */
  NodeMaterialConnectionPointPtr& get_x();

  /**
   * @brief Gets the y operand input component.
   */
  NodeMaterialConnectionPointPtr& get_y();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ArcTan2Block& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the x operand input component
   */
  ReadOnlyProperty<ArcTan2Block, NodeMaterialConnectionPointPtr> x;

  /**
   * Gets the y operand input component
   */
  ReadOnlyProperty<ArcTan2Block, NodeMaterialConnectionPointPtr> y;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ArcTan2Block, NodeMaterialConnectionPointPtr> output;

}; // end of class ArcTan2Block

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_ARC_TAN2_BLOCK_H
