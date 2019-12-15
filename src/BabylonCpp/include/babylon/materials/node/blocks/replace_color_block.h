#ifndef BABYLON_MATERIALS_NODE_BLOCKS_REPLACE_COLOR_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_REPLACE_COLOR_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ReplaceColorBlock;
using ReplaceColorBlockPtr = std::shared_ptr<ReplaceColorBlock>;

/**
 * @brief Block used to replace a color by another one.
 */
class BABYLON_SHARED_EXPORT ReplaceColorBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ReplaceColorBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ReplaceColorBlock>(new ReplaceColorBlock(std::forward<Ts>(args)...));
  }
  ~ReplaceColorBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ReplaceColorBlock.
   * @param name defines the block name
   */
  ReplaceColorBlock(const std::string& name);

  /**
   * @brief Gets the value input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the reference input component.
   */
  NodeMaterialConnectionPointPtr& get_reference();

  /**
   * @brief Gets the distance input component.
   */
  NodeMaterialConnectionPointPtr& get_distance();

  /**
   * @brief Gets the replacement input component.
   */
  NodeMaterialConnectionPointPtr& get_replacement();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ReplaceColorBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value input component
   */
  ReadOnlyProperty<ReplaceColorBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the reference input component
   */
  ReadOnlyProperty<ReplaceColorBlock, NodeMaterialConnectionPointPtr> reference;

  /**
   * Gets the distance input component
   */
  ReadOnlyProperty<ReplaceColorBlock, NodeMaterialConnectionPointPtr> distance;

  /**
   * Gets the replacement input component
   */
  ReadOnlyProperty<ReplaceColorBlock, NodeMaterialConnectionPointPtr> replacement;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ReplaceColorBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ReplaceColorBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_REPLACE_COLOR_BLOCK_H
