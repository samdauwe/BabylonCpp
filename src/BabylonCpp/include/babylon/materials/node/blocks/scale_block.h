#ifndef BABYLON_MATERIALS_NODE_BLOCKS_SCALE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_SCALE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ScaleBlock;
using ScaleBlockPtr = std::shared_ptr<ScaleBlock>;

/**
 * @brief Block used to scale a vector by a float.
 */
class BABYLON_SHARED_EXPORT ScaleBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ScaleBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ScaleBlock>(new ScaleBlock(std::forward<Ts>(args)...));
  }
  ~ScaleBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ScaleBlock.
   * @param name defines the block name
   */
  ScaleBlock(const std::string& name);

  /**
   * @brief Gets the input component.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the factor input component.
   */
  NodeMaterialConnectionPointPtr& get_factor();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ScaleBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input component
   */
  ReadOnlyProperty<ScaleBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the factor input component
   */
  ReadOnlyProperty<ScaleBlock, NodeMaterialConnectionPointPtr> factor;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ScaleBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ScaleBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_SCALE_BLOCK_H
