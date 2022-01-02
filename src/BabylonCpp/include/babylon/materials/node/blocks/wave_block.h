#ifndef BABYLON_MATERIALS_NODE_BLOCKS_WAVE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_WAVE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class WaveBlock;
using WaveBlockPtr = std::shared_ptr<WaveBlock>;

/**
 * @brief Operations supported by the Wave block.
 */
enum class BABYLON_SHARED_EXPORT WaveBlockKind {
  /** SawTooth */
  SawTooth,
  /** Square */
  Square,
  /** Triangle */
  Triangle
}; // end of enum class WaveBlockKind

/**
 * @brief Block used to apply wave operation to floats.
 */
class BABYLON_SHARED_EXPORT WaveBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static WaveBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<WaveBlock>(new WaveBlock(std::forward<Ts>(args)...));
  }
  ~WaveBlock() override;

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
   * @brief Creates a new WaveBlock.
   * @param name defines the block name
   */
  WaveBlock(const std::string& name);

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
  WaveBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets or sets the kind of wave to be applied by the block
   */
  WaveBlockKind kind;

  /**
   * Gets the input component
   */
  ReadOnlyProperty<WaveBlock, NodeMaterialConnectionPointPtr> input;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<WaveBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class WaveBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_WAVE_BLOCK_H
