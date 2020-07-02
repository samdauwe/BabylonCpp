#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_RAMP_GRADIENT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_RAMP_GRADIENT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ParticleRampGradientBlock;
using ParticleRampGradientBlockPtr = std::shared_ptr<ParticleRampGradientBlock>;

/**
 * @brief Block used for the particle ramp gradient section.
 */
class BABYLON_SHARED_EXPORT ParticleRampGradientBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ParticleRampGradientBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ParticleRampGradientBlock>(
      new ParticleRampGradientBlock(std::forward<Ts>(args)...));
  }
  ~ParticleRampGradientBlock() override;

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
   * @brief Creates a new ParticleRampGradientBlock.
   * @param name defines the block name
   */
  ParticleRampGradientBlock(const std::string& name);

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the rampColor output component.
   */
  NodeMaterialConnectionPointPtr& get_rampColor();

  /**
   * @brief Hidden
   */
  ParticleRampGradientBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the color input component
   */
  ReadOnlyProperty<ParticleRampGradientBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the rampColor output component
   */
  ReadOnlyProperty<ParticleRampGradientBlock, NodeMaterialConnectionPointPtr> rampColor;

}; // end of class ParticleRampGradientBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_RAMP_GRADIENT_BLOCK_H
