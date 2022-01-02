#ifndef BABYLON_MATERIALS_NODE_BLOCKS_NORMAL_BLEND_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_NORMAL_BLEND_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class NormalBlendBlock;
using NormalBlendBlockPtr = std::shared_ptr<NormalBlendBlock>;

/**
 * @brief Block used to blend normals.
 */
class BABYLON_SHARED_EXPORT NormalBlendBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static NormalBlendBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<NormalBlendBlock>(new NormalBlendBlock(std::forward<Ts>(args)...));
  }
  ~NormalBlendBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new NormalBlendBlock.
   * @param name defines the block name
   */
  NormalBlendBlock(const std::string& name);

  /**
   * @brief Gets the first input component.
   */
  NodeMaterialConnectionPointPtr& get_normalMap0();

  /**
   * @brief Gets the second input component.
   */
  NodeMaterialConnectionPointPtr& get_normalMap1();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  NormalBlendBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the first input component
   */
  ReadOnlyProperty<NormalBlendBlock, NodeMaterialConnectionPointPtr> normalMap0;

  /**
   * Gets the second input component
   */
  ReadOnlyProperty<NormalBlendBlock, NodeMaterialConnectionPointPtr> normalMap1;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<NormalBlendBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class NormalBlendBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_NORMAL_BLEND_BLOCK_H
