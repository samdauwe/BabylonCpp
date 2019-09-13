#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_MERGER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_MERGER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class VectorMergerBlock;
using VectorMergerBlockPtr = std::shared_ptr<VectorMergerBlock>;

/**
 * @brief Block used to create a Vector2/3/4 out of individual inputs (one for
 * each component).
 */
class BABYLON_SHARED_EXPORT VectorMergerBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static VectorMergerBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<VectorMergerBlock>(
      new VectorMergerBlock(std::forward<Ts>(args)...));
  }
  ~VectorMergerBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Create a new VectorMergerBlock.
   * @param name defines the block name
   */
  VectorMergerBlock(const std::string& name);

  /**
   * @brief Gets the x component (input).
   */
  NodeMaterialConnectionPointPtr& get_x();

  /**
   * @brief Gets the y component (input).
   */
  NodeMaterialConnectionPointPtr& get_y();

  /**
   * @brief Gets the z component (input).
   */
  NodeMaterialConnectionPointPtr& get_z();

  /**
   * @brief Gets the w component (input).
   */
  NodeMaterialConnectionPointPtr& get_w();

  /**
   * @brief Gets the xyzw component (output).
   */
  NodeMaterialConnectionPointPtr& get_xyzw();

  /**
   * @brief Gets the xyz component (output).
   */
  NodeMaterialConnectionPointPtr& get_xyz();

  /**
   * @brief Gets the xy component (output).
   */
  NodeMaterialConnectionPointPtr& get_xy();

  /**
   * @brief Hidden
   */
  VectorMergerBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the x component (input)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> x;

  /**
   * Gets the y component (input)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> y;

  /**
   * Gets the z component (input)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> z;

  /**
   * Gets the w component (input)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> w;

  /**
   * Gets the xyzw component (output)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> xyzw;

  /**
   * Gets the xyz component (output)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> xyz;

  /**
   * Gets the xy component (output)
   */
  ReadOnlyProperty<VectorMergerBlock, NodeMaterialConnectionPointPtr> xy;

}; // end of class VectorMergerBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VECTOR_MERGER_BLOCK_H
