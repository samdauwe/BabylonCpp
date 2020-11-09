#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_FOG_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_FOG_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class FogBlock;
using FogBlockPtr = std::shared_ptr<FogBlock>;

/**
 * @brief Block used to add support for scene fog.
 */
class BABYLON_SHARED_EXPORT FogBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FogBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FogBlock>(new FogBlock(std::forward<Ts>(args)...));
  }
  ~FogBlock() override;

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

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      SubMesh* subMesh = nullptr) override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

protected:
  /**
   * @brief Creates a new FogBlock.
   * @param name defines the block name
   */
  FogBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the view input component.
   */
  NodeMaterialConnectionPointPtr& get_view();

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the fog color input component.
   */
  NodeMaterialConnectionPointPtr& get_fogColor();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  FogBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<FogBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the view input component
   */
  ReadOnlyProperty<FogBlock, NodeMaterialConnectionPointPtr> view;

  /**
   * Gets the color input component
   */
  ReadOnlyProperty<FogBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the fog color input component
   */
  ReadOnlyProperty<FogBlock, NodeMaterialConnectionPointPtr> fogColor;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<FogBlock, NodeMaterialConnectionPointPtr> output;

private:
  std::string _fogDistanceName;
  std::string _fogParameters;

}; // end of class FogBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_FOG_BLOCK_H
