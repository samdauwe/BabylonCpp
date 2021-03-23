#ifndef BABYLON_MATERIALS_NODE_BLOCKS_COLOR_SPLITTER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_COLOR_SPLITTER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(ColorSplitterBlock)

/**
 * @brief Block used to expand a Color3/4 into 4 outputs (one for each component).
 */
class BABYLON_SHARED_EXPORT ColorSplitterBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ColorSplitterBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ColorSplitterBlock>(new ColorSplitterBlock(std::forward<Ts>(args)...));
  }
  ~ColorSplitterBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Create a new ColorSplitterBlock.
   * @param name defines the block name
   */
  ColorSplitterBlock(const std::string& name);

  /**
   * @brief Gets the rgba component (input).
   */
  NodeMaterialConnectionPointPtr& get_rgba();

  /**
   * @brief Gets the rgb component (input).
   */
  NodeMaterialConnectionPointPtr& get_rgbIn();

  /**
   * @brief Gets the rgb component (output).
   */
  NodeMaterialConnectionPointPtr& get_rgbOut();

  /**
   * @brief Gets the r component (output).
   */
  NodeMaterialConnectionPointPtr& get_r();

  /**
   * @brief Gets the g component (output).
   */
  NodeMaterialConnectionPointPtr& get_g();

  /**
   * @brief Gets the b component (output).
   */
  NodeMaterialConnectionPointPtr& get_b();

  /**
   * @brief Gets the a component (output).
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Hidden
   */
  std::string _inputRename(const std::string& name) override;

  /**
   * @brief Hidden
   */
  std::string _outputRename(const std::string& name) override;

  /**
   * @brief Hidden
   */
  ColorSplitterBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the rgba component (input)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb component (input)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> rgbIn;

  /**
   * Gets the rgb component (output)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> rgbOut;

  /**
   * Gets the r component (output)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g component (output)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b component (output)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets the a component (output)
   */
  ReadOnlyProperty<ColorSplitterBlock, NodeMaterialConnectionPointPtr> a;

}; // end of class ColorSplitterBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_COLOR_SPLITTER_BLOCK_H
