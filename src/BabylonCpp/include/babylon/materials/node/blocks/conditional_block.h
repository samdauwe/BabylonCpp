#ifndef BABYLON_MATERIALS_NODE_BLOCKS_CONDITIONAL_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_CONDITIONAL_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(ConditionalBlock)

/**
 * @brief Operations supported by the ConditionalBlock block.
 */
enum class ConditionalBlockConditions {
  /** Equal */
  Equal,
  /** NotEqual */
  NotEqual,
  /** LessThan */
  LessThan,
  /** GreaterThan */
  GreaterThan,
  /** LessOrEqual */
  LessOrEqual,
  /** GreaterOrEqual */
  GreaterOrEqual,
  /** Logical Exclusive OR */
  Xor,
  /** Logical Or */
  Or,
  /** Logical And */
  And
}; // end of enum class ConditionalBlockConditions

/**
 * @brief Block used to apply conditional operation between floats
 */
class BABYLON_SHARED_EXPORT ConditionalBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ConditionalBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ConditionalBlock>(new ConditionalBlock(std::forward<Ts>(args)...));
  }
  ~ConditionalBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Exposes the properties to the Serialize?
   */
  json serialize() const override;

  /**
   * @brief Exposes the properties to the deserialize?
   */
  void _deserialize(const json& serializationObject, Scene* scene,
                    const std::string& rootUrl) override;

protected:
  /**
   * @brief Creates a new ConditionalBlock.
   * @param name defines the block name
   */
  ConditionalBlock(const std::string& name);

  /**
   * @brief Gets the first operand component.
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Gets the second operand component.
   */
  NodeMaterialConnectionPointPtr& get_b();

  /**
   * @brief Gets the value to return if condition is true.
   */
  NodeMaterialConnectionPointPtr& get__true();

  /**
   * @brief Gets the value to return if condition is false.
   */
  NodeMaterialConnectionPointPtr& get__false();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ConditionalBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Exposes the properties to the UI?
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets or sets the condition applied by the block
   */
  ConditionalBlockConditions condition;

  /**
   * Gets the first operand component
   */
  ReadOnlyProperty<ConditionalBlock, NodeMaterialConnectionPointPtr> a;

  /**
   * Gets the second operand component
   */
  ReadOnlyProperty<ConditionalBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets the value to return if condition is true
   */
  ReadOnlyProperty<ConditionalBlock, NodeMaterialConnectionPointPtr> _true;

  /**
   *  Gets the value to return if condition is false
   */
  ReadOnlyProperty<ConditionalBlock, NodeMaterialConnectionPointPtr> _false;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ConditionalBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ConditionalBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_CONDITIONAL_BLOCK_H
