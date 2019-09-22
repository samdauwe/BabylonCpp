#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_VERTEX_OUTPUT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_VERTEX_OUTPUT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class VertexOutputBlock;
using VertexOutputBlockPtr = std::shared_ptr<VertexOutputBlock>;

/**
 * @brief Block used to output the vertex position.
 */
class BABYLON_SHARED_EXPORT VertexOutputBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static VertexOutputBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<VertexOutputBlock>(
      new VertexOutputBlock(std::forward<Ts>(args)...));
  }
  ~VertexOutputBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new VertexOutputBlock.
   * @param name defines the block name
   */
  VertexOutputBlock(const std::string& name);

  /**
   * @brief Gets the vector input component.
   */
  NodeMaterialConnectionPointPtr& get_vector();

  /**
   * @brief Hidden
   */
  VertexOutputBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the vector input component
   */
  ReadOnlyProperty<VertexOutputBlock, NodeMaterialConnectionPointPtr> vector;

}; // end of class VertexOutputBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_VERTEX_OUTPUT_BLOCK_H
