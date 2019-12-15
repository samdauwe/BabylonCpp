#ifndef BABYLON_MATERIALS_NODE_BLOCKS_SIMPLEX_PERLIN_3D_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_SIMPLEX_PERLIN_3D_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class SimplexPerlin3DBlock;
using SimplexPerlin3DBlockPtr = std::shared_ptr<SimplexPerlin3DBlock>;

/**
 * @brief Block used to Generate a Simplex Perlin 3d Noise Pattern.
 *
 *  Wombat
 *  An efficient texture-free GLSL procedural noise library
 *  Source: https://github.com/BrianSharpe/Wombat
 *  Derived from: https://github.com/BrianSharpe/GPU-Noise-Lib
 *
 *  I'm not one for copyrights.  Use the code however you wish.
 *  All I ask is that credit be given back to the blog or myself when appropriate.
 *  And also to let me know if you come up with any changes, improvements, thoughts or interesting
 *  uses for this stuff. :) Thanks!
 *
 *  Brian Sharpe
 *  brisharpe CIRCLE_A yahoo DOT com
 *  http://briansharpe.wordpress.com
 *  https://github.com/BrianSharpe
 *
 *
 *  This is a modified version of Stefan Gustavson's and Ian McEwan's work at
 *  http://github.com/ashima/webgl-noise Modifications are...
 *  - faster random number generation
 *  - analytical final normalization
 *  - space scaled can have an approx feature size of 1.0
 *  - filter kernel changed to fix discontinuities at tetrahedron boundaries
 *
 *  Converted to BJS by Pryme8
 *
 *  Simplex Perlin Noise 3D
 *  Return value range of -1.0->1.0
 */
class BABYLON_SHARED_EXPORT SimplexPerlin3DBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static SimplexPerlin3DBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<SimplexPerlin3DBlock>(
      new SimplexPerlin3DBlock(std::forward<Ts>(args)...));
  }
  ~SimplexPerlin3DBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new SimplexPerlin3DBlock.
   * @param name defines the block name
   */
  SimplexPerlin3DBlock(const std::string& name);

  /**
   * @brief Gets the seed operand input component.
   */
  NodeMaterialConnectionPointPtr& get_seed();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  SimplexPerlin3DBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the seed operand input component
   */
  ReadOnlyProperty<SimplexPerlin3DBlock, NodeMaterialConnectionPointPtr> seed;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<SimplexPerlin3DBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class SimplexPerlin3DBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_SIMPLEX_PERLIN_3D_BLOCK_H
