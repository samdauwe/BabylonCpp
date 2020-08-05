#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTIVITY_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTIVITY_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class ReflectivityBlock;
using ReflectivityBlockPtr = std::shared_ptr<ReflectivityBlock>;

/**
 * @brief Block used to implement the reflectivity module of the PBR material.
 */
class BABYLON_SHARED_EXPORT ReflectivityBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ReflectivityBlockPtr New(Ts&&... args)
  {
    auto reflectivityBlock
      = std::shared_ptr<ReflectivityBlock>(new ReflectivityBlock(std::forward<Ts>(args)...));
    ReflectivityBlock::RegisterConnections(reflectivityBlock);

    return reflectivityBlock;
  }
  ~ReflectivityBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const ReflectivityBlockPtr& reflectivityBlock);

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
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   * @param subMesh defines the submesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

  /**
   * @brief Gets the main code of the block (fragment side).
   * @param state current state of the node material building
   * @param aoIntensityVarName name of the variable with the ambient occlusion intensity
   * @returns the shader code
   */
  std::string getCode(NodeMaterialBuildState& state, const std::string& aoIntensityVarName);

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
   * @brief Serializes this block in a JSON representation.
   * @returns the serialized block object
   */
  json serialize() const override;

  /**
   * @brief Hidden
   */
  void _deserialize(const json& serializationObject, Scene* scene,
                    const std::string& rootUrl) override;

protected:
  /**
   * @brief Creates a new ReflectivityBlock.
   * @param name defines the block name
   */
  ReflectivityBlock(const std::string& name);

  /**
   * @brief Gets the metallic input component.
   */
  NodeMaterialConnectionPointPtr& get_metallic();

  /**
   * @brief Gets the roughness input component.
   */
  NodeMaterialConnectionPointPtr& get_roughness();

  /**
   * @brief Gets the texture input component.
   */
  NodeMaterialConnectionPointPtr& get_texture();

  /**
   * @brief Gets the reflectivity object output component.
   */
  NodeMaterialConnectionPointPtr& get_reflectivity();

  /**
   * @brief Hidden
   */
  ReflectivityBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /** @hidden */
  std::string _vMetallicReflectanceFactorsName;

  /**
   * The property below is set by the main PBR block prior to calling methods of this class.
   */

  /** @hidden */
  NodeMaterialConnectionPointPtr indexOfRefractionConnectionPoint;

  /**
   * Specifies if the metallic texture contains the ambient occlusion information in its red
   * channel.
   */
  // @editableInPropertyPage("AO from red channel", PropertyTypeForEdition.Boolean, "METALLIC
  // WORKFLOW", { "notifiers": { "update": true }})
  bool useAmbientOcclusionFromMetallicTextureRed;

  /**
   * Specifies if the metallic texture contains the metallness information in its blue channel.
   */
  // @editableInPropertyPage("Metallness from blue channel", PropertyTypeForEdition.Boolean,
  // "METALLIC WORKFLOW", { "notifiers": { "update": true }})
  bool useMetallnessFromMetallicTextureBlue;

  /**
   * Specifies if the metallic texture contains the roughness information in its alpha channel.
   */
  // @editableInPropertyPage("Roughness from alpha channel", PropertyTypeForEdition.Boolean,
  // "METALLIC WORKFLOW", { "notifiers": { "update": true }})
  bool useRoughnessFromMetallicTextureAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its green channel.
   */
  // @editableInPropertyPage("Roughness from green channel", PropertyTypeForEdition.Boolean,
  // "METALLIC WORKFLOW", { "notifiers": { "update": true }})
  bool useRoughnessFromMetallicTextureGreen;

  /**
   * Gets the metallic input component
   */
  ReadOnlyProperty<ReflectivityBlock, NodeMaterialConnectionPointPtr> metallic;

  /**
   * Gets the roughness input component
   */
  ReadOnlyProperty<ReflectivityBlock, NodeMaterialConnectionPointPtr> roughness;

  /**
   * Gets the texture input component
   */
  ReadOnlyProperty<ReflectivityBlock, NodeMaterialConnectionPointPtr> texture;

  /**
   * Gets the reflectivity object output component
   */
  ReadOnlyProperty<ReflectivityBlock, NodeMaterialConnectionPointPtr> reflectivity;

private:
  Color3 _metallicReflectanceColor;
  float _metallicF0Factor;

}; // end of class ReflectivityBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTIVITY_BLOCK_H
