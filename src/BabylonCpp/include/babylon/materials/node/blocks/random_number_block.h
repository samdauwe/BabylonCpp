#ifndef BABYLON_MATERIALS_NODE_BLOCKS_RANDOM_NUMBER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_RANDOM_NUMBER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class RandomNumberBlock;
using RandomNumberBlockPtr = std::shared_ptr<RandomNumberBlock>;

/**
 * @brief Block used to get a random number.
 */
class BABYLON_SHARED_EXPORT RandomNumberBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static RandomNumberBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<RandomNumberBlock>(new RandomNumberBlock(std::forward<Ts>(args)...));
  }
  ~RandomNumberBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new RandomNumberBlock.
   * @param name defines the block name
   */
  RandomNumberBlock(const std::string& name);

  /**
   * @brief Gets the seed input component.
   */
  NodeMaterialConnectionPointPtr& get_seed();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  RandomNumberBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the seed input component
   */
  ReadOnlyProperty<RandomNumberBlock, NodeMaterialConnectionPointPtr> seed;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<RandomNumberBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class RandomNumberBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_RANDOM_NUMBER_BLOCK_H
