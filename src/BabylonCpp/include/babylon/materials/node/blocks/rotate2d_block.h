#ifndef BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class Rotate2dBlock;
using Rotate2dBlockPtr = std::shared_ptr<Rotate2dBlock>;

/**
 * @brief Block used to rotate a 2d vector by a given angle.
 */
class BABYLON_SHARED_EXPORT Rotate2dBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static Rotate2dBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<Rotate2dBlock>(new Rotate2dBlock(std::forward<Ts>(args)...));
  }
  ~Rotate2dBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& nodeMaterial) override;

protected:
  /**
   * @brief Creates a new Rotate2dBlock.
   * @param name defines the block name
   */
  Rotate2dBlock(const std::string& name);

  /**
   * @brief Gets the input vector.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the input angle.
   */
  NodeMaterialConnectionPointPtr& get_angle();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  Rotate2dBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the input vector
   */
  ReadOnlyProperty<Rotate2dBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the input angle
   */
  ReadOnlyProperty<Rotate2dBlock, NodeMaterialConnectionPointPtr> angle;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<Rotate2dBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class Rotate2dBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_ADD_BLOCK_H
