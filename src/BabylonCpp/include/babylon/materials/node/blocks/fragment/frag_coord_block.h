#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAG_COORD_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAG_COORD_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class FragCoordBlock;
using FragCoordBlockPtr = std::shared_ptr<FragCoordBlock>;

/**
 * @brief Block used to make gl_FragCoord available.
 */
class BABYLON_SHARED_EXPORT FragCoordBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FragCoordBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FragCoordBlock>(new FragCoordBlock(std::forward<Ts>(args)...));
  }
  ~FragCoordBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new FragCoordBlock.
   * @param name defines the block name
   */
  FragCoordBlock(const std::string& name);

  /**
   * @brief Gets the xy component.
   */
  NodeMaterialConnectionPointPtr& get_xy();

  /**
   * @brief Gets the xyz component.
   */
  NodeMaterialConnectionPointPtr& get_xyz();

  /**
   * @brief Gets the xyzw component.
   */
  NodeMaterialConnectionPointPtr& get_xyzw();

  /**
   * @brief Gets the x component.
   */
  NodeMaterialConnectionPointPtr& get_x();

  /**
   * @brief Gets the y component.
   */
  NodeMaterialConnectionPointPtr& get_y();

  /**
   * @brief Gets the z component.
   */
  NodeMaterialConnectionPointPtr& get_z();

  /**
   * @brief Gets the w component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  std::string writeOutputs(NodeMaterialBuildState& state);

  /**
   * @brief Hidden
   */
  FragCoordBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the xy component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> xy;

  /**
   * Gets the xyz component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> xyz;

  /**
   * Gets the xyzw component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> xyzw;

  /**
   * Gets the x component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> x;

  /**
   * Gets the y component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> y;

  /**
   * Gets the z component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> z;

  /**
   * Gets the w component
   */
  ReadOnlyProperty<FragCoordBlock, NodeMaterialConnectionPointPtr> output;

private:
  std::string _varName;
  Scene* scene;

}; // end of class FragCoordBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAG_COORD_BLOCK_H
