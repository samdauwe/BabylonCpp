#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class FragmentOutputBlock;
using FragmentOutputBlockPtr = std::shared_ptr<FragmentOutputBlock>;

/**
 * @brief Block used to output the final color.
 */
class BABYLON_SHARED_EXPORT FragmentOutputBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FragmentOutputBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FragmentOutputBlock>(new FragmentOutputBlock(std::forward<Ts>(args)...));
  }
  ~FragmentOutputBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new FragmentOutputBlock.
   * @param name defines the block name
   */
  FragmentOutputBlock(const std::string& name);

  /**
   * @brief Gets the rgba input component.
   */
  NodeMaterialConnectionPointPtr& get_rgba();

  /**
   * @brief Gets the rgb input component.
   */
  NodeMaterialConnectionPointPtr& get_rgb();

  /**
   * @brief Gets the a input component.
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Hidden
   */
  FragmentOutputBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the rgba input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the a input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> a;

}; // end of class FragmentOutputBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H
