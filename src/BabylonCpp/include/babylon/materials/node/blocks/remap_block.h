#ifndef BABYLON_MATERIALS_NODE_BLOCKS_REMAP_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_REMAP_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

class RemapBlock;
using RemapBlockPtr = std::shared_ptr<RemapBlock>;

/**
 * @brief Block used to remap a float from a range to a new one.
 */
class BABYLON_SHARED_EXPORT RemapBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static RemapBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<RemapBlock>(new RemapBlock(std::forward<Ts>(args)...));
  }
  ~RemapBlock() override;

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
   * @brief Creates a new RemapBlock.
   * @param name defines the block name
   */
  RemapBlock(const std::string& name);

  /**
   * @brief Gets the input component.
   */
  NodeMaterialConnectionPointPtr& get_input();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  RemapBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets the source range
   */
  Vector2 sourceRange;

  /**
   * Gets or sets the target range
   */
  Vector2 targetRange;

  /**
   * Gets the input component
   */
  ReadOnlyProperty<RemapBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<RemapBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class RemapBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_REMAP_BLOCK_H
