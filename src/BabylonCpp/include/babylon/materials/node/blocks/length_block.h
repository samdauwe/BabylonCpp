#ifndef BABYLON_MATERIALS_NODE_BLOCKS_LENGTH_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_LENGTH_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class LengthBlock;
using LengthBlockPtr = std::shared_ptr<LengthBlock>;

/**
 * @brief Block used to get the length of a vector.
 */
class BABYLON_SHARED_EXPORT LengthBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static LengthBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<LengthBlock>(new LengthBlock(std::forward<Ts>(args)...));
  }
  ~LengthBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new LengthBlock.
   * @param name defines the block name
   */
  LengthBlock(const std::string& name);

  /**
   * @brief Gets the value input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  LengthBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value input component
   */
  ReadOnlyProperty<LengthBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<LengthBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class LengthBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_LENGTH_BLOCK_H
