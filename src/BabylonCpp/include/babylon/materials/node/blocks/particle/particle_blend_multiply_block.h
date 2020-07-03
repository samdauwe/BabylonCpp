#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_BLEND_MULTIPLY_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_BLEND_MULTIPLY_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ParticleBlendMultiplyBlock;
using ParticleBlendMultiplyBlockPtr = std::shared_ptr<ParticleBlendMultiplyBlock>;

/**
 * @brief Block used for the particle blend multiply section.
 */
class BABYLON_SHARED_EXPORT ParticleBlendMultiplyBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ParticleBlendMultiplyBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ParticleBlendMultiplyBlock>(
      new ParticleBlendMultiplyBlock(std::forward<Ts>(args)...));
  }
  ~ParticleBlendMultiplyBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

protected:
  /**
   * @brief Creates a new ParticleBlendMultiplyBlock.
   * @param name defines the block name
   */
  ParticleBlendMultiplyBlock(const std::string& name);

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the alphaTexture input component.
   */
  NodeMaterialConnectionPointPtr& get_alphaTexture();

  /**
   * @brief Gets the alphaColor input component.
   */
  NodeMaterialConnectionPointPtr& get_alphaColor();

  /**
   * @brief Gets the blendColor output component.
   */
  NodeMaterialConnectionPointPtr& get_blendColor();

  /**
   * @brief Hidden
   */
  ParticleBlendMultiplyBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the color input component
   */
  ReadOnlyProperty<ParticleBlendMultiplyBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the alphaTexture input component
   */
  ReadOnlyProperty<ParticleBlendMultiplyBlock, NodeMaterialConnectionPointPtr> alphaTexture;

  /**
   * Gets the alphaColor input component
   */
  ReadOnlyProperty<ParticleBlendMultiplyBlock, NodeMaterialConnectionPointPtr> alphaColor;

  /**
   * Gets the blendColor output component
   */
  ReadOnlyProperty<ParticleBlendMultiplyBlock, NodeMaterialConnectionPointPtr> blendColor;

}; // end of class ParticleBlendMultiplyBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_BLEND_MULTIPLY_BLOCK_H
