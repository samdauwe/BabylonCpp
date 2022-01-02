#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DOT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DOT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DotBlock;
using DotBlockPtr = std::shared_ptr<DotBlock>;

/**
 * @brief Block used to apply a dot product between 2 vectors.
 */
class BABYLON_SHARED_EXPORT DotBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DotBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DotBlock>(new DotBlock(std::forward<Ts>(args)...));
  }
  ~DotBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DotBlock.
   * @param name defines the block name
   */
  DotBlock(const std::string& name);

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
  DotBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the left operand input component
   */
  ReadOnlyProperty<DotBlock, NodeMaterialConnectionPointPtr> left;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<DotBlock, NodeMaterialConnectionPointPtr> right;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<DotBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class DotBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DOT_BLOCK_H
