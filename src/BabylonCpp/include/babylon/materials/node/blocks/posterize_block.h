#ifndef BABYLON_MATERIALS_NODE_BLOCKS_POSTERIZE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_POSTERIZE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class PosterizeBlock;
using PosterizeBlockPtr = std::shared_ptr<PosterizeBlock>;

/**
 * @brief Block used to posterize a value.
 * @see https://en.wikipedia.org/wiki/Posterization
 */
class BABYLON_SHARED_EXPORT PosterizeBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static PosterizeBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<PosterizeBlock>(new PosterizeBlock(std::forward<Ts>(args)...));
  }
  ~PosterizeBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new PosterizeBlock.
   * @param name defines the block name
   */
  PosterizeBlock(const std::string& name);

  /**
   * @brief Gets the value input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the steps input component.
   */
  NodeMaterialConnectionPointPtr& get_steps();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  PosterizeBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value input component
   */
  ReadOnlyProperty<PosterizeBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the steps input component
   */
  ReadOnlyProperty<PosterizeBlock, NodeMaterialConnectionPointPtr> steps;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<PosterizeBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class PosterizeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_POSTERIZE_BLOCK_H
