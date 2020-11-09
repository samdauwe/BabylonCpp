#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_PERTURB_NORMAL_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_PERTURB_NORMAL_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class PerturbNormalBlock;
using PerturbNormalBlockPtr = std::shared_ptr<PerturbNormalBlock>;

/**
 * @brief Block used to pertub normals based on a normal map.
 */
class BABYLON_SHARED_EXPORT PerturbNormalBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static PerturbNormalBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<PerturbNormalBlock>(new PerturbNormalBlock(std::forward<Ts>(args)...));
  }
  ~PerturbNormalBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

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

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& nodeMaterial) override;

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
   * @brief Creates a new PerturbNormalBlock.
   * @param name defines the block name
   */
  PerturbNormalBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal();

  /**
   * @brief Gets the world tangent input component.
   */
  NodeMaterialConnectionPointPtr& get_worldTangent();

  /**
   * @brief Gets the uv input component.
   */
  NodeMaterialConnectionPointPtr& get_uv();

  /**
   * @brief Gets the normal map color input component.
   */
  NodeMaterialConnectionPointPtr& get_normalMapColor();

  /**
   * @brief Gets the strength input component.
   */
  NodeMaterialConnectionPointPtr& get_strength();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  PerturbNormalBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets a boolean indicating that normal should be inverted on X axis
   */
  // @editableInPropertyPage("Invert X axis", PropertyTypeForEdition.Boolean, "PROPERTIES", {
  // "notifiers": { "update": false }})
  bool invertX;

  /**
   * Gets or sets a boolean indicating that normal should be inverted on Y axis
   */
  // @editableInPropertyPage("Invert Y axis", PropertyTypeForEdition.Boolean, "PROPERTIES", {
  // "notifiers": { "update": false }})
  bool invertY;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the world tangent input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> worldTangent;

  /**
   * Gets the uv input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the normal map color input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> normalMapColor;

  /**
   * Gets the strength input component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> strength;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<PerturbNormalBlock, NodeMaterialConnectionPointPtr> output;

private:
  std::string _tangentSpaceParameterName;

}; // end of class PerturbNormalBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_PERTURB_NORMAL_BLOCK_H
