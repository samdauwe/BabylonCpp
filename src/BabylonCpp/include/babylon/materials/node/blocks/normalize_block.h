#ifndef BABYLON_MATERIALS_NODE_BLOCKS_NORMALIZE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_NORMALIZE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class NormalizeBlock;
using NormalizeBlockPtr = std::shared_ptr<NormalizeBlock>;

/**
 * @brief Block used to normalize a vector.
 */
class BABYLON_SHARED_EXPORT NormalizeBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static NormalizeBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<NormalizeBlock>(
      new NormalizeBlock(std::forward<Ts>(args)...));
  }
  ~NormalizeBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new NormalizeBlock.
   * @param name defines the block name
   */
  NormalizeBlock(const std::string& name);

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
  NormalizeBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<NormalizeBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<NormalizeBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class NormalizeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_NORMALIZE_BLOCK_H
