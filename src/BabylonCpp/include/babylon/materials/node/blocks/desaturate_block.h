#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DESATURATE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DESATURATE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DesaturateBlock;
using DesaturateBlockPtr = std::shared_ptr<DesaturateBlock>;

/**
 * @brief Block used to desaturate a color.
 */
class BABYLON_SHARED_EXPORT DesaturateBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DesaturateBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DesaturateBlock>(new DesaturateBlock(std::forward<Ts>(args)...));
  }
  ~DesaturateBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DesaturateBlock.
   * @param name defines the block name
   */
  DesaturateBlock(const std::string& name);

  /**
   * @brief Gets the color operand input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the level operand input component.
   */
  NodeMaterialConnectionPointPtr& get_level();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  DesaturateBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the color operand input component
   */
  ReadOnlyProperty<DesaturateBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the level operand input component
   */
  ReadOnlyProperty<DesaturateBlock, NodeMaterialConnectionPointPtr> level;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<DesaturateBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class DesaturateBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DESATURATE_BLOCK_H
