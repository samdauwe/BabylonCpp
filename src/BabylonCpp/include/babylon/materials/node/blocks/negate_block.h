#ifndef BABYLON_MATERIALS_NODE_BLOCKS_NEGATE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_NEGATE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class NegateBlock;
using NegateBlockPtr = std::shared_ptr<NegateBlock>;

/**
 * @brief Block used to get negative version of a value (i.e. x * -1).
 */
class BABYLON_SHARED_EXPORT NegateBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static NegateBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<NegateBlock>(new NegateBlock(std::forward<Ts>(args)...));
  }
  ~NegateBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new NegateBlock.
   * @param name defines the block name
   */
  NegateBlock(const std::string& name);

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
  NegateBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the value input component
   */
  ReadOnlyProperty<NegateBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<NegateBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class NegateBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_NEGATE_BLOCK_H
