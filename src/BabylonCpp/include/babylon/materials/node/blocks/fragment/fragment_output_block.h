#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(FragmentOutputBlock)

/**
 * @brief Block used to output the final color.
 */
class BABYLON_SHARED_EXPORT FragmentOutputBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static FragmentOutputBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<FragmentOutputBlock>(new FragmentOutputBlock(std::forward<Ts>(args)...));
  }
  ~FragmentOutputBlock() override;

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
   * @param subMesh defines which submesh to render
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
   * @brief Creates a new FragmentOutputBlock.
   * @param name defines the block name
   */
  FragmentOutputBlock(const std::string& name);

  /**
   * @brief Gets the rgba input component.
   */
  NodeMaterialConnectionPointPtr& get_rgba();

  /**
   * @brief Gets the rgb input component.
   */
  NodeMaterialConnectionPointPtr& get_rgb();

  /**
   * @brief Gets the a input component.
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Hidden
   */
  FragmentOutputBlock& _buildBlock(NodeMaterialBuildState& state) override;

  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets a boolean indicating if content needs to be converted to gamma space
   */
  // clang-format off
  // @editableInPropertyPage("Convert to gamma space", PropertyTypeForEdition.Boolean, "PROPERTIES", { "notifiers": { "update": true }})
  // clang-format on
  bool convertToGammaSpace;

  /**
   * Gets or sets a boolean indicating if content needs to be converted to linear space
   */
  // clang-format off
  // @editableInPropertyPage("Convert to linear space", PropertyTypeForEdition.Boolean, "PROPERTIES", { "notifiers": { "update": true }})
  // clang-format on
  bool convertToLinearSpace;

  /**
   * Gets the rgba input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the a input component
   */
  ReadOnlyProperty<FragmentOutputBlock, NodeMaterialConnectionPointPtr> a;

private:
  std::string _linearDefineName;
  std::string _gammaDefineName;

}; // end of class FragmentOutputBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_FRAGMENT_OUTPUT_BLOCK_H
