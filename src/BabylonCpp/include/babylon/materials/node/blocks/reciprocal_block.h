#ifndef BABYLON_MATERIALS_NODE_BLOCKS_RECIPROCAL_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_RECIPROCAL_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ReciprocalBlock;
using ReciprocalBlockPtr = std::shared_ptr<ReciprocalBlock>;

/**
 * @brief Block used to get the reciprocal (1 / x) of a value.
 */
class BABYLON_SHARED_EXPORT ReciprocalBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ReciprocalBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ReciprocalBlock>(new ReciprocalBlock(std::forward<Ts>(args)...));
  }
  ~ReciprocalBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ReciprocalBlock.
   * @param name defines the block name
   */
  ReciprocalBlock(const std::string& name);

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
  ReciprocalBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<ReciprocalBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ReciprocalBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ReciprocalBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_RECIPROCAL_BLOCK_H
