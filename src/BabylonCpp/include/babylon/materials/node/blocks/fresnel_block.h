#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRESNEL_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRESNEL_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class FresnelBlock;
using FresnelBlockPtr = std::shared_ptr<FresnelBlock>;

/**
 * @brief Block used to compute fresnel value.
 */
class BABYLON_SHARED_EXPORT FresnelBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FresnelBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FresnelBlock>(new FresnelBlock(std::forward<Ts>(args)...));
  }
  ~FresnelBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

protected:
  /**
   * @brief Creates a new FresnelBlock.
   * @param name defines the block name
   */
  FresnelBlock(const std::string& name);

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal();

  /**
   * @brief Gets the view direction input component.
   */
  NodeMaterialConnectionPointPtr& get_viewDirection();

  /**
   * @brief Gets the bias input component.
   */
  NodeMaterialConnectionPointPtr& get_bias();

  /**
   * @brief Gets the camera (or eye) position component.
   */
  NodeMaterialConnectionPointPtr& get_power();

  /**
   * @brief Gets the fresnel output component.
   */
  NodeMaterialConnectionPointPtr& get_fresnel();

  /**
   * @brief Hidden
   */
  FresnelBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<FresnelBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the view direction input component
   */
  ReadOnlyProperty<FresnelBlock, NodeMaterialConnectionPointPtr> viewDirection;

  /**
   * Gets the bias input component
   */
  ReadOnlyProperty<FresnelBlock, NodeMaterialConnectionPointPtr> bias;

  /**
   * Gets the camera (or eye) position component
   */
  ReadOnlyProperty<FresnelBlock, NodeMaterialConnectionPointPtr> power;

  /**
   * Gets the fresnel output component
   */
  ReadOnlyProperty<FresnelBlock, NodeMaterialConnectionPointPtr> fresnel;

}; // end of class FresnelBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRESNEL_BLOCK_H
