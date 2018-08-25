#ifndef BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H
#define BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

/**
 * @brief SimplePineGenerator uses a tube to generate a tree.
 */
struct BABYLON_SHARED_EXPORT SimplePineGenerator {

  /**
   * @brief Simple Pine Generator function.
   *
   * @param scene - BABYLON scene.
   * @param trunkMaterial - material used for trunk.
   * @param leafMaterial - material for canopies.
   * @param canopies - number of leaf sections.
   * @param baseRadius
   * @param height - height of tree.
   * @param tessellation
   * @param twist
   * @return the mesh representing the tree
   */
  static MeshPtr CreateTree(Scene* scene, const MaterialPtr& trunkMaterial,
                            const MaterialPtr& leafMaterial,
                            unsigned int canopies = 7, float baseRadius = 1.8f,
                            float height = 75.f, unsigned int tessellation = 6,
                            float twist = 0.6f);

}; // end of class SimplePineGenerator

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H
