#ifndef BABYLON_MATERIALS_NODE_BLOCKS_REFLECT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_REFLECT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ReflectBlock;
using ReflectBlockPtr = std::shared_ptr<ReflectBlock>;

/**
 * @brief Block used to get the reflected vector from a direction and a normal.
 */
class BABYLON_SHARED_EXPORT ReflectBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ReflectBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ReflectBlock>(new ReflectBlock(std::forward<Ts>(args)...));
  }
  ~ReflectBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new ReflectBlock.
   * @param name defines the block name
   */
  ReflectBlock(const std::string& name);

  /**
   * @brief Gets the incident component.
   */
  NodeMaterialConnectionPointPtr& get_incident();

  /**
   * @brief Gets the normal component.
   */
  NodeMaterialConnectionPointPtr& get_normal();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ReflectBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the incident component
   */
  ReadOnlyProperty<ReflectBlock, NodeMaterialConnectionPointPtr> incident;

  /**
   * Gets the normal component
   */
  ReadOnlyProperty<ReflectBlock, NodeMaterialConnectionPointPtr> normal;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ReflectBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ReflectBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_REFLECT_BLOCK_H
