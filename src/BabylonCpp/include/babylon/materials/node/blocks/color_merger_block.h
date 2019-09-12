#ifndef BABYLON_MATERIALS_NODE_BLOCKS_COLOR_MERGER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_COLOR_MERGER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ColorMergerBlock;
using ColorMergerBlockPtr = std::shared_ptr<ColorMergerBlock>;

/**
 * @brief Block used to create a Color3/4 out of individual inputs (one for each
 * component)
 */
class BABYLON_SHARED_EXPORT ColorMergerBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ColorMergerBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ColorMergerBlock>(
      new ColorMergerBlock(std::forward<Ts>(args)...));
  }
  ~ColorMergerBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Create a new ColorMergerBlock.
   * @param name defines the block name
   */
  ColorMergerBlock(const std::string& name);

  /**
   * @brief Gets the r component (input).
   */
  NodeMaterialConnectionPointPtr& get_r();

  /**
   * @brief Gets the g component (input).
   */
  NodeMaterialConnectionPointPtr& get_g();

  /**
   * @brief Gets the b component (input).
   */
  NodeMaterialConnectionPointPtr& get_b();

  /**
   * @brief Gets the a component (input).
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Gets the rgba component (output).
   */
  NodeMaterialConnectionPointPtr& get_rgba();

  /**
   * @brief Gets the rgb component (output).
   */
  NodeMaterialConnectionPointPtr& get_rgb();

  /**
   * @brief Hidden
   */
  ColorMergerBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the r component (input)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g component (input)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b component (input)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets the a component (input)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> a;

  /**
   * Gets the rgba component (output)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb component (output)
   */
  ReadOnlyProperty<ColorMergerBlock, NodeMaterialConnectionPointPtr> rgb;

}; // end of class ColorMergerBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_COLOR_MERGER_BLOCK_H
