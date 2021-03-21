#ifndef BABYLON_MATERIALS_NODE_BLOCKS_CLAMP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_CLAMP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(ClampBlock)

/**
 * @brief Block used to clamp a float.
 */
class BABYLON_SHARED_EXPORT ClampBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ClampBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ClampBlock>(new ClampBlock(std::forward<Ts>(args)...));
  }
  ~ClampBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

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
   * @brief Creates a new ClampBlock.
   * @param name defines the block name
   */
  ClampBlock(const std::string& name);

  /**
   * @brief Gets the value input component.
   */
  NodeMaterialConnectionPointPtr& get_value();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ClampBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets the minimum range
   */
  // @editableInPropertyPage("Minimum", PropertyTypeForEdition.Float)
  float minimum;

  /**
   * Gets or sets the maximum range
   */
  // @editableInPropertyPage("Maximum", PropertyTypeForEdition.Float)
  float maximum;

  /**
   * Gets the right operand input component
   */
  ReadOnlyProperty<ClampBlock, NodeMaterialConnectionPointPtr> value;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ClampBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ClampBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_CLAMP_BLOCK_H
