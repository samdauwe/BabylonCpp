#ifndef BABYLON_MATERIALS_NODE_BLOCKS_REFRACT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_REFRACT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class RefractBlock;
using RefractBlockPtr = std::shared_ptr<RefractBlock>;

/**
 * @brief Block used to get the refracted vector from a direction and a normal.
 */
class BABYLON_SHARED_EXPORT RefractBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static RefractBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<RefractBlock>(new RefractBlock(std::forward<Ts>(args)...));
  }
  ~RefractBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new RefractBlock.
   * @param name defines the block name
   */
  RefractBlock(const std::string& name);

  /**
   * @brief Gets the incident component.
   */
  NodeMaterialConnectionPointPtr& get_incident();

  /**
   * @brief Gets the normal component.
   */
  NodeMaterialConnectionPointPtr& get_normal();

  /**
   * @brief Gets the index of refraction component.
   */
  NodeMaterialConnectionPointPtr& get_ior();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  RefractBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the incident component
   */
  ReadOnlyProperty<RefractBlock, NodeMaterialConnectionPointPtr> incident;

  /**
   * Gets the normal component
   */
  ReadOnlyProperty<RefractBlock, NodeMaterialConnectionPointPtr> normal;

  /**
   * Gets the index of refraction component
   */
  ReadOnlyProperty<RefractBlock, NodeMaterialConnectionPointPtr> ior;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<RefractBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class RefractBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_REFRACT_BLOCK_H
