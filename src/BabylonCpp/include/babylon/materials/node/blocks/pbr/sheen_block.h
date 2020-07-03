#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_SHEEN_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_SHEEN_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ReflectionBlock;
class SheenBlock;
using ReflectionBlockPtr = std::shared_ptr<ReflectionBlock>;
using SheenBlockPtr      = std::shared_ptr<SheenBlock>;

/**
 * @brief Block used to implement the sheen module of the PBR material.
 */
class BABYLON_SHARED_EXPORT SheenBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static SheenBlockPtr New(Ts&&... args)
  {
    auto sheenBlock = std::shared_ptr<SheenBlock>(new SheenBlock(std::forward<Ts>(args)...));
    SheenBlock::RegisterConnections(sheenBlock);

    return sheenBlock;
  }
  ~SheenBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const SheenBlockPtr& sheenBlock);

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Gets the main code of the block (fragment side).
   * @param reflectionBlock instance of a ReflectionBlock null if the code must be generated without
   * an active reflection module
   * @returns the shader code
   */
  std::string getCode(const ReflectionBlockPtr& reflectionBlock) const;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      const SubMeshPtr& subMesh = nullptr) override;

protected:
  /**
   * @brief Creates a new SheenBlock.
   * @param name defines the block name
   */
  SheenBlock(const std::string& name);

  /**
   * @brief Gets the intensity input component.
   */
  NodeMaterialConnectionPointPtr& get_intensity();

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the roughness input component.
   */
  NodeMaterialConnectionPointPtr& get_roughness();

  /**
   * @brief Gets the texture input component.
   */
  NodeMaterialConnectionPointPtr& get_texture();

  /**
   * @brief Gets the sheen object output component.
   */
  NodeMaterialConnectionPointPtr& get_sheen();

  /**
   * @brief Hidden
   */
  SheenBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

  /**
   * @brief Serializes this block in a JSON representation.
   * @returns the serialized block object
   */
  json serialize() const override;

  /**
   * @brief Hidden
   */
  void _deserialize(const json& serializationObject, Scene* scene,
                    const std::string& rootUrl) override;

public:
  /**
   * If true, the sheen effect is layered above the base BRDF with the albedo-scaling technique.
   * It allows the strength of the sheen effect to not depend on the base color of the material,
   * making it easier to setup and tweak the effect
   */
  // @editableInPropertyPage("Albedo scaling", PropertyTypeForEdition.Boolean, "PROPERTIES", {
  // "notifiers": { "update": true }})
  bool albedoScaling;

  /**
   * Defines if the sheen is linked to the sheen color.
   */
  // @editableInPropertyPage("Link sheen with albedo", PropertyTypeForEdition.Boolean, "PROPERTIES",
  // { "notifiers": { "update": true }})
  bool linkSheenWithAlbedo;

  /**
   * Gets the intensity input component
   */
  ReadOnlyProperty<SheenBlock, NodeMaterialConnectionPointPtr> intensity;

  /**
   * Gets the color input component
   */
  ReadOnlyProperty<SheenBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the roughness input component
   */
  ReadOnlyProperty<SheenBlock, NodeMaterialConnectionPointPtr> roughness;

  /**
   * Gets the texture input component
   */
  ReadOnlyProperty<SheenBlock, NodeMaterialConnectionPointPtr> texture;

  /**
   * Gets the sheen object output component
   */
  ReadOnlyProperty<SheenBlock, NodeMaterialConnectionPointPtr> sheen;

}; // end of class SheenBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_SHEEN_BLOCK_H
