#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DISCARD_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DISCARD_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class DiscardBlock;
using DiscardBlockPtr = std::shared_ptr<DiscardBlock>;

/**
 * @brief Block used to discard a pixel if a value is smaller than a cutoff.
 */
class BABYLON_SHARED_EXPORT DiscardBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static DiscardBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<DiscardBlock>(new DiscardBlock(std::forward<Ts>(args)...));
  }
  ~DiscardBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new DiscardBlock.
   * @param name defines the block name
   */
  DiscardBlock(const std::string& name);

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the cutoff input component.
   */
  NodeMaterialConnectionPointPtr& get_cutoff();

  /**
   * @brief Hidden
   */
  DiscardBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the color input component
   */
  ReadOnlyProperty<DiscardBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the cutoff input component
   */
  ReadOnlyProperty<DiscardBlock, NodeMaterialConnectionPointPtr> cutoff;

}; // end of class DiscardBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_DISCARD_BLOCK_H
