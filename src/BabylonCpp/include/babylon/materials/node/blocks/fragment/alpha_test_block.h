#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_ALPHA_TEST_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_ALPHA_TEST_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class AlphaTestBlock;
using AlphaTestBlockPtr = std::shared_ptr<AlphaTestBlock>;

/**
 * @brief Block used to add an alpha test in the fragment shader.
 */
class BABYLON_SHARED_EXPORT AlphaTestBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static AlphaTestBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<AlphaTestBlock>(new AlphaTestBlock(std::forward<Ts>(args)...));
  }
  ~AlphaTestBlock() override;

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
   * @brief Creates a new AlphaTestBlock.
   * @param name defines the block name
   */
  AlphaTestBlock(const std::string& name);

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the alpha input component.
   */
  NodeMaterialConnectionPointPtr& get_alpha();

  /**
   * @brief Hidden
   */
  AlphaTestBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets the alpha value where alpha testing happens
   */
  float alphaCutOff;

  /**
   * Gets the color input component
   */
  ReadOnlyProperty<AlphaTestBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the alpha input component
   */
  ReadOnlyProperty<AlphaTestBlock, NodeMaterialConnectionPointPtr> alpha;

}; // end of class AlphaTestBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_ALPHA_TEST_BLOCK_H
