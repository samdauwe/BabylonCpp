#ifndef BABYLON_MATERIALS_NODE_BLOCKS_TRIGONOMETRY_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_TRIGONOMETRY_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class TrigonometryBlock;
using TrigonometryBlockPtr = std::shared_ptr<TrigonometryBlock>;

/**
 * @brief Operations supported by the Trigonometry block.
 */
enum class BABYLON_SHARED_EXPORT TrigonometryBlockOperations {
  /** Cos */
  Cos,
  /** Sin */
  Sin,
  /** Abs */
  Abs,
  /** Exp */
  Exp,
  /** Exp2 */
  Exp2,
  /** Round */
  Round,
  /** Floor */
  Floor,
  /** Ceiling */
  Ceiling,
}; // end of enum class TrigonometryBlockOperations

/**
 * @brief Block used to apply trigonometry operation to floats.
 */
class BABYLON_SHARED_EXPORT TrigonometryBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static TrigonometryBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<TrigonometryBlock>(
      new TrigonometryBlock(std::forward<Ts>(args)...));
  }
  ~TrigonometryBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

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
   * @brief Creates a new TrigonometryBlock.
   * @param name defines the block name
   */
  TrigonometryBlock(const std::string& name);

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
  TrigonometryBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets or sets the operation applied by the block
   */
  TrigonometryBlockOperations operation;

  /**
   * Gets the input component
   */
  ReadOnlyProperty<TrigonometryBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<TrigonometryBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class TrigonometryBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_TRIGONOMETRY_BLOCK_H
