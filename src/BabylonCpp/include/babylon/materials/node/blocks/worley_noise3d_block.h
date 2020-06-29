#ifndef BABYLON_MATERIALS_NODE_BLOCKS_WORLEY_NOISE_3D_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_WORLEY_NOISE_3D_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class WorleyNoise3DBlock;
using WorleyNoise3DBlockPtr = std::shared_ptr<WorleyNoise3DBlock>;

/**
 * @brief Block used to Generate a Worley Noise 3D Noise Pattern.
 *
 * Source: https://github.com/Erkaman/glsl-worley
 * Converted to BJS by Pryme8
 *
 * Worley Noise 3D
 * Return vec2 value range of -1.0->1.0, F1-F2 respectively
 */
class BABYLON_SHARED_EXPORT WorleyNoise3DBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static WorleyNoise3DBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<WorleyNoise3DBlock>(new WorleyNoise3DBlock(std::forward<Ts>(args)...));
  }
  ~WorleyNoise3DBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Exposes the properties to the UI?
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

protected:
  /**
   * @brief Creates a new WorleyNoise3DBlock.
   * @param name defines the block name
   */
  WorleyNoise3DBlock(const std::string& name);

  /**
   * @brief Gets the seed input component.
   */
  NodeMaterialConnectionPointPtr& get_seed();

  /**
   * @brief Gets the jitter input component.
   */
  NodeMaterialConnectionPointPtr& get_jitter();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  WorleyNoise3DBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets or sets a boolean indicating that normal should be inverted on X axis
   */
  // @editableInPropertyPage("Use Manhattan Distance", PropertyTypeForEdition.Boolean, "PROPERTIES",
  // { "notifiers": { "update": false }})
  bool manhattanDistance;

  /**
   * Gets the seed input component
   */
  ReadOnlyProperty<WorleyNoise3DBlock, NodeMaterialConnectionPointPtr> seed;

  /**
   * Gets the jitter input component
   */
  ReadOnlyProperty<WorleyNoise3DBlock, NodeMaterialConnectionPointPtr> jitter;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<WorleyNoise3DBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class WorleyNoise3DBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_WORLEY_NOISE_3D_BLOCK_H
