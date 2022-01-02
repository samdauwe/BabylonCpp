#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_SCREEN_SIZE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_SCREEN_SIZE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ScreenSizeBlock;
using ScreenSizeBlockPtr = std::shared_ptr<ScreenSizeBlock>;

/**
 * @brief Block used to get the screen sizes.
 */
class BABYLON_SHARED_EXPORT ScreenSizeBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ScreenSizeBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ScreenSizeBlock>(new ScreenSizeBlock(std::forward<Ts>(args)...));
  }
  ~ScreenSizeBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   * @param subMesh defines the submesh that will be rendered
   */
  void bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

protected:
  /**
   * @brief Creates a new ScreenSizeBlock.
   * @param name defines the block name
   */
  ScreenSizeBlock(const std::string& name);

  /**
   * @brief Gets the xy component.
   */
  NodeMaterialConnectionPointPtr& get_xy();

  /**
   * @brief Gets the x component.
   */
  NodeMaterialConnectionPointPtr& get_x();

  /**
   * @brief Gets the y component.
   */
  NodeMaterialConnectionPointPtr& get_y();

  /**
   * @brief Hidden
   */
  std::string writeOutputs(NodeMaterialBuildState& state, const std::string& varName);

  /**
   * @brief Hidden
   */
  ScreenSizeBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the xy component
   */
  ReadOnlyProperty<ScreenSizeBlock, NodeMaterialConnectionPointPtr> xy;

  /**
   * Gets the x component
   */
  ReadOnlyProperty<ScreenSizeBlock, NodeMaterialConnectionPointPtr> x;

  /**
   * Gets the y component
   */
  ReadOnlyProperty<ScreenSizeBlock, NodeMaterialConnectionPointPtr> y;

private:
  std::string _varName;
  Scene* _scene;

}; // end of class ScreenSizeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_SCREEN_SIZE_BLOCK_H
