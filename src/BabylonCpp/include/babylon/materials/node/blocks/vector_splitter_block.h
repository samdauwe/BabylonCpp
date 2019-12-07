#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_SPLITTER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_SPLITTER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class VectorSplitterBlock;
using VectorSplitterBlockPtr = std::shared_ptr<VectorSplitterBlock>;

/**
 * @brief Block used to expand a Vector3/4 into 4 outputs (one for each component).
 */
class BABYLON_SHARED_EXPORT VectorSplitterBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static VectorSplitterBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<VectorSplitterBlock>(new VectorSplitterBlock(std::forward<Ts>(args)...));
  }
  ~VectorSplitterBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Create a new VectorSplitterBlock.
   * @param name defines the block name
   */
  VectorSplitterBlock(const std::string& name);

  /**
   * @brief Gets the xyzw component (input).
   */
  NodeMaterialConnectionPointPtr& get_xyzw();

  /**
   * @brief Gets the xyz component (input).
   */
  NodeMaterialConnectionPointPtr& get_xyzIn();

  /**
   * @brief Gets the xy component (input).
   */
  NodeMaterialConnectionPointPtr& get_xyIn();

  /**
   * @brief Gets the xyz component (output).
   */
  NodeMaterialConnectionPointPtr& get_xyzOut();

  /**
   * @brief Gets the xy component (output).
   */
  NodeMaterialConnectionPointPtr& get_xyOut();

  /**
   * @brief Gets the x component (output).
   */
  NodeMaterialConnectionPointPtr& get_x();

  /**
   * @brief Gets the y component (output).
   */
  NodeMaterialConnectionPointPtr& get_y();

  /**
   * @brief Gets the z component (output).
   */
  NodeMaterialConnectionPointPtr& get_z();

  /**
   * @brief Gets the w component (output).
   */
  NodeMaterialConnectionPointPtr& get_w();

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
  VectorSplitterBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the xyzw component (input)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> xyzw;

  /**
   * Gets the xyz component (input)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> xyzIn;

  /**
   * Gets the xy component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> xyIn;

  /**
   * Gets the xyz component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> xyzOut;

  /**
   * Gets the xy component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> xyOut;

  /**
   * Gets the x component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> x;

  /**
   * Gets the y component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> y;

  /**
   * Gets the z component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> z;

  /**
   * Gets the w component (output)
   */
  ReadOnlyProperty<VectorSplitterBlock, NodeMaterialConnectionPointPtr> w;

}; // end of class VectorSplitterBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_SPLITTER_BLOCK_H
