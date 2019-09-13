#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VIEW_DIRECTION_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VIEW_DIRECTION_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ViewDirectionBlock;
using ViewDirectionBlockPtr = std::shared_ptr<ViewDirectionBlock>;

/**
 * @brief Block used to get the view direction.
 */
class BABYLON_SHARED_EXPORT ViewDirectionBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ViewDirectionBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ViewDirectionBlock>(
      new ViewDirectionBlock(std::forward<Ts>(args)...));
  }
  ~ViewDirectionBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

protected:
  /**
   * @brief Creates a new ViewDirectionBlock.
   * @param name defines the block name
   */
  ViewDirectionBlock(const std::string& name);

  /**
   * @brief Gets the world position component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the camera position component.
   */
  NodeMaterialConnectionPointPtr& get_cameraPosition();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ViewDirectionBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the world position component
   */
  ReadOnlyProperty<ViewDirectionBlock, NodeMaterialConnectionPointPtr>
    worldPosition;

  /**
   * Gets the camera position component
   */
  ReadOnlyProperty<ViewDirectionBlock, NodeMaterialConnectionPointPtr>
    cameraPosition;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ViewDirectionBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ViewDirectionBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VIEW_DIRECTION_BLOCK_H
