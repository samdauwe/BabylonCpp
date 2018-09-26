#ifndef BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_GENERATOR_H
#define BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_GENERATOR_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Material;
class Mesh;
class Scene;
using MaterialPtr = std::shared_ptr<Material>;
using MeshPtr     = std::shared_ptr<Mesh>;

namespace Extensions {

/**
 * @brief QuickTreeGenerator uses a sphere with randomized vertices to generate
 * a tree.
 */
struct BABYLON_SHARED_EXPORT QuickTreeGenerator {

  /**
   * Quick Tree Generator function.
   *
   * @param sizeBranch - sphere radius used for branches and leaves 15 to 20.
   * @param sizeTrunk - height of trunk 10 to 15.
   * @param radius - radius of trunk 1 to 5.
   * @param trunkMaterial - material used for trunk.
   * @param leafMaterial - material for canopies.
   * @param scene - BABYLON scene.
   */
  static MeshPtr CreateTree(float sizeBranch, float sizeTrunk, float radius,
                            const MaterialPtr& trunkMaterial,
                            const MaterialPtr& leafMaterial, Scene* scene);

}; // end of struct QuickTreeGenerator

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_GENERATOR_H
