#ifndef BABYLON_MATERIALS_NODE_BLOCKS_TRANSFORM_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_TRANSFORM_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class TransformBlock;
using TransformBlockPtr = std::shared_ptr<TransformBlock>;

/**
 * @brief Block used to transform a vector (2, 3 or 4) with a matrix. It will generate a Vector4.
 */
class BABYLON_SHARED_EXPORT TransformBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static TransformBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<TransformBlock>(new TransformBlock(std::forward<Ts>(args)...));
  }
  ~TransformBlock() override;

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
   * @brief Creates a new TransformBlock.
   * @param name defines the block name
   */
  TransformBlock(const std::string& name);

  /**
   * @brief Gets the vector input.
   */
  NodeMaterialConnectionPointPtr& get_vector();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Gets the xyz output component.
   */
  NodeMaterialConnectionPointPtr& get_xyz();

  /**
   * @brief Gets the matrix transform input.
   */
  NodeMaterialConnectionPointPtr& get_transform();

  /**
   * @brief Hidden
   */
  TransformBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Defines the value to use to complement W value to transform it to a Vector4
   */
  float complementW;

  /**
   * Defines the value to use to complement z value to transform it to a Vector4
   */
  float complementZ;

  /**
   * Gets the vector input
   */
  ReadOnlyProperty<TransformBlock, NodeMaterialConnectionPointPtr> vector;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<TransformBlock, NodeMaterialConnectionPointPtr> output;

  /**
   * Gets the xyz output component
   */
  ReadOnlyProperty<TransformBlock, NodeMaterialConnectionPointPtr> xyz;

  /**
   * Gets the matrix transform input
   */
  ReadOnlyProperty<TransformBlock, NodeMaterialConnectionPointPtr> transform;

}; // end of class TransformBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_TRANSFORM_BLOCK_H
