#ifndef BABYLON_MATERIALS_NODE_BLOCKS_MATRIX_BUILDER_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_MATRIX_BUILDER_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(MatrixBuilderBlock)

/**
 * @brief Block used to build a matrix from 4 Vector4.
 */
class BABYLON_SHARED_EXPORT MatrixBuilderBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static MatrixBuilderBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<MatrixBuilderBlock>(new MatrixBuilderBlock(std::forward<Ts>(args)...));
  }
  ~MatrixBuilderBlock() override; // = default

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new MatrixBuilder.
   * @param name defines the block name
   */
  MatrixBuilderBlock(const std::string& name);

  /**
   * @brief Gets the row0 vector.
   */
  NodeMaterialConnectionPointPtr& get_row0();

  /**
   * @brief Gets the row1 vector.
   */
  NodeMaterialConnectionPointPtr& get_row1();

  /**
   * @brief Gets the row2 vector.
   */
  NodeMaterialConnectionPointPtr& get_row2();

  /**
   * @brief Gets the row3 vector.
   */
  NodeMaterialConnectionPointPtr& get_row3();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& nodeMaterial) override;

  /**
   * @brief Hidden
   */
  MatrixBuilderBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the row0 vector
   */
  ReadOnlyProperty<MatrixBuilderBlock, NodeMaterialConnectionPointPtr> row0;

  /**
   * Gets the row1 vector
   */
  ReadOnlyProperty<MatrixBuilderBlock, NodeMaterialConnectionPointPtr> row1;

  /**
   * Gets the row2 vector
   */
  ReadOnlyProperty<MatrixBuilderBlock, NodeMaterialConnectionPointPtr> row2;

  /**
   * Gets the row3 vector
   */
  ReadOnlyProperty<MatrixBuilderBlock, NodeMaterialConnectionPointPtr> row3;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<MatrixBuilderBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class MatrixBuilderBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_MATRIX_BUILDER_BLOCK_H
